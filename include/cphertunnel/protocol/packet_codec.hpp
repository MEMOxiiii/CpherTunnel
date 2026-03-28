// =============================================================================
// CpherTunnel - Packet Codec
// Handles encoding packets into batch frames and decoding batch frames
// into individual packets. A batch frame wraps one or more packets.
// =============================================================================
#pragma once

#include "packet.hpp"
#include "binary_stream.hpp"
#include <vector>
#include <functional>

namespace cphertunnel {

// =============================================================================
// PacketCodec - Encodes/decodes packet batch frames
// =============================================================================
//
// Bedrock batch frame format:
//   [0xFE] (game packet marker, handled at RakNet level)
//   For each sub-packet in the batch:
//     [VarInt: payload length] [payload bytes...]
//   Where payload = [VarInt: packet header] [packet-specific fields]
//
class PacketCodec {
public:
    /// Encode a single packet into a batch frame payload (without 0xFE prefix)
    static std::string encodeSingle(const Packet& packet) {
        std::string payload = packet.serialize();
        BinaryStream frame;
        frame.writeUnsignedVarInt(static_cast<uint32_t>(payload.size()));
        frame.writeRawBytes(payload);
        return std::string(frame.getBuffer());
    }

    /// Encode multiple packets into a single batch frame payload
    static std::string encodeBatch(const std::vector<const Packet*>& packets) {
        BinaryStream frame;
        for (const auto* pkt : packets) {
            std::string payload = pkt->serialize();
            frame.writeUnsignedVarInt(static_cast<uint32_t>(payload.size()));
            frame.writeRawBytes(payload);
        }
        return std::string(frame.getBuffer());
    }

    /// Result of decoding a single sub-packet from a batch
    struct DecodedSubPacket {
        PacketHeader header;
        const uint8_t* payload_data;
        size_t payload_size;
        size_t header_varint_size; // bytes consumed by header varint
    };

    /// Decode a batch frame into individual sub-packet descriptors
    /// The callback receives each sub-packet for processing
    static bool decodeBatch(
        const uint8_t* data, size_t size,
        std::function<void(const DecodedSubPacket&)> callback)
    {
        ReadOnlyBinaryStream stream(data, size);
        while (stream.getUnreadLength() > 0) {
            uint32_t packetLen = stream.readUnsignedVarInt();
            if (stream.hasOverflowed() || packetLen == 0) return false;
            if (packetLen > stream.getUnreadLength()) return false;

            size_t packetStart = stream.getReadPointer();

            // Read the header VarInt
            size_t beforeHeader = stream.getReadPointer();
            uint32_t headerVal = stream.readUnsignedVarInt();
            if (stream.hasOverflowed()) return false;
            size_t headerSize = stream.getReadPointer() - beforeHeader;

            DecodedSubPacket sub;
            sub.header = PacketHeader::decode(headerVal);
            sub.header_varint_size = headerSize;
            sub.payload_data = stream.getData() + stream.getReadPointer();
            sub.payload_size = packetLen - headerSize;

            callback(sub);

            // Skip to end of this sub-packet
            stream.setReadPointer(packetStart + packetLen);
        }
        return true;
    }

    /// Get the packet ID from raw batch frame data at a given sub-packet
    static PacketId peekPacketId(const uint8_t* subPacketData, size_t size) {
        ReadOnlyBinaryStream stream(subPacketData, size);
        uint32_t headerVal = stream.readUnsignedVarInt();
        if (stream.hasOverflowed()) return PacketId::EndId;
        PacketHeader h = PacketHeader::decode(headerVal);
        return static_cast<PacketId>(h.packet_id);
    }
};

} // namespace cphertunnel
