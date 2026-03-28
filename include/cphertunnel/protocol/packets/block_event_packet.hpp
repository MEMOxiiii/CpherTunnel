// CpherTunnel - BlockEventPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class BlockEventPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(BlockEventPacket, BlockEvent)

    NetworkBlockPos position;
    int32_t event_type = 0;
    int32_t event_data = 0;

    void write(BinaryStream& stream) const override {
        stream.writeNetworkBlockPos(position);
        stream.writeVarInt(event_type);
        stream.writeVarInt(event_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readNetworkBlockPos();
        event_type = stream.readVarInt();
        event_data = stream.readVarInt();
    }
};

} // namespace cphertunnel
