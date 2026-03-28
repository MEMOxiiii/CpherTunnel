// CpherTunnel - InteractPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class InteractPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(InteractPacket, Interact)

    uint8_t action = 0;
    ActorRuntimeID target_runtime_id;
    Vec3 position; // only for InteractUpdate

    void write(BinaryStream& stream) const override {
        stream.writeByte(action);
        stream.writeActorRuntimeID(target_runtime_id);
        if (action == static_cast<uint8_t>(InteractAction::InteractUpdate)) {
            stream.writeVec3(position);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action = stream.readByte();
        target_runtime_id = stream.readActorRuntimeID();
        if (action == static_cast<uint8_t>(InteractAction::InteractUpdate)) {
            position = stream.readVec3();
        }
    }
};

} // namespace cphertunnel
