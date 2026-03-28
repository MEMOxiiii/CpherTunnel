// CpherTunnel - LevelEventPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class LevelEventPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(LevelEventPacket, LevelEvent)

    int32_t event_id = 0;
    Vec3 position;
    int32_t data = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(event_id);
        stream.writeVec3(position);
        stream.writeVarInt(data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        event_id = stream.readVarInt();
        position = stream.readVec3();
        data = stream.readVarInt();
    }
};

} // namespace cphertunnel
