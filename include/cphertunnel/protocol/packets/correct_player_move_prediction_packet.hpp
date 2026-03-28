// CpherTunnel - CorrectPlayerMovePredictionPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class CorrectPlayerMovePredictionPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CorrectPlayerMovePredictionPacket, CorrectPlayerMovePrediction)

    uint8_t prediction_type = 0;
    Vec3 position;
    Vec3 delta;
    bool on_ground = false;
    uint64_t tick = 0;
    Vec2 rotation;

    void write(BinaryStream& stream) const override {
        stream.writeByte(prediction_type);
        stream.writeVec3(position);
        stream.writeVec3(delta);
        stream.writeBool(on_ground);
        stream.writeUnsignedVarInt64(tick);
        if (prediction_type == 1) { // vehicle
            stream.writeVec2(rotation);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        prediction_type = stream.readByte();
        position = stream.readVec3();
        delta = stream.readVec3();
        on_ground = stream.readBool();
        tick = stream.readUnsignedVarInt64();
        if (prediction_type == 1) {
            rotation = stream.readVec2();
        }
    }
};

} // namespace cphertunnel
