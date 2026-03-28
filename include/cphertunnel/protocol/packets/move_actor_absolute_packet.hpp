// CpherTunnel - MoveActorAbsolutePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class MoveActorAbsolutePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(MoveActorAbsolutePacket, MoveAbsoluteActor)

    ActorRuntimeID runtime_id;
    uint16_t flags = 0; // bit 0=on_ground, bit 1=teleported, bit 2=force_move
    Vec3 position;
    uint8_t rotation_x = 0; // byte rotation (angle / 360 * 256)
    uint8_t rotation_y = 0;
    uint8_t rotation_z = 0;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(runtime_id);
        stream.writeUnsignedShort(flags);
        stream.writeVec3(position);
        stream.writeByte(rotation_x);
        stream.writeByte(rotation_y);
        stream.writeByte(rotation_z);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        runtime_id = stream.readActorRuntimeID();
        flags = stream.readUnsignedShort();
        position = stream.readVec3();
        rotation_x = stream.readByte();
        rotation_y = stream.readByte();
        rotation_z = stream.readByte();
    }
};

} // namespace cphertunnel
