// CpherTunnel - CameraAimAssistActorPriorityPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class CameraAimAssistActorPriorityPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CameraAimAssistActorPriorityPacket, CameraAimAssistActorPriority)

    ActorUniqueID entity_unique_id;
    std::string preset_id;
    std::string category_id;
    std::string actor_id;
    uint8_t priority = 0;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(entity_unique_id);
        stream.writeString(preset_id);
        stream.writeString(category_id);
        stream.writeString(actor_id);
        stream.writeByte(priority);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_unique_id = stream.readActorUniqueID();
        preset_id = stream.readString();
        category_id = stream.readString();
        actor_id = stream.readString();
        priority = stream.readByte();
    }
};

} // namespace cphertunnel
