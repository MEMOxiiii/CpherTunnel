// CpherTunnel - CameraAimAssistPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class CameraAimAssistPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CameraAimAssistPacket, CameraAimAssist)

    std::string preset_id;
    Vec2 view_angle;
    float distance = 0.0f;
    uint8_t target_mode = 0;
    uint8_t action = 0;

    void write(BinaryStream& stream) const override {
        stream.writeString(preset_id);
        stream.writeVec2(view_angle);
        stream.writeFloat(distance);
        stream.writeByte(target_mode);
        stream.writeByte(action);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        preset_id = stream.readString();
        view_angle = stream.readVec2();
        distance = stream.readFloat();
        target_mode = stream.readByte();
        action = stream.readByte();
    }
};

} // namespace cphertunnel
