// CpherTunnel - PlayerArmorDamagePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class PlayerArmorDamagePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PlayerArmorDamagePacket, PlayerArmorDamage)

    uint8_t damage_bits = 0;
    int32_t helmet_damage = 0;
    int32_t chestplate_damage = 0;
    int32_t leggings_damage = 0;
    int32_t boots_damage = 0;
    int32_t body_damage = 0;

    void write(BinaryStream& stream) const override {
        stream.writeByte(damage_bits);
        if (damage_bits & 0x01) stream.writeVarInt(helmet_damage);
        if (damage_bits & 0x02) stream.writeVarInt(chestplate_damage);
        if (damage_bits & 0x04) stream.writeVarInt(leggings_damage);
        if (damage_bits & 0x08) stream.writeVarInt(boots_damage);
        if (damage_bits & 0x10) stream.writeVarInt(body_damage);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        damage_bits = stream.readByte();
        if (damage_bits & 0x01) helmet_damage = stream.readVarInt();
        if (damage_bits & 0x02) chestplate_damage = stream.readVarInt();
        if (damage_bits & 0x04) leggings_damage = stream.readVarInt();
        if (damage_bits & 0x08) boots_damage = stream.readVarInt();
        if (damage_bits & 0x10) body_damage = stream.readVarInt();
    }
};

} // namespace cphertunnel
