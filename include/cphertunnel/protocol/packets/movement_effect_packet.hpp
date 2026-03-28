// CpherTunnel - MovementEffectPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class MovementEffectPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(MovementEffectPacket, MovementEffect)

    ActorRuntimeID entity_runtime_id;
    uint32_t effect_type = 0;
    int32_t duration = 0;
    uint64_t tick = 0;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(entity_runtime_id);
        stream.writeUnsignedVarInt(effect_type);
        stream.writeVarInt(duration);
        stream.writeUnsignedVarInt64(tick);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_runtime_id = stream.readActorRuntimeID();
        effect_type = stream.readUnsignedVarInt();
        duration = stream.readVarInt();
        tick = stream.readUnsignedVarInt64();
    }
};

} // namespace cphertunnel
