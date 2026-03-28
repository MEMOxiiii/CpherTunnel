// CpherTunnel - SpawnExperienceOrbPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SpawnExperienceOrbPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SpawnExperienceOrbPacket, SpawnExperienceOrb)

    Vec3 position;
    int32_t count = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVec3(position);
        stream.writeVarInt(count);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readVec3();
        count = stream.readVarInt();
    }
};

} // namespace cphertunnel
