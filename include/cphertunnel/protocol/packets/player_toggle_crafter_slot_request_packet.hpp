// CpherTunnel - PlayerToggleCrafterSlotRequestPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class PlayerToggleCrafterSlotRequestPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PlayerToggleCrafterSlotRequestPacket, PlayerToggleCrafterSlotRequest)

    int32_t x = 0;
    int32_t y = 0;
    int32_t z = 0;
    uint8_t slot = 0;
    bool disabled = false;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(x);
        stream.writeVarInt(y);
        stream.writeVarInt(z);
        stream.writeByte(slot);
        stream.writeBool(disabled);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        x = stream.readVarInt();
        y = stream.readVarInt();
        z = stream.readVarInt();
        slot = stream.readByte();
        disabled = stream.readBool();
    }
};

} // namespace cphertunnel
