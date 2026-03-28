// CpherTunnel - UpdateBlockPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class UpdateBlockPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(UpdateBlockPacket, UpdateBlock)

    NetworkBlockPos position;
    uint32_t block_runtime_id = 0;
    uint32_t flags = 0; // bitset: 0x1=neighbors, 0x2=network, 0x4=no_graphic, 0x8=priority
    uint32_t layer = 0; // 0=normal, 1=liquid

    void write(BinaryStream& stream) const override {
        stream.writeNetworkBlockPos(position);
        stream.writeUnsignedVarInt(block_runtime_id);
        stream.writeUnsignedVarInt(flags);
        stream.writeUnsignedVarInt(layer);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readNetworkBlockPos();
        block_runtime_id = stream.readUnsignedVarInt();
        flags = stream.readUnsignedVarInt();
        layer = stream.readUnsignedVarInt();
    }
};

} // namespace cphertunnel
