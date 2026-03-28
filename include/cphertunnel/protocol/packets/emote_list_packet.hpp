// CpherTunnel - EmoteListPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class EmoteListPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(EmoteListPacket, EmoteList)

    ActorRuntimeID player_id;
    std::vector<UUID> emote_piece_ids;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(player_id);
        stream.writeUnsignedVarInt(static_cast<uint32_t>(emote_piece_ids.size()));
        for (auto& id : emote_piece_ids) {
            stream.writeUUID(id);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        player_id = stream.readActorRuntimeID();
        uint32_t count = stream.readUnsignedVarInt();
        emote_piece_ids.resize(count);
        for (auto& id : emote_piece_ids) {
            id = stream.readUUID();
        }
    }
};

} // namespace cphertunnel
