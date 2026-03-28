// CpherTunnel - LevelSoundEventPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class LevelSoundEventPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(LevelSoundEventPacket, LevelSoundEvent)

    uint32_t sound_id = 0;
    Vec3 position;
    int32_t extra_data = -1;
    std::string entity_type;
    bool is_baby_mob = false;
    bool is_global = false;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(sound_id);
        stream.writeVec3(position);
        stream.writeVarInt(extra_data);
        stream.writeString(entity_type);
        stream.writeBool(is_baby_mob);
        stream.writeBool(is_global);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        sound_id = stream.readUnsignedVarInt();
        position = stream.readVec3();
        extra_data = stream.readVarInt();
        entity_type = stream.readString();
        is_baby_mob = stream.readBool();
        is_global = stream.readBool();
    }
};

} // namespace cphertunnel
