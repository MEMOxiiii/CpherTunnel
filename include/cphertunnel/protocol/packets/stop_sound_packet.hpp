// CpherTunnel - StopSoundPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class StopSoundPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(StopSoundPacket, StopSound)

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

} // namespace cphertunnel
