// CpherTunnel - SetTimePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SetTimePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SetTimePacket, SetTime)

    int32_t time = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(time);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        time = stream.readVarInt();
    }
};

} // namespace cphertunnel
