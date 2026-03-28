// CpherTunnel - AnvilDamagePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class AnvilDamagePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(AnvilDamagePacket, AnvilDamage)

    uint8_t damage = 0;
    NetworkBlockPos position;

    void write(BinaryStream& stream) const override {
        stream.writeByte(damage);
        stream.writeNetworkBlockPos(position);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        damage = stream.readByte();
        position = stream.readNetworkBlockPos();
    }
};

} // namespace cphertunnel
