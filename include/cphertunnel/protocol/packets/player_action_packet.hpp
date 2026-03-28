// CpherTunnel - PlayerActionPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class PlayerActionPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PlayerActionPacket, PlayerAction)

    ActorRuntimeID runtime_id;
    PlayerActionType action = PlayerActionType::StartBreak;
    NetworkBlockPos block_position;
    NetworkBlockPos result_position;
    int32_t face = 0;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(runtime_id);
        stream.writeVarInt(static_cast<int32_t>(action));
        stream.writeNetworkBlockPos(block_position);
        stream.writeNetworkBlockPos(result_position);
        stream.writeVarInt(face);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        runtime_id = stream.readActorRuntimeID();
        action = static_cast<PlayerActionType>(stream.readVarInt());
        block_position = stream.readNetworkBlockPos();
        result_position = stream.readNetworkBlockPos();
        face = stream.readVarInt();
    }
};

} // namespace cphertunnel
