// CpherTunnel - LabTablePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class LabTablePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(LabTablePacket, LabTable)

    uint8_t action_type = 0;
    NetworkBlockPos position;
    uint8_t reaction_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeByte(action_type);
        stream.writeNetworkBlockPos(position);
        stream.writeByte(reaction_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action_type = stream.readByte();
        position = stream.readNetworkBlockPos();
        reaction_type = stream.readByte();
    }
};

} // namespace cphertunnel
