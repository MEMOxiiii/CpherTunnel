// CpherTunnel - OpenSignPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class OpenSignPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(OpenSignPacket, OpenSign)

    NetworkBlockPos position;
    bool front_side = true;

    void write(BinaryStream& stream) const override {
        stream.writeNetworkBlockPos(position);
        stream.writeBool(front_side);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readNetworkBlockPos();
        front_side = stream.readBool();
    }
};

} // namespace cphertunnel
