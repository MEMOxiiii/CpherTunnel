// CpherTunnel - UpdatePlayerGameTypePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class UpdatePlayerGameTypePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(UpdatePlayerGameTypePacket, UpdatePlayerGameType)

    int32_t game_type = 0;
    ActorUniqueID player_id;
    uint64_t tick = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(game_type);
        stream.writeActorUniqueID(player_id);
        stream.writeUnsignedVarInt64(tick);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        game_type = stream.readVarInt();
        player_id = stream.readActorUniqueID();
        tick = stream.readUnsignedVarInt64();
    }
};

} // namespace cphertunnel
