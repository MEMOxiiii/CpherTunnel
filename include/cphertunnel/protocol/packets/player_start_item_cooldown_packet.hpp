// CpherTunnel - PlayerStartItemCooldownPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class PlayerStartItemCooldownPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PlayerStartItemCooldownPacket, PlayerStartItemCooldown)

    std::string item_category;
    int32_t cooldown_ticks = 0;

    void write(BinaryStream& stream) const override {
        stream.writeString(item_category);
        stream.writeVarInt(cooldown_ticks);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        item_category = stream.readString();
        cooldown_ticks = stream.readVarInt();
    }
};

} // namespace cphertunnel
