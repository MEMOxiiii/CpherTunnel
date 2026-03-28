// CpherTunnel - ChangeDimensionPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ChangeDimensionPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ChangeDimensionPacket, ChangeDimension)

    int32_t dimension = 0;
    Vec3 position;
    bool respawn = false;
    std::optional<uint32_t> loading_screen_id;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(dimension);
        stream.writeVec3(position);
        stream.writeBool(respawn);
        stream.writeBool(loading_screen_id.has_value());
        if (loading_screen_id.has_value()) {
            stream.writeUnsignedInt(loading_screen_id.value());
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        dimension = stream.readVarInt();
        position = stream.readVec3();
        respawn = stream.readBool();
        if (stream.readBool()) {
            loading_screen_id = stream.readUnsignedInt();
        }
    }
};

} // namespace cphertunnel
