// CpherTunnel - SetPlayerInventoryOptionsPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SetPlayerInventoryOptionsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SetPlayerInventoryOptionsPacket, SetPlayerInventoryOptions)

    int32_t left_tab = 0;
    int32_t right_tab = 0;
    bool filtering = false;
    int32_t layout = 0;
    int32_t crafting_layout = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(left_tab);
        stream.writeVarInt(right_tab);
        stream.writeBool(filtering);
        stream.writeVarInt(layout);
        stream.writeVarInt(crafting_layout);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        left_tab = stream.readVarInt();
        right_tab = stream.readVarInt();
        filtering = stream.readBool();
        layout = stream.readVarInt();
        crafting_layout = stream.readVarInt();
    }
};

} // namespace cphertunnel
