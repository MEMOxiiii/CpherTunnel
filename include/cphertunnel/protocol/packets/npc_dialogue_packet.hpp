// CpherTunnel - NpcDialoguePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class NpcDialoguePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(NpcDialoguePacket, NpcDialogue)

    uint64_t entity_unique_id = 0;
    int32_t action_type = 0;
    std::string dialogue;
    std::string scene_name;
    std::string npc_name;
    std::string action_json;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedInt64(entity_unique_id);
        stream.writeVarInt(action_type);
        stream.writeString(dialogue);
        stream.writeString(scene_name);
        stream.writeString(npc_name);
        stream.writeString(action_json);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_unique_id = stream.readUnsignedInt64();
        action_type = stream.readVarInt();
        dialogue = stream.readString();
        scene_name = stream.readString();
        npc_name = stream.readString();
        action_json = stream.readString();
    }
};

} // namespace cphertunnel
