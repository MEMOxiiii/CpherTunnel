// CpherTunnel - CameraShakePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class CameraShakePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CameraShakePacket, CameraShake)

    float intensity = 0.0f;
    float seconds = 0.0f;
    uint8_t shake_type = 0;  // 0=positional, 1=rotational
    uint8_t shake_action = 0; // 0=add, 1=stop

    void write(BinaryStream& stream) const override {
        stream.writeFloat(intensity);
        stream.writeFloat(seconds);
        stream.writeByte(shake_type);
        stream.writeByte(shake_action);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        intensity = stream.readFloat();
        seconds = stream.readFloat();
        shake_type = stream.readByte();
        shake_action = stream.readByte();
    }
};

} // namespace cphertunnel
