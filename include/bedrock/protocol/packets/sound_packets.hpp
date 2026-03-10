// =============================================================================
// BedrockProtocol - Sound Packets
// Play sound, stop sound
// =============================================================================
#pragma once

#include "../packet.hpp"

namespace bedrock {

// =============================================================================
// PlaySoundPacket (ID: 86)
// =============================================================================
class PlaySoundPacket : public Packet {
public:
    BEDROCK_PACKET(PlaySoundPacket, PlaySound)

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

// =============================================================================
// StopSoundPacket (ID: 87)
// =============================================================================
class StopSoundPacket : public Packet {
public:
    BEDROCK_PACKET(StopSoundPacket, StopSound)

    std::string sound_name;
    bool stop_all = false;
    bool stop_music_legacy = false;

    void write(BinaryStream& stream) const override {
        stream.writeString(sound_name);
        stream.writeBool(stop_all);
        stream.writeBool(stop_music_legacy);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        sound_name = stream.readString();
        stop_all = stream.readBool();
        stop_music_legacy = stream.readBool();
    }
};

} // namespace bedrock
