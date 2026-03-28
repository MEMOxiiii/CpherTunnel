// =============================================================================
// CpherTunnel - VarInt Encoding/Decoding
// Standard Minecraft VarInt (7-bit groups, MSB continuation)
// ZigZag encoding for signed types
// =============================================================================
#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

namespace cphertunnel {

class VarInt {
public:
    // ─── Unsigned VarInt (32-bit) ────────────────────────────────
    static void writeUnsignedVarInt(std::string& buffer, uint32_t value) {
        do {
            uint8_t byte = value & 0x7F;
            value >>= 7;
            if (value) byte |= 0x80;
            buffer.push_back(static_cast<char>(byte));
        } while (value);
    }

    static uint32_t readUnsignedVarInt(const uint8_t* data, size_t len, size_t& offset) {
        uint32_t value = 0;
        int shift = 0;
        do {
            if (offset >= len) throw std::runtime_error("VarInt: buffer overflow");
            if (shift >= 35) throw std::runtime_error("VarInt: too many bytes");
            uint8_t byte = data[offset++];
            value |= static_cast<uint32_t>(byte & 0x7F) << shift;
            if ((byte & 0x80) == 0) break;
            shift += 7;
        } while (true);
        return value;
    }

    // ─── Unsigned VarInt64 (64-bit) ──────────────────────────────
    static void writeUnsignedVarInt64(std::string& buffer, uint64_t value) {
        do {
            uint8_t byte = value & 0x7F;
            value >>= 7;
            if (value) byte |= 0x80;
            buffer.push_back(static_cast<char>(byte));
        } while (value);
    }

    static uint64_t readUnsignedVarInt64(const uint8_t* data, size_t len, size_t& offset) {
        uint64_t value = 0;
        int shift = 0;
        do {
            if (offset >= len) throw std::runtime_error("VarInt64: buffer overflow");
            if (shift >= 70) throw std::runtime_error("VarInt64: too many bytes");
            uint8_t byte = data[offset++];
            value |= static_cast<uint64_t>(byte & 0x7F) << shift;
            if ((byte & 0x80) == 0) break;
            shift += 7;
        } while (true);
        return value;
    }

    // ─── Signed VarInt (ZigZag encoding) ─────────────────────────
    static void writeVarInt(std::string& buffer, int32_t value) {
        writeUnsignedVarInt(buffer, zigZagEncode32(value));
    }

    static int32_t readVarInt(const uint8_t* data, size_t len, size_t& offset) {
        return zigZagDecode32(readUnsignedVarInt(data, len, offset));
    }

    // ─── Signed VarInt64 (ZigZag encoding) ───────────────────────
    static void writeVarInt64(std::string& buffer, int64_t value) {
        writeUnsignedVarInt64(buffer, zigZagEncode64(value));
    }

    static int64_t readVarInt64(const uint8_t* data, size_t len, size_t& offset) {
        return zigZagDecode64(readUnsignedVarInt64(data, len, offset));
    }

    // ─── ZigZag Helpers ──────────────────────────────────────────
    static uint32_t zigZagEncode32(int32_t n) {
        return static_cast<uint32_t>((n >> 31) ^ (n << 1));
    }

    static int32_t zigZagDecode32(uint32_t n) {
        return static_cast<int32_t>((n >> 1) ^ -(n & 1));
    }

    static uint64_t zigZagEncode64(int64_t n) {
        return static_cast<uint64_t>((n >> 63) ^ (n << 1));
    }

    static int64_t zigZagDecode64(uint64_t n) {
        return static_cast<int64_t>((n >> 1) ^ -(n & 1));
    }

    // ─── Size calculation ────────────────────────────────────────
    static size_t unsignedVarIntSize(uint32_t value) {
        size_t size = 0;
        do { size++; value >>= 7; } while (value);
        return size;
    }

    static size_t unsignedVarInt64Size(uint64_t value) {
        size_t size = 0;
        do { size++; value >>= 7; } while (value);
        return size;
    }
};

} // namespace cphertunnel
