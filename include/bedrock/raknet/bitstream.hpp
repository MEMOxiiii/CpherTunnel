// =============================================================================
// BedrockProtocol - RakNet BitStream
// Bit-level read/write stream for RakNet protocol
// =============================================================================
#pragma once

#include "types.hpp"
#include <vector>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <cassert>

namespace bedrock::raknet {

// =============================================================================
// BitStream - Bit-level read/write buffer
// Used for encoding/decoding RakNet frame headers and payloads
// =============================================================================
class BitStream {
public:
    BitStream() {
        data_.reserve(BITSTREAM_STACK_ALLOCATION_SIZE);
    }
    
    explicit BitStream(size_t initial_bytes) {
        data_.resize(initial_bytes, 0);
    }
    
    BitStream(const uint8_t* data, size_t length) 
        : data_(data, data + length)
        , write_bit_offset_(length * 8) {
    }
    
    // Copy from vector
    explicit BitStream(const std::vector<uint8_t>& data) 
        : data_(data)
        , write_bit_offset_(data.size() * 8) {
    }
    
    // =========================================================================
    // Write operations
    // =========================================================================
    
    void writeBit(bool value) {
        ensureWriteCapacity(1);
        size_t byte_idx = write_bit_offset_ >> 3;
        size_t bit_idx = write_bit_offset_ & 7;
        
        if (byte_idx >= data_.size()) {
            data_.push_back(0);
        }
        
        if (value) {
            data_[byte_idx] |= (0x80 >> bit_idx);
        } else {
            data_[byte_idx] &= ~(0x80 >> bit_idx);
        }
        write_bit_offset_++;
    }
    
    void writeBits(const uint8_t* input, size_t num_bits, bool right_aligned = true) {
        if (num_bits == 0) return;
        
        // If byte-aligned and writing full bytes, optimise
        if ((write_bit_offset_ & 7) == 0 && (num_bits & 7) == 0) {
            ensureWriteCapacity(num_bits);
            size_t byte_idx = write_bit_offset_ >> 3;
            size_t num_bytes = num_bits >> 3;
            while (data_.size() < byte_idx + num_bytes) data_.push_back(0);
            std::memcpy(data_.data() + byte_idx, input, num_bytes);
            write_bit_offset_ += num_bits;
            return;
        }
        
        // Bit-by-bit write
        size_t full_bytes = num_bits >> 3;
        size_t remaining_bits = num_bits & 7;
        
        for (size_t i = 0; i < full_bytes; ++i) {
            writeUint8(input[i]);
        }
        
        if (remaining_bits > 0) {
            uint8_t last_byte = input[full_bytes];
            if (right_aligned) {
                last_byte <<= (8 - remaining_bits);
            }
            for (size_t i = 0; i < remaining_bits; ++i) {
                writeBit((last_byte & (0x80 >> i)) != 0);
            }
        }
    }
    
    void writeUint8(uint8_t value) {
        writeBits(&value, 8);
    }
    
    void writeUint16BE(uint16_t value) {
        uint8_t buf[2];
        buf[0] = static_cast<uint8_t>((value >> 8) & 0xFF);
        buf[1] = static_cast<uint8_t>(value & 0xFF);
        writeBits(buf, 16);
    }
    
    void writeUint16LE(uint16_t value) {
        uint8_t buf[2];
        buf[0] = static_cast<uint8_t>(value & 0xFF);
        buf[1] = static_cast<uint8_t>((value >> 8) & 0xFF);
        writeBits(buf, 16);
    }
    
    void writeUint24LE(uint32_t value) {
        uint8_t buf[3];
        bedrock::raknet::writeUint24LE(buf, value);
        writeBits(buf, 24);
    }
    
    void writeUint32BE(uint32_t value) {
        uint8_t buf[4];
        for (int i = 3; i >= 0; --i) {
            buf[3 - i] = static_cast<uint8_t>((value >> (i * 8)) & 0xFF);
        }
        writeBits(buf, 32);
    }
    
    void writeUint64BE(uint64_t value) {
        uint8_t buf[8];
        bedrock::raknet::writeInt64BE(buf, value);
        writeBits(buf, 64);
    }
    
    void writeBytes(const uint8_t* src, size_t length) {
        writeBits(src, length * 8);
    }
    
    void writeBytes(const std::vector<uint8_t>& src) {
        writeBytes(src.data(), src.size());
    }
    
    void writeMagic() {
        writeBytes(OFFLINE_MESSAGE_MAGIC, MAGIC_SIZE);
    }
    
    void writeAddress(const SystemAddress& addr) {
        std::vector<uint8_t> temp;
        addr.writeTo(temp);
        writeBytes(temp.data(), temp.size());
    }
    
    void writeString(const std::string& str) {
        writeUint16BE(static_cast<uint16_t>(str.size()));
        writeBytes(reinterpret_cast<const uint8_t*>(str.data()), str.size());
    }
    
    void padToByteAlignment() {
        size_t remainder = write_bit_offset_ & 7;
        if (remainder > 0) {
            size_t pad = 8 - remainder;
            for (size_t i = 0; i < pad; ++i) {
                writeBit(false);
            }
        }
    }
    
    // =========================================================================
    // Read operations
    // =========================================================================
    
    bool readBit() {
        if (read_bit_offset_ >= write_bit_offset_) {
            throw std::runtime_error("BitStream: read past end");
        }
        size_t byte_idx = read_bit_offset_ >> 3;
        size_t bit_idx = read_bit_offset_ & 7;
        read_bit_offset_++;
        return (data_[byte_idx] & (0x80 >> bit_idx)) != 0;
    }
    
    void readBits(uint8_t* output, size_t num_bits, bool right_aligned = true) {
        if (num_bits == 0) return;
        
        // Byte-aligned fast path
        if ((read_bit_offset_ & 7) == 0 && (num_bits & 7) == 0) {
            size_t byte_idx = read_bit_offset_ >> 3;
            size_t num_bytes = num_bits >> 3;
            if (byte_idx + num_bytes > data_.size()) {
                throw std::runtime_error("BitStream: read past end");
            }
            std::memcpy(output, data_.data() + byte_idx, num_bytes);
            read_bit_offset_ += num_bits;
            return;
        }
        
        size_t full_bytes = num_bits >> 3;
        size_t remaining_bits = num_bits & 7;
        
        for (size_t i = 0; i < full_bytes; ++i) {
            output[i] = readUint8();
        }
        
        if (remaining_bits > 0) {
            uint8_t val = 0;
            for (size_t i = 0; i < remaining_bits; ++i) {
                if (readBit()) {
                    val |= (0x80 >> i);
                }
            }
            if (right_aligned) {
                val >>= (8 - remaining_bits);
            }
            output[full_bytes] = val;
        }
    }
    
    uint8_t readUint8() {
        uint8_t val = 0;
        readBits(&val, 8);
        return val;
    }
    
    uint16_t readUint16BE() {
        uint8_t buf[2];
        readBits(buf, 16);
        return (static_cast<uint16_t>(buf[0]) << 8) | buf[1];
    }
    
    uint16_t readUint16LE() {
        uint8_t buf[2];
        readBits(buf, 16);
        return buf[0] | (static_cast<uint16_t>(buf[1]) << 8);
    }
    
    uint32_t readUint24LE() {
        uint8_t buf[3];
        readBits(buf, 24);
        return bedrock::raknet::readUint24LE(buf);
    }
    
    uint32_t readUint32BE() {
        uint8_t buf[4];
        readBits(buf, 32);
        return (static_cast<uint32_t>(buf[0]) << 24) |
               (static_cast<uint32_t>(buf[1]) << 16) |
               (static_cast<uint32_t>(buf[2]) << 8) |
               buf[3];
    }
    
    uint64_t readUint64BE() {
        uint8_t buf[8];
        readBits(buf, 64);
        return bedrock::raknet::readInt64BE(buf);
    }
    
    std::vector<uint8_t> readBytes(size_t count) {
        std::vector<uint8_t> result(count);
        readBits(result.data(), count * 8);
        return result;
    }
    
    bool readMagicAndValidate() {
        auto magic = readBytes(MAGIC_SIZE);
        return std::memcmp(magic.data(), OFFLINE_MESSAGE_MAGIC, MAGIC_SIZE) == 0;
    }
    
    SystemAddress readAddress() {
        size_t offset = read_bit_offset_ >> 3;
        SystemAddress addr = SystemAddress::readFrom(data_.data(), offset);
        read_bit_offset_ = offset * 8;
        return addr;
    }
    
    std::string readString() {
        uint16_t len = readUint16BE();
        auto bytes = readBytes(len);
        return std::string(reinterpret_cast<const char*>(bytes.data()), len);
    }
    
    void skipToByteAlignment() {
        size_t remainder = read_bit_offset_ & 7;
        if (remainder > 0) {
            read_bit_offset_ += (8 - remainder);
        }
    }
    
    // =========================================================================
    // State
    // =========================================================================
    
    size_t getWriteBitOffset() const { return write_bit_offset_; }
    size_t getReadBitOffset() const { return read_bit_offset_; }
    size_t getWriteByteOffset() const { return (write_bit_offset_ + 7) >> 3; }
    size_t getReadByteOffset() const { return read_bit_offset_ >> 3; }
    size_t getBitsAvailable() const { return write_bit_offset_ - read_bit_offset_; }
    size_t getBytesAvailable() const { return getBitsAvailable() >> 3; }
    bool hasDataToRead() const { return read_bit_offset_ < write_bit_offset_; }
    
    void resetRead() { read_bit_offset_ = 0; }
    void resetWrite() { write_bit_offset_ = 0; }
    void reset() { read_bit_offset_ = 0; write_bit_offset_ = 0; data_.clear(); }
    
    const uint8_t* getData() const { return data_.data(); }
    uint8_t* getData() { return data_.data(); }
    size_t getDataSize() const { return (write_bit_offset_ + 7) >> 3; }
    
    std::vector<uint8_t> toVector() const {
        return std::vector<uint8_t>(data_.begin(), data_.begin() + getDataSize());
    }
    
    // Get remaining unread data as vector
    std::vector<uint8_t> getRemainingData() const {
        size_t byte_offset = read_bit_offset_ >> 3;
        size_t total_bytes = getDataSize();
        if (byte_offset >= total_bytes) return {};
        return std::vector<uint8_t>(data_.begin() + byte_offset, data_.begin() + total_bytes);
    }
    
private:
    void ensureWriteCapacity(size_t bits_needed) {
        size_t total_bits = write_bit_offset_ + bits_needed;
        size_t bytes_needed = (total_bits + 7) >> 3;
        if (bytes_needed > data_.size()) {
            data_.resize(bytes_needed, 0);
        }
    }
    
    std::vector<uint8_t> data_;
    size_t write_bit_offset_ = 0;
    size_t read_bit_offset_ = 0;
};

} // namespace bedrock::raknet
