// CpherTunnel - ClientCameraAimAssistPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ClientCameraAimAssistPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ClientCameraAimAssistPacket, ClientCameraAimAssist)

    Vec2 view_angle;
    float distance = 0.0f;
    uint8_t target_mode = 0;
    ActorUniqueID target_entity_id;

    void write(BinaryStream& stream) const override {
        stream.writeVec2(view_angle);
        stream.writeFloat(distance);
        stream.writeByte(target_mode);
        stream.writeActorUniqueID(target_entity_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        view_angle = stream.readVec2();
        distance = stream.readFloat();
        target_mode = stream.readByte();
        target_entity_id = stream.readActorUniqueID();
    }
};

} // namespace cphertunnel
