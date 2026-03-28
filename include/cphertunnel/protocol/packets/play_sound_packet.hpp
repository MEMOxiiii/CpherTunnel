// CpherTunnel - PlaySoundPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class PlaySoundPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PlaySoundPacket, PlaySound)

    std::string sound_name;
    NetworkBlockPos position; // multiplied by 8 for fixed-point
    float volume = 1.0f;
    float pitch = 1.0f;

    void write(BinaryStream& stream) const override {
        stream.writeString(sound_name);
        // Position is encoded as block pos * 8
        stream.writeVarInt(position.x * 8);
        stream.writeUnsignedVarInt(position.y * 8);
        stream.writeVarInt(position.z * 8);
        stream.writeFloat(volume);
        stream.writeFloat(pitch);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        sound_name = stream.readString();
        position.x = stream.readVarInt() / 8;
        position.y = stream.readUnsignedVarInt() / 8;
        position.z = stream.readVarInt() / 8;
        volume = stream.readFloat();
        pitch = stream.readFloat();
    }
};

} // namespace cphertunnel
