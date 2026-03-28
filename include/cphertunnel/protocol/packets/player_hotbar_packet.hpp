// CpherTunnel - PlayerHotbarPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class PlayerHotbarPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PlayerHotbarPacket, PlayerHotbar)

    uint32_t selected_hotbar_slot = 0;
    uint8_t window_id = 0;
    bool select_hotbar_slot = true;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(selected_hotbar_slot);
        stream.writeByte(window_id);
        stream.writeBool(select_hotbar_slot);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        selected_hotbar_slot = stream.readUnsignedVarInt();
        window_id = stream.readByte();
        select_hotbar_slot = stream.readBool();
    }
};

} // namespace cphertunnel
