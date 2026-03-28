// CpherTunnel - MoveActorDeltaPacket (ID: 111)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class MoveActorDeltaPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(MoveActorDeltaPacket, MoveDeltaActor)

    enum HeaderFlag : uint16_t {
        HasX            = 0x0001,
        HasY            = 0x0002,
        HasZ            = 0x0004,
        HasRotX         = 0x0008,
        HasRotY         = 0x0010,
        HasRotYHead     = 0x0020,
        OnGround        = 0x0040,
        Teleport        = 0x0080,
        ForceMoveLocalEntity = 0x0100
    };

    ActorRuntimeID runtime_id;
    uint16_t header = 0;
    float x = 0, y = 0, z = 0;
    uint8_t rot_x = 0, rot_y = 0, rot_y_head = 0;

    void write(BinaryStream& s) const override {
        s.writeActorRuntimeID(runtime_id);
        s.writeUnsignedShort(header);
        if (header & HasX) s.writeFloat(x);
        if (header & HasY) s.writeFloat(y);
        if (header & HasZ) s.writeFloat(z);
        if (header & HasRotX) s.writeByte(rot_x);
        if (header & HasRotY) s.writeByte(rot_y);
        if (header & HasRotYHead) s.writeByte(rot_y_head);
    }
    void read(ReadOnlyBinaryStream& s) override {
        runtime_id = s.readActorRuntimeID();
        header = s.readUnsignedShort();
        if (header & HasX) x = s.readFloat();
        if (header & HasY) y = s.readFloat();
        if (header & HasZ) z = s.readFloat();
        if (header & HasRotX) rot_x = s.readByte();
        if (header & HasRotY) rot_y = s.readByte();
        if (header & HasRotYHead) rot_y_head = s.readByte();
    }
};
} // namespace cphertunnel
