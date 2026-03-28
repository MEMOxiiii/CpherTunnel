// =============================================================================
// CpherTunnel - Batch Packet Processing
// Handles the full pipeline: batch framing + compression + encryption
// =============================================================================
#pragma once

#include "../protocol/packet_codec.hpp"
#include "compression.hpp"
#include "encryption.hpp"
#include <memory>

namespace cphertunnel::network {

// =============================================================================
// BatchPacketProcessor
// Pipeline: Raw → Decrypt → Decompress → Decode sub-packets (and reverse)
// =============================================================================
class BatchPacketProcessor {
public:
    BatchPacketProcessor()
        : compression_(std::make_unique<NoCompression>())
        , encryption_(createEncryption()) {
    }
    
    // =========================================================================
    // Configuration
    // =========================================================================
    
    void setCompression(CompressionAlgorithm algo) {
        compression_ = createCompression(algo);
        compression_initialized_ = true;
    }
    
    void setCompression(std::unique_ptr<Compression> comp) {
        compression_ = std::move(comp);
        compression_initialized_ = true;
    }
    
    /// Mark compression as initialized (call after sending NetworkSettings)
    void setCompressionInitialized(bool v) { compression_initialized_ = v; }
    bool isCompressionInitialized() const { return compression_initialized_; }
    
    Encryption& getEncryption() { return *encryption_; }
    
    void setEncryption(std::unique_ptr<Encryption> enc) {
        encryption_ = std::move(enc);
    }
    
    CompressionAlgorithm getCompressionAlgorithm() const {
        return compression_->getAlgorithm();
    }
    
    // =========================================================================
    // Encode: Packets → compressed/encrypted batch payload
    // =========================================================================
    
    // Encode a single packet into a batch payload ready for RakNet
    std::vector<uint8_t> encodeSingle(const cphertunnel::Packet& packet) {
        // Serialize packet into sub-packet format
        auto sub_packet = cphertunnel::PacketCodec::encodeSingle(packet);
        
        std::vector<uint8_t> result;
        
        if (compression_initialized_) {
            // After NetworkSettings: prepend compression header byte
            result.push_back(compressionHeaderByte());
            auto compressed = compression_->compress(
                reinterpret_cast<const uint8_t*>(sub_packet.data()), sub_packet.size());
            result.insert(result.end(), compressed.begin(), compressed.end());
        } else {
            // Before NetworkSettings: raw batch frame, no header byte
            result.assign(sub_packet.begin(), sub_packet.end());
        }
        
        // Encrypt
        return encryption_->encrypt(result);
    }
    
    // Encode multiple packets into a single batch payload
    std::vector<uint8_t> encodeBatch(const std::vector<const cphertunnel::Packet*>& packets) {
        auto batch = cphertunnel::PacketCodec::encodeBatch(packets);
        
        std::vector<uint8_t> result;
        
        if (compression_initialized_) {
            result.push_back(compressionHeaderByte());
            auto compressed = compression_->compress(
                reinterpret_cast<const uint8_t*>(batch.data()), batch.size());
            result.insert(result.end(), compressed.begin(), compressed.end());
        } else {
            result.assign(batch.begin(), batch.end());
        }
        
        return encryption_->encrypt(result);
    }
    
    // =========================================================================
    // Decode: batch payload → individual packet buffers
    // =========================================================================
    
    // Decode a batch payload into individual sub-packet buffers
    // Returns pairs of (PacketId, serialized body)
    std::vector<std::pair<PacketId, std::vector<uint8_t>>> decode(
            const std::vector<uint8_t>& payload) {
        // Decrypt
        auto decrypted = encryption_->decrypt(payload);
        
        std::vector<std::pair<PacketId, std::vector<uint8_t>>> result;
        if (decrypted.empty()) return result;
        
        // Strip compression header byte (if initialized) and decompress
        auto decompressed = decompressBatch(decrypted);
        
        // Decode sub-packets
        cphertunnel::PacketCodec::decodeBatch(decompressed.data(), decompressed.size(),
            [&result](const cphertunnel::PacketCodec::DecodedSubPacket& sub) {
                PacketId id = static_cast<PacketId>(sub.header.packet_id);
                result.emplace_back(id,
                    std::vector<uint8_t>(sub.payload_data,
                                         sub.payload_data + sub.payload_size));
            });
        
        return result;
    }
    
    // Decode with callback for each sub-packet
    void decode(const std::vector<uint8_t>& payload,
                std::function<void(PacketId, const uint8_t*, size_t)> callback) {
        auto decrypted = encryption_->decrypt(payload);
        if (decrypted.empty()) return;
        
        auto decompressed = decompressBatch(decrypted); // handles pre/post initialization
        cphertunnel::PacketCodec::decodeBatch(decompressed.data(), decompressed.size(),
            [&callback](const cphertunnel::PacketCodec::DecodedSubPacket& sub) {
                callback(static_cast<PacketId>(sub.header.packet_id),
                         sub.payload_data, sub.payload_size);
            });
    }
    
private:
    // Get the compression header byte for the current algorithm
    uint8_t compressionHeaderByte() const {
        switch (compression_->getAlgorithm()) {
            case CompressionAlgorithm::ZLib:   return 0x00;
            case CompressionAlgorithm::Snappy: return 0x01;
            case CompressionAlgorithm::None:   return 0xFF;
            default:                           return 0xFF;
        }
    }
    
    // Strip compression header byte (if initialized) and decompress the batch data
    std::vector<uint8_t> decompressBatch(const std::vector<uint8_t>& data) {
        if (!compression_initialized_) {
            // Before NetworkSettings: raw batch frame, no header byte
            return data;
        }
        
        if (data.empty()) return data;
        
        uint8_t header = data[0];
        const uint8_t* batch_data = data.data() + 1;
        size_t batch_size = data.size() - 1;
        
        if (header == 0xFF) {
            // No compression
            return std::vector<uint8_t>(batch_data, batch_data + batch_size);
        }
        // Compressed data — use the configured decompressor
        return compression_->decompress(batch_data, batch_size);
    }
    
    std::unique_ptr<Compression> compression_;
    std::unique_ptr<Encryption> encryption_;
    bool compression_initialized_ = false;
};

} // namespace cphertunnel::network
