// CpherTunnel - HurtArmorPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class HurtArmorPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(HurtArmorPacket, HurtArmor)

    int32_t cause = 0;
    int32_t damage = 0;
    uint64_t armor_slots = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(cause);
        stream.writeVarInt(damage);
        stream.writeUnsignedVarInt64(armor_slots);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        cause = stream.readVarInt();
        damage = stream.readVarInt();
        armor_slots = stream.readUnsignedVarInt64();
    }
};

} // namespace cphertunnel
