// CpherTunnel - AddPaintingPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class AddPaintingPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(AddPaintingPacket, AddPainting)

    ActorUniqueID entity_id;
    ActorRuntimeID runtime_id;
    Vec3 position;
    int32_t direction = 0;
    std::string motive;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(entity_id);
        stream.writeActorRuntimeID(runtime_id);
        stream.writeVec3(position);
        stream.writeVarInt(direction);
        stream.writeString(motive);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_id = stream.readActorUniqueID();
        runtime_id = stream.readActorRuntimeID();
        position = stream.readVec3();
        direction = stream.readVarInt();
        motive = stream.readString();
    }
};

} // namespace cphertunnel
