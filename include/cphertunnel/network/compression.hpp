// =============================================================================
// CpherTunnel - Network Compression
// ZLib and Snappy compression for Bedrock batch packets
// =============================================================================
#pragma once

#include "../protocol/types.hpp"
#include <vector>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <algorithm>

// Optional: If zlib is available, include it for real decompression
// Users must link against zlib themselves if they want ZLib support
#ifdef CPHERTUNNEL_HAS_ZLIB
#include <zlib.h>
#endif

namespace cphertunnel::network {

// =============================================================================
// Compression Algorithm - Matches protocol CompressionAlgorithm enum
// =============================================================================
using CompressionAlgorithm = cphertunnel::CompressionAlgorithm;

// =============================================================================
// Compression Interface
// =============================================================================
class Compression {
public:
    virtual ~Compression() = default;
    virtual std::vector<uint8_t> compress(const uint8_t* data, size_t length) = 0;
    virtual std::vector<uint8_t> decompress(const uint8_t* data, size_t length,
                                              size_t max_size = 10 * 1024 * 1024) = 0;
    virtual CompressionAlgorithm getAlgorithm() const = 0;
};

// =============================================================================
// No Compression (passthrough)
// =============================================================================
class NoCompression : public Compression {
public:
    std::vector<uint8_t> compress(const uint8_t* data, size_t length) override {
        return std::vector<uint8_t>(data, data + length);
    }
    
    std::vector<uint8_t> decompress(const uint8_t* data, size_t length,
                                      size_t /*max_size*/ = 10 * 1024 * 1024) override {
        return std::vector<uint8_t>(data, data + length);
    }
    
    CompressionAlgorithm getAlgorithm() const override {
        return CompressionAlgorithm::None;
    }
};

// =============================================================================
// ZLib Compression (Deflate)
// =============================================================================
#ifdef CPHERTUNNEL_HAS_ZLIB
class ZLibCompression : public Compression {
public:
    explicit ZLibCompression(int level = Z_DEFAULT_COMPRESSION, uint16_t threshold = 256)
        : level_(level), threshold_(threshold) {}
    
    std::vector<uint8_t> compress(const uint8_t* data, size_t length) override {
        if (length < threshold_) {
            // Below threshold: store uncompressed
            return std::vector<uint8_t>(data, data + length);
        }
        
        uLongf compressed_size = compressBound(static_cast<uLong>(length));
        std::vector<uint8_t> output(compressed_size);
        
        int result = compress2(output.data(), &compressed_size,
                               data, static_cast<uLong>(length), level_);
        
        if (result != Z_OK) {
            throw std::runtime_error("ZLib compression failed: " + std::to_string(result));
        }
        
        output.resize(compressed_size);
        return output;
    }
    
    std::vector<uint8_t> decompress(const uint8_t* data, size_t length,
                                      size_t max_size = 10 * 1024 * 1024) override {
        // Try increasing buffer sizes
        size_t output_size = length * 4;
        if (output_size > max_size) output_size = max_size;
        
        std::vector<uint8_t> output(output_size);
        
        z_stream strm{};
        strm.next_in = const_cast<Bytef*>(data);
        strm.avail_in = static_cast<uInt>(length);
        strm.next_out = output.data();
        strm.avail_out = static_cast<uInt>(output_size);
        
        if (inflateInit(&strm) != Z_OK) {
            throw std::runtime_error("ZLib inflateInit failed");
        }
        
        int result = inflate(&strm, Z_FINISH);
        if (result == Z_BUF_ERROR || result == Z_OK) {
            // Need bigger buffer
            output_size = std::min(output_size * 4, max_size);
            output.resize(output_size);
            strm.next_out = output.data() + strm.total_out;
            strm.avail_out = static_cast<uInt>(output_size - strm.total_out);
            result = inflate(&strm, Z_FINISH);
        }
        
        if (result != Z_STREAM_END) {
            inflateEnd(&strm);
            throw std::runtime_error("ZLib decompression failed: " + std::to_string(result));
        }
        
        output.resize(strm.total_out);
        inflateEnd(&strm);
        return output;
    }
    
    CompressionAlgorithm getAlgorithm() const override {
        return CompressionAlgorithm::ZLib;
    }
    
    void setThreshold(uint16_t threshold) { threshold_ = threshold; }
    void setLevel(int level) { level_ = level; }
    
private:
    int level_;
    uint16_t threshold_;
};
#endif

// =============================================================================
// Snappy Compression (stub - users must implement or link snappy)
// =============================================================================
class SnappyCompression : public Compression {
public:
    std::vector<uint8_t> compress(const uint8_t* data, size_t length) override {
        // Snappy format stub: just copy the data
        // Users should replace this with actual snappy::Compress
        // when linking against the snappy library
#ifdef CPHERTUNNEL_HAS_SNAPPY
        size_t max_compressed = snappy::MaxCompressedLength(length);
        std::vector<uint8_t> output(max_compressed);
        size_t actual;
        snappy::RawCompress(reinterpret_cast<const char*>(data), length,
                           reinterpret_cast<char*>(output.data()), &actual);
        output.resize(actual);
        return output;
#else
        return std::vector<uint8_t>(data, data + length);
#endif
    }
    
    std::vector<uint8_t> decompress(const uint8_t* data, size_t length,
                                      size_t /*max_size*/ = 10 * 1024 * 1024) override {
#ifdef CPHERTUNNEL_HAS_SNAPPY
        size_t uncompressed_size;
        if (!snappy::GetUncompressedLength(reinterpret_cast<const char*>(data), length,
                                            &uncompressed_size)) {
            throw std::runtime_error("Snappy: invalid compressed data");
        }
        std::vector<uint8_t> output(uncompressed_size);
        if (!snappy::RawUncompress(reinterpret_cast<const char*>(data), length,
                                    reinterpret_cast<char*>(output.data()))) {
            throw std::runtime_error("Snappy decompression failed");
        }
        return output;
#else
        return std::vector<uint8_t>(data, data + length);
#endif
    }
    
    CompressionAlgorithm getAlgorithm() const override {
        return CompressionAlgorithm::Snappy;
    }
};

// =============================================================================
// Factory
// =============================================================================
inline std::unique_ptr<Compression> createCompression(CompressionAlgorithm algo) {
    switch (algo) {
#ifdef CPHERTUNNEL_HAS_ZLIB
        case CompressionAlgorithm::ZLib:
            return std::make_unique<ZLibCompression>();
#endif
        case CompressionAlgorithm::Snappy:
            return std::make_unique<SnappyCompression>();
        case CompressionAlgorithm::None:
        default:
            return std::make_unique<NoCompression>();
    }
}

} // namespace cphertunnel::network
