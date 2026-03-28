// CpherTunnel - BlockPickRequestPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class BlockPickRequestPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(BlockPickRequestPacket, BlockPickRequest)

    NetworkBlockPos position;
    bool add_user_data = false;
    uint8_t hotbar_slot = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(position.x);
        stream.writeVarInt(static_cast<int32_t>(position.y));
        stream.writeVarInt(position.z);
        stream.writeBool(add_user_data);
        stream.writeByte(hotbar_slot);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position.x = stream.readVarInt();
        position.y = static_cast<uint32_t>(stream.readVarInt());
        position.z = stream.readVarInt();
        add_user_data = stream.readBool();
        hotbar_slot = stream.readByte();
    }
};

} // namespace cphertunnel
