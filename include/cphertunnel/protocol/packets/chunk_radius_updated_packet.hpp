// CpherTunnel - ChunkRadiusUpdatedPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ChunkRadiusUpdatedPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ChunkRadiusUpdatedPacket, ChunkRadiusUpdated)

    int32_t chunk_radius = 8;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(chunk_radius);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        chunk_radius = stream.readVarInt();
    }
};

} // namespace cphertunnel
