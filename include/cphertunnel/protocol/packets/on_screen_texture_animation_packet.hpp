// CpherTunnel - OnScreenTextureAnimationPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class OnScreenTextureAnimationPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(OnScreenTextureAnimationPacket, OnScreenTextureAnimation)

    uint32_t animation_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedInt(animation_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        animation_type = stream.readUnsignedInt();
    }
};

} // namespace cphertunnel
