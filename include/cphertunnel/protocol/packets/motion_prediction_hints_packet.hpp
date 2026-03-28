// CpherTunnel - MotionPredictionHintsPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class MotionPredictionHintsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(MotionPredictionHintsPacket, MotionPredictionHints)

    ActorRuntimeID entity_runtime_id;
    Vec3 velocity;
    bool on_ground = false;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(entity_runtime_id);
        stream.writeVec3(velocity);
        stream.writeBool(on_ground);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_runtime_id = stream.readActorRuntimeID();
        velocity = stream.readVec3();
        on_ground = stream.readBool();
    }
};

} // namespace cphertunnel
