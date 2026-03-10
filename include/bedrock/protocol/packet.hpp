// =============================================================================
// BedrockProtocol - Base Packet Class
// All protocol packets derive from this base
// =============================================================================
#pragma once

#include "types.hpp"
#include "packet_ids.hpp"
#include "binary_stream.hpp"
#include <memory>
#include <string>

namespace bedrock {

// =============================================================================
// Packet - Abstract base for all Minecraft Bedrock packets
// =============================================================================
class Packet {
public:
    virtual ~Packet() = default;

    /// Returns the unique packet ID
    virtual PacketId getId() const = 0;

    /// Returns the human-readable packet name
    virtual const char* getName() const {
        return packetIdToString(getId());
    }

    /// Serializes packet fields into the stream (after header)
    virtual void write(BinaryStream& stream) const = 0;

    /// Deserializes packet fields from the stream (after header)
    virtual void read(ReadOnlyBinaryStream& stream) = 0;

    /// Maximum allowed payload size (default 10 MB)
    virtual size_t getMaxSize() const { return MAX_PACKET_SIZE; }

    /// Whether this packet is compressible
    virtual Compressibility getCompressibility() const {
        return Compressibility::Compressible;
    }

    /// Whether this packet should be sent reliably
    virtual NetworkPeerReliability getReliability() const {
        return NetworkPeerReliability::ReliableOrdered;
    }

    /// Whether batching is disallowed for this packet
    virtual bool disallowBatching() const { return false; }

    // ─── Sender/Target SubClient ─────────────────────────────────
    SubClientId senderSubId = SubClientId::PrimaryClient;
    SubClientId targetSubId = SubClientId::PrimaryClient;

    // ─── Serialization helpers ───────────────────────────────────

    /// Serialize the full packet (header + payload) into a buffer
    std::string serialize() const {
        BinaryStream stream;
        // Write packet header as VarInt
        PacketHeader header;
        header.packet_id = static_cast<uint32_t>(getId());
        header.sender_subclient = senderSubId;
        header.target_subclient = targetSubId;
        stream.writeUnsignedVarInt(header.encode());
        // Write packet payload
        write(stream);
        return std::string(stream.getBuffer());
    }

    /// Deserialize a packet from raw data (starting after the batch frame header)
    bool deserialize(const uint8_t* data, size_t size) {
        ReadOnlyBinaryStream stream(data, size);
        // Read packet header
        uint32_t headerVal = stream.readUnsignedVarInt();
        if (stream.hasOverflowed()) return false;
        PacketHeader header = PacketHeader::decode(headerVal);
        senderSubId = header.sender_subclient;
        targetSubId = header.target_subclient;
        // Read packet payload
        read(stream);
        return !stream.hasOverflowed();
    }
};

/// Helper macro for defining packet classes with less boilerplate
#define BEDROCK_PACKET(ClassName, ID_VALUE) \
    PacketId getId() const override { return PacketId::ID_VALUE; } \
    const char* getName() const override { return #ClassName; }

} // namespace bedrock
