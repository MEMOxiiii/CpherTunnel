// CpherTunnel - SetDefaultGameTypePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SetDefaultGameTypePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SetDefaultGameTypePacket, SetDefaultGameType)

    int32_t game_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(game_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        game_type = stream.readVarInt();
    }
};

} // namespace cphertunnel
