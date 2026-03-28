// CpherTunnel - MobEffectPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class MobEffectPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(MobEffectPacket, MobEffect)

    ActorRuntimeID runtime_id;
    uint8_t event_id = 0; // 1=add, 2=modify, 3=remove
    int32_t effect_id = 0;
    int32_t amplifier = 0;
    bool particles = true;
    int32_t duration = 0;
    uint64_t tick = 0;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(runtime_id);
        stream.writeByte(event_id);
        stream.writeVarInt(effect_id);
        stream.writeVarInt(amplifier);
        stream.writeBool(particles);
        stream.writeVarInt(duration);
        stream.writeUnsignedVarInt64(tick);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        runtime_id = stream.readActorRuntimeID();
        event_id = stream.readByte();
        effect_id = stream.readVarInt();
        amplifier = stream.readVarInt();
        particles = stream.readBool();
        duration = stream.readVarInt();
        tick = stream.readUnsignedVarInt64();
    }
};

} // namespace cphertunnel
