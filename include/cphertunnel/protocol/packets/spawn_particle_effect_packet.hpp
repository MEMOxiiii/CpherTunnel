// CpherTunnel - SpawnParticleEffectPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SpawnParticleEffectPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SpawnParticleEffectPacket, SpawnParticleEffect)

    uint8_t dimension_id = 0;
    ActorUniqueID entity_id;
    Vec3 position;
    std::string particle_name;
    std::optional<std::string> molang_variables;

    void write(BinaryStream& stream) const override {
        stream.writeByte(dimension_id);
        stream.writeActorUniqueID(entity_id);
        stream.writeVec3(position);
        stream.writeString(particle_name);
        stream.writeBool(molang_variables.has_value());
        if (molang_variables.has_value()) {
            stream.writeString(molang_variables.value());
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        dimension_id = stream.readByte();
        entity_id = stream.readActorUniqueID();
        position = stream.readVec3();
        particle_name = stream.readString();
        if (stream.readBool()) {
            molang_variables = stream.readString();
        }
    }
};

} // namespace cphertunnel
