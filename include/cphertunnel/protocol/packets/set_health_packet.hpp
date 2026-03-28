// CpherTunnel - SetHealthPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SetHealthPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SetHealthPacket, SetHealth)

    int32_t health = 20;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(health);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        health = stream.readVarInt();
    }
};

} // namespace cphertunnel
