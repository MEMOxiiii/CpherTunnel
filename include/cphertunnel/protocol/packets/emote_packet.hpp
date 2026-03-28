// CpherTunnel - EmotePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class EmotePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(EmotePacket, Emote)

    ActorRuntimeID runtime_id;
    std::string emote_id;
    uint32_t emote_ticks = 0;
    std::string xuid;
    std::string platform_id;
    uint8_t flags = 0; // bit 0 = SERVER_SIDE, bit 1 = MUTE_EMOTE_CHAT

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(runtime_id);
        stream.writeString(emote_id);
        stream.writeUnsignedVarInt(emote_ticks);
        stream.writeString(xuid);
        stream.writeString(platform_id);
        stream.writeByte(flags);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        runtime_id = stream.readActorRuntimeID();
        emote_id = stream.readString();
        emote_ticks = stream.readUnsignedVarInt();
        xuid = stream.readString();
        platform_id = stream.readString();
        flags = stream.readByte();
    }
};

} // namespace cphertunnel
