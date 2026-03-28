// CpherTunnel - GuiDataPickItemPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class GuiDataPickItemPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(GuiDataPickItemPacket, GuiDataPickItem)

    std::string item_description;
    std::string item_effects;
    int32_t hotbar_slot = 0;

    void write(BinaryStream& stream) const override {
        stream.writeString(item_description);
        stream.writeString(item_effects);
        stream.writeSignedInt(hotbar_slot);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        item_description = stream.readString();
        item_effects = stream.readString();
        hotbar_slot = stream.readSignedInt();
    }
};

} // namespace cphertunnel
