// CpherTunnel - SetActorMotionPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SetActorMotionPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SetActorMotionPacket, SetActorMotion)

    ActorRuntimeID runtime_id;
    Vec3 motion;
    uint64_t tick = 0;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(runtime_id);
        stream.writeVec3(motion);
        stream.writeUnsignedVarInt64(tick);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        runtime_id = stream.readActorRuntimeID();
        motion = stream.readVec3();
        tick = stream.readUnsignedVarInt64();
    }
};

} // namespace cphertunnel
