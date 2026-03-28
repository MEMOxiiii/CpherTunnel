// CpherTunnel - SetSpawnPositionPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SetSpawnPositionPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SetSpawnPositionPacket, SetSpawnPosition)

    int32_t spawn_type = 0; // 0=player, 1=world
    NetworkBlockPos player_position;
    int32_t dimension = 0;
    NetworkBlockPos world_position;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(spawn_type);
        stream.writeNetworkBlockPos(player_position);
        stream.writeVarInt(dimension);
        stream.writeNetworkBlockPos(world_position);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        spawn_type = stream.readVarInt();
        player_position = stream.readNetworkBlockPos();
        dimension = stream.readVarInt();
        world_position = stream.readNetworkBlockPos();
    }
};

} // namespace cphertunnel
