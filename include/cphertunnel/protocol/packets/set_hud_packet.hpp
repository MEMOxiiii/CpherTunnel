// CpherTunnel - SetHudPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SetHudPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SetHudPacket, SetHud)

    std::vector<uint32_t> hud_elements; // element IDs
    uint8_t visibility = 0; // 0=hide, 1=reset

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(static_cast<uint32_t>(hud_elements.size()));
        for (auto e : hud_elements) {
            stream.writeUnsignedVarInt(e);
        }
        stream.writeByte(visibility);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        uint32_t count = stream.readUnsignedVarInt();
        hud_elements.resize(count);
        for (auto& e : hud_elements) {
            e = stream.readUnsignedVarInt();
        }
        visibility = stream.readByte();
    }
};

} // namespace cphertunnel
