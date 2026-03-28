// CpherTunnel - ServerPlayerPostMovePositionPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ServerPlayerPostMovePositionPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ServerPlayerPostMovePositionPacket, ServerPlayerPostMovePosition)

    Vec3 position;
    ActorRuntimeID player_runtime_id;

    void write(BinaryStream& stream) const override {
        stream.writeVec3(position);
        stream.writeActorRuntimeID(player_runtime_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readVec3();
        player_runtime_id = stream.readActorRuntimeID();
    }
};

} // namespace cphertunnel
