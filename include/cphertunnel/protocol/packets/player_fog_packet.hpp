// CpherTunnel - PlayerFogPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class PlayerFogPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PlayerFogPacket, PlayerFogSetting)

    std::vector<std::string> fog_stack;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(static_cast<uint32_t>(fog_stack.size()));
        for (auto& f : fog_stack) stream.writeString(f);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        uint32_t count = stream.readUnsignedVarInt();
        fog_stack.resize(count);
        for (auto& f : fog_stack) f = stream.readString();
    }
};

} // namespace cphertunnel
