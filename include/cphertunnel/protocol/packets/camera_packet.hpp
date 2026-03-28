// CpherTunnel - CameraPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class CameraPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CameraPacket, Camera)

    ActorUniqueID camera_entity_id;
    ActorUniqueID target_player_id;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(camera_entity_id);
        stream.writeActorUniqueID(target_player_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        camera_entity_id = stream.readActorUniqueID();
        target_player_id = stream.readActorUniqueID();
    }
};

} // namespace cphertunnel
