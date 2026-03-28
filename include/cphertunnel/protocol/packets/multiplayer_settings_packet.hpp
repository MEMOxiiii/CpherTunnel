// CpherTunnel - MultiplayerSettingsPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class MultiplayerSettingsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(MultiplayerSettingsPacket, MultiplayerSettings)

    int32_t action_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(action_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action_type = stream.readVarInt();
    }
};

} // namespace cphertunnel
