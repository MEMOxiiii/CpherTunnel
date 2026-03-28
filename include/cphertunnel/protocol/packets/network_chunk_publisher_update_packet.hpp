// CpherTunnel - NetworkChunkPublisherUpdatePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class NetworkChunkPublisherUpdatePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(NetworkChunkPublisherUpdatePacket, NetworkChunkPublisherUpdate)

    BlockPos position;
    uint32_t radius = 0;
    std::vector<ChunkPos> saved_chunks; // Server built chunks list

    void write(BinaryStream& stream) const override {
        stream.writeBlockPos(position);
        stream.writeUnsignedVarInt(radius);
        // "No size compression" = plain uint32 (not varint)
        stream.writeUnsignedInt(static_cast<uint32_t>(saved_chunks.size()));
        for (auto& cp : saved_chunks) {
            stream.writeVarInt(cp.x);
            stream.writeVarInt(cp.z);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readBlockPos();
        radius = stream.readUnsignedVarInt();
        uint32_t count = stream.readUnsignedInt();
        saved_chunks.resize(count);
        for (auto& cp : saved_chunks) {
            cp.x = stream.readVarInt();
            cp.z = stream.readVarInt();
        }
    }
};

} // namespace cphertunnel
