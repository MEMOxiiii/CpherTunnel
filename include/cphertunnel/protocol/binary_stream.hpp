// =============================================================================
// CpherTunnel - Binary Stream (Read/Write)
// Handles all protocol-level serialization with Little-Endian byte order,
// VarInt encoding, and string prefixed with VarInt length.
// =============================================================================
#pragma once

#include "types.hpp"
#include "varint.hpp"
#include <cstring>
#include <stdexcept>
#include <type_traits>

namespace cphertunnel {

// =============================================================================
// ReadOnlyBinaryStream
// =============================================================================
class ReadOnlyBinaryStream {
public:
    ReadOnlyBinaryStream() = default;

    explicit ReadOnlyBinaryStream(std::string_view data)
        : data_(reinterpret_cast<const uint8_t*>(data.data()))
        , size_(data.size())
        , offset_(0) {}

    ReadOnlyBinaryStream(const uint8_t* data, size_t size)
        : data_(data), size_(size), offset_(0) {}

    ReadOnlyBinaryStream(const ByteBuffer& buf)
        : data_(buf.data()), size_(buf.size()), offset_(0) {}

    // ─── Status ──────────────────────────────────────────────────
    bool hasOverflowed() const { return overflowed_; }
    size_t getReadPointer() const { return offset_; }
    size_t getUnreadLength() const { return overflowed_ ? 0 : (size_ - offset_); }
    size_t getLength() const { return size_; }
    bool canRead(size_t n) const { return !overflowed_ && (offset_ + n <= size_); }

    const uint8_t* getData() const { return data_; }

    void setReadPointer(size_t pos) {
        if (pos > size_) { overflowed_ = true; return; }
        offset_ = pos;
    }

    // ─── Raw Read ────────────────────────────────────────────────
    void read(void* dest, size_t len) {
        if (!canRead(len)) { overflowed_ = true; return; }
        std::memcpy(dest, data_ + offset_, len);
        offset_ += len;
    }

    // ─── Primitive Reads (Little-Endian) ─────────────────────────
    bool readBool() {
        return readByte() != 0;
    }

    uint8_t readByte() {
        if (!canRead(1)) { overflowed_ = true; return 0; }
        return data_[offset_++];
    }

    int8_t readSignedByte() {
        return static_cast<int8_t>(readByte());
    }

    uint16_t readUnsignedShort() {
        uint16_t val = 0;
        read(&val, 2);
        return val; // LE native on x86
    }

    int16_t readSignedShort() {
        return static_cast<int16_t>(readUnsignedShort());
    }

    uint32_t readUnsignedInt() {
        uint32_t val = 0;
        read(&val, 4);
        return val;
    }

    int32_t readSignedInt() {
        return static_cast<int32_t>(readUnsignedInt());
    }

    int32_t readSignedBigEndianInt() {
        uint32_t val = readUnsignedInt();
        // Swap from LE to BE
        val = ((val >> 24) & 0xFF) |
              ((val >> 8) & 0xFF00) |
              ((val << 8) & 0xFF0000) |
              ((val << 24) & 0xFF000000);
        return static_cast<int32_t>(val);
    }

    uint64_t readUnsignedInt64() {
        uint64_t val = 0;
        read(&val, 8);
        return val;
    }

    int64_t readSignedInt64() {
        return static_cast<int64_t>(readUnsignedInt64());
    }

    float readFloat() {
        float val = 0;
        read(&val, 4);
        return val;
    }

    double readDouble() {
        double val = 0;
        read(&val, 8);
        return val;
    }

    // ─── VarInt Reads ────────────────────────────────────────────
    uint32_t readUnsignedVarInt() {
        if (overflowed_) return 0;
        try {
            return VarInt::readUnsignedVarInt(data_, size_, offset_);
        } catch (...) {
            overflowed_ = true;
            return 0;
        }
    }

    int32_t readVarInt() {
        if (overflowed_) return 0;
        try {
            return VarInt::readVarInt(data_, size_, offset_);
        } catch (...) {
            overflowed_ = true;
            return 0;
        }
    }

    uint64_t readUnsignedVarInt64() {
        if (overflowed_) return 0;
        try {
            return VarInt::readUnsignedVarInt64(data_, size_, offset_);
        } catch (...) {
            overflowed_ = true;
            return 0;
        }
    }

    int64_t readVarInt64() {
        if (overflowed_) return 0;
        try {
            return VarInt::readVarInt64(data_, size_, offset_);
        } catch (...) {
            overflowed_ = true;
            return 0;
        }
    }

    // ─── String Read ─────────────────────────────────────────────
    std::string readString() {
        uint32_t len = readUnsignedVarInt();
        if (overflowed_ || len > MAX_PACKET_SIZE) { overflowed_ = true; return {}; }
        if (!canRead(len)) { overflowed_ = true; return {}; }
        std::string result(reinterpret_cast<const char*>(data_ + offset_), len);
        offset_ += len;
        return result;
    }

    // ─── ByteArray Read ──────────────────────────────────────────
    ByteBuffer readByteArray() {
        uint32_t len = readUnsignedVarInt();
        if (overflowed_ || len > MAX_PACKET_SIZE) { overflowed_ = true; return {}; }
        if (!canRead(len)) { overflowed_ = true; return {}; }
        ByteBuffer result(data_ + offset_, data_ + offset_ + len);
        offset_ += len;
        return result;
    }

    // ─── Compound Types ──────────────────────────────────────────
    Vec3 readVec3() {
        Vec3 v;
        v.x = readFloat();
        v.y = readFloat();
        v.z = readFloat();
        return v;
    }

    Vec2 readVec2() {
        Vec2 v;
        v.x = readFloat();
        v.y = readFloat();
        return v;
    }

    BlockPos readBlockPos() {
        BlockPos p;
        p.x = readVarInt();
        p.y = readVarInt();
        p.z = readVarInt();
        return p;
    }

    NetworkBlockPos readNetworkBlockPos() {
        NetworkBlockPos p;
        p.x = readVarInt();
        p.y = readUnsignedVarInt();
        p.z = readVarInt();
        return p;
    }

    UUID readUUID() {
        UUID u;
        u.most_significant = readUnsignedInt64();
        u.least_significant = readUnsignedInt64();
        return u;
    }

    ActorUniqueID readActorUniqueID() {
        return {readVarInt64()};
    }

    ActorRuntimeID readActorRuntimeID() {
        return {readUnsignedVarInt64()};
    }

    // Read remaining bytes
    ByteBuffer readRemainingBytes() {
        if (overflowed_ || offset_ >= size_) return {};
        ByteBuffer result(data_ + offset_, data_ + size_);
        offset_ = size_;
        return result;
    }

protected:
    const uint8_t* data_ = nullptr;
    size_t         size_ = 0;
    size_t         offset_ = 0;
    bool           overflowed_ = false;
};

// =============================================================================
// BinaryStream (read + write)
// =============================================================================
class BinaryStream : public ReadOnlyBinaryStream {
public:
    BinaryStream() {
        updateView();
    }

    explicit BinaryStream(size_t reserve) {
        buffer_.reserve(reserve);
        updateView();
    }

    // ─── Buffer Access ───────────────────────────────────────────
    const std::string& getBuffer() const { return buffer_; }
    std::string&& moveBuffer() { return std::move(buffer_); }
    size_t getWriteLength() const { return buffer_.size(); }

    void reset() {
        buffer_.clear();
        offset_ = 0;
        overflowed_ = false;
        updateView();
    }

    // ─── Raw Write ───────────────────────────────────────────────
    void write(const void* data, size_t len) {
        buffer_.append(reinterpret_cast<const char*>(data), len);
        updateView();
    }

    // ─── Primitive Writes (Little-Endian) ────────────────────────
    void writeBool(bool value) {
        writeByte(value ? 1 : 0);
    }

    void writeByte(uint8_t value) {
        buffer_.push_back(static_cast<char>(value));
        updateView();
    }

    void writeSignedByte(int8_t value) {
        writeByte(static_cast<uint8_t>(value));
    }

    void writeUnsignedShort(uint16_t value) {
        write(&value, 2);
    }

    void writeSignedShort(int16_t value) {
        write(&value, 2);
    }

    void writeUnsignedInt(uint32_t value) {
        write(&value, 4);
    }

    void writeSignedInt(int32_t value) {
        write(&value, 4);
    }

    void writeSignedBigEndianInt(int32_t value) {
        uint32_t v = static_cast<uint32_t>(value);
        v = ((v >> 24) & 0xFF) |
            ((v >> 8) & 0xFF00) |
            ((v << 8) & 0xFF0000) |
            ((v << 24) & 0xFF000000);
        write(&v, 4);
    }

    void writeUnsignedInt64(uint64_t value) {
        write(&value, 8);
    }

    void writeSignedInt64(int64_t value) {
        write(&value, 8);
    }

    void writeFloat(float value) {
        write(&value, 4);
    }

    void writeDouble(double value) {
        write(&value, 8);
    }

    // ─── VarInt Writes ───────────────────────────────────────────
    void writeUnsignedVarInt(uint32_t value) {
        VarInt::writeUnsignedVarInt(buffer_, value);
        updateView();
    }

    void writeVarInt(int32_t value) {
        VarInt::writeVarInt(buffer_, value);
        updateView();
    }

    void writeUnsignedVarInt64(uint64_t value) {
        VarInt::writeUnsignedVarInt64(buffer_, value);
        updateView();
    }

    void writeVarInt64(int64_t value) {
        VarInt::writeVarInt64(buffer_, value);
        updateView();
    }

    // ─── String Write ────────────────────────────────────────────
    void writeString(std::string_view value) {
        writeUnsignedVarInt(static_cast<uint32_t>(value.size()));
        write(value.data(), value.size());
    }

    void writeByteArray(const ByteBuffer& data) {
        writeUnsignedVarInt(static_cast<uint32_t>(data.size()));
        write(data.data(), data.size());
    }

    void writeRawBytes(const ByteBuffer& data) {
        write(data.data(), data.size());
    }

    void writeRawBytes(const std::string& data) {
        write(data.data(), data.size());
    }

    // ─── Compound Types ──────────────────────────────────────────
    void writeVec3(const Vec3& v) {
        writeFloat(v.x);
        writeFloat(v.y);
        writeFloat(v.z);
    }

    void writeVec2(const Vec2& v) {
        writeFloat(v.x);
        writeFloat(v.y);
    }

    void writeBlockPos(const BlockPos& p) {
        writeVarInt(p.x);
        writeVarInt(p.y);
        writeVarInt(p.z);
    }

    void writeNetworkBlockPos(const NetworkBlockPos& p) {
        writeVarInt(p.x);
        writeUnsignedVarInt(p.y);
        writeVarInt(p.z);
    }

    void writeUUID(const UUID& u) {
        writeUnsignedInt64(u.most_significant);
        writeUnsignedInt64(u.least_significant);
    }

    void writeActorUniqueID(const ActorUniqueID& id) {
        writeVarInt64(id.id);
    }

    void writeActorRuntimeID(const ActorRuntimeID& id) {
        writeUnsignedVarInt64(id.id);
    }

private:
    void updateView() {
        data_ = reinterpret_cast<const uint8_t*>(buffer_.data());
        size_ = buffer_.size();
    }

    std::string buffer_;
};

} // namespace cphertunnel
