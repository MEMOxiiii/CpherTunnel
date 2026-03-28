// CpherTunnel - RespawnPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class RespawnPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(RespawnPacket, Respawn)

    Vec3 position;
    uint8_t state = 0; // 0=searching, 1=ready, 2=client_ready, 3=dead
    ActorRuntimeID runtime_entity_id;

    void write(BinaryStream& stream) const override {
        stream.writeVec3(position);
        stream.writeByte(state);
        stream.writeActorRuntimeID(runtime_entity_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readVec3();
        state = stream.readByte();
        runtime_entity_id = stream.readActorRuntimeID();
    }
};

} // namespace cphertunnel
