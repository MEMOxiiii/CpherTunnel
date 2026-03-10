// =============================================================================
// BedrockProtocol - UI Packets
// Forms, HUD, emotes, NPC dialogue, skins, etc.
// =============================================================================
#pragma once

#include "../packet.hpp"

namespace bedrock {

// =============================================================================
// ModalFormRequestPacket (ID: 100)
// =============================================================================
class ModalFormRequestPacket : public Packet {
public:
    BEDROCK_PACKET(ModalFormRequestPacket, ModalFormRequest)

    uint32_t form_id = 0;
    std::string form_json; // JSON string for the form

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(form_id);
        stream.writeString(form_json);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        form_id = stream.readUnsignedVarInt();
        form_json = stream.readString();
    }
};

// =============================================================================
// ModalFormResponsePacket (ID: 101)
// =============================================================================
class ModalFormResponsePacket : public Packet {
public:
    BEDROCK_PACKET(ModalFormResponsePacket, ModalFormResponse)

    uint32_t form_id = 0;
    bool has_response_data = false;
    std::string response_json;
    bool has_cancel_reason = false;
    uint8_t cancel_reason = 0; // 0=user_closed, 1=user_busy

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(form_id);
        stream.writeBool(has_response_data);
        if (has_response_data) {
            stream.writeString(response_json);
        }
        stream.writeBool(has_cancel_reason);
        if (has_cancel_reason) {
            stream.writeByte(cancel_reason);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        form_id = stream.readUnsignedVarInt();
        has_response_data = stream.readBool();
        if (has_response_data) {
            response_json = stream.readString();
        }
        has_cancel_reason = stream.readBool();
        if (has_cancel_reason) {
            cancel_reason = stream.readByte();
        }
    }
};

// =============================================================================
// ServerSettingsRequestPacket (ID: 102)
// =============================================================================
class ServerSettingsRequestPacket : public Packet {
public:
    BEDROCK_PACKET(ServerSettingsRequestPacket, ServerSettingsRequest)

    void write(BinaryStream&) const override {}
    void read(ReadOnlyBinaryStream&) override {}
};

// =============================================================================
// ServerSettingsResponsePacket (ID: 103)
// =============================================================================
class ServerSettingsResponsePacket : public Packet {
public:
    BEDROCK_PACKET(ServerSettingsResponsePacket, ServerSettingsResponse)

    uint32_t form_id = 0;
    std::string form_json;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(form_id);
        stream.writeString(form_json);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        form_id = stream.readUnsignedVarInt();
        form_json = stream.readString();
    }
};

// =============================================================================
// ClientboundCloseFormPacket (ID: 310)
// =============================================================================
class ClientboundCloseFormPacket : public Packet {
public:
    BEDROCK_PACKET(ClientboundCloseFormPacket, ClientboundCloseForm)

    void write(BinaryStream&) const override {}
    void read(ReadOnlyBinaryStream&) override {}
};

// =============================================================================
// EmotePacket (ID: 138)
// =============================================================================
class EmotePacket : public Packet {
public:
    BEDROCK_PACKET(EmotePacket, Emote)

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

// =============================================================================
// EmoteListPacket (ID: 152)
// =============================================================================
class EmoteListPacket : public Packet {
public:
    BEDROCK_PACKET(EmoteListPacket, EmoteList)

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

// =============================================================================
// NpcRequestPacket (ID: 98)
// =============================================================================
class NpcRequestPacket : public Packet {
public:
    BEDROCK_PACKET(NpcRequestPacket, NpcRequest)

    ActorRuntimeID entity_runtime_id;
    uint8_t request_type = 0;
    std::string command;
    uint8_t action_type = 0;
    std::string scene_name;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(entity_runtime_id);
        stream.writeByte(request_type);
        stream.writeString(command);
        stream.writeByte(action_type);
        stream.writeString(scene_name);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_runtime_id = stream.readActorRuntimeID();
        request_type = stream.readByte();
        command = stream.readString();
        action_type = stream.readByte();
        scene_name = stream.readString();
    }
};

// =============================================================================
// NpcDialoguePacket (ID: 169)
// =============================================================================
class NpcDialoguePacket : public Packet {
public:
    BEDROCK_PACKET(NpcDialoguePacket, NpcDialogue)

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

// =============================================================================
// SetHudPacket (ID: 308)
// =============================================================================
class SetHudPacket : public Packet {
public:
    BEDROCK_PACKET(SetHudPacket, SetHud)

    std::vector<uint32_t> hud_elements; // element IDs
    uint8_t visibility = 0; // 0=hide, 1=reset

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(static_cast<uint32_t>(hud_elements.size()));
        for (auto e : hud_elements) {
            stream.writeUnsignedVarInt(e);
        }
        stream.writeByte(visibility);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        uint32_t count = stream.readUnsignedVarInt();
        hud_elements.resize(count);
        for (auto& e : hud_elements) {
            e = stream.readUnsignedVarInt();
        }
        visibility = stream.readByte();
    }
};

// =============================================================================
// ShowStoreOfferPacket (ID: 91)
// =============================================================================
class ShowStoreOfferPacket : public Packet {
public:
    BEDROCK_PACKET(ShowStoreOfferPacket, ShowStoreOffer)

    std::string offer_id;
    uint8_t show_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeString(offer_id);
        stream.writeByte(show_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        offer_id = stream.readString();
        show_type = stream.readByte();
    }
};

// =============================================================================
// ShowProfilePacket (ID: 104)
// =============================================================================
class ShowProfilePacket : public Packet {
public:
    BEDROCK_PACKET(ShowProfilePacket, ShowProfile)

    std::string xuid;

    void write(BinaryStream& stream) const override {
        stream.writeString(xuid);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        xuid = stream.readString();
    }
};

// =============================================================================
// PlayerSkinPacket (ID: 93)
// =============================================================================
class PlayerSkinPacket : public Packet {
public:
    BEDROCK_PACKET(PlayerSkinPacket, PlayerSkin)

    UUID player_uuid;
    // Skin data is complex - store as raw for forward compatibility
    ByteBuffer skin_data;

    void write(BinaryStream& stream) const override {
        stream.writeUUID(player_uuid);
        stream.writeRawBytes(skin_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        player_uuid = stream.readUUID();
        skin_data = stream.readRemainingBytes();
    }
};

// =============================================================================
// BookEditPacket (ID: 97)
// =============================================================================
class BookEditPacket : public Packet {
public:
    BEDROCK_PACKET(BookEditPacket, BookEdit)

    uint8_t action = 0;
    uint8_t inventory_slot = 0;
    // Fields depend on action type
    uint8_t page_number = 0;
    uint8_t secondary_page_number = 0;
    std::string text;
    std::string photo;
    std::string title;
    std::string author;
    std::string xuid;

    void write(BinaryStream& stream) const override {
        stream.writeByte(action);
        stream.writeByte(inventory_slot);
        switch (action) {
            case 0: // ReplacePage
            case 1: // AddPage
                stream.writeByte(page_number);
                stream.writeString(text);
                stream.writeString(photo);
                break;
            case 2: // DeletePage
                stream.writeByte(page_number);
                break;
            case 3: // SwapPages
                stream.writeByte(page_number);
                stream.writeByte(secondary_page_number);
                break;
            case 4: // SignBook
                stream.writeString(title);
                stream.writeString(author);
                stream.writeString(xuid);
                break;
            default: break;
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action = stream.readByte();
        inventory_slot = stream.readByte();
        switch (action) {
            case 0:
            case 1:
                page_number = stream.readByte();
                text = stream.readString();
                photo = stream.readString();
                break;
            case 2:
                page_number = stream.readByte();
                break;
            case 3:
                page_number = stream.readByte();
                secondary_page_number = stream.readByte();
                break;
            case 4:
                title = stream.readString();
                author = stream.readString();
                xuid = stream.readString();
                break;
            default: break;
        }
    }
};

// =============================================================================
// AwardAchievementPacket (ID: 309)
// =============================================================================
class AwardAchievementPacket : public Packet {
public:
    BEDROCK_PACKET(AwardAchievementPacket, AwardAchievement)

    int32_t achievement_id = 0;

    void write(BinaryStream& stream) const override {
        stream.writeSignedInt(achievement_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        achievement_id = stream.readSignedInt();
    }
};

// =============================================================================
// ClientboundDataDrivenUIShowScreenPacket (ID: 333)
// =============================================================================
class ClientboundDataDrivenUIShowScreenPacket : public Packet {
public:
    BEDROCK_PACKET(ClientboundDataDrivenUIShowScreenPacket, ClientboundDataDrivenUIShowScreen)

    std::string screen_name;
    std::string screen_data;

    void write(BinaryStream& stream) const override {
        stream.writeString(screen_name);
        stream.writeString(screen_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        screen_name = stream.readString();
        screen_data = stream.readString();
    }
};

// =============================================================================
// ClientboundDataDrivenUIReloadPacket (ID: 335)
// =============================================================================
class ClientboundDataDrivenUIReloadPacket : public Packet {
public:
    BEDROCK_PACKET(ClientboundDataDrivenUIReloadPacket, ClientboundDataDrivenUIReload)

    void write(BinaryStream&) const override {}
    void read(ReadOnlyBinaryStream&) override {}
};

// =============================================================================
// ClientboundDataDrivenUICloseScreenPacket (ID: 334)
// =============================================================================
class ClientboundDataDrivenUICloseScreenPacket : public Packet {
public:
    BEDROCK_PACKET(ClientboundDataDrivenUICloseScreenPacket, ClientboundDataDrivenUICloseScreen)

    std::optional<uint32_t> form_id;

    void write(BinaryStream& stream) const override {
        stream.writeBool(form_id.has_value());
        if (form_id.has_value()) {
            stream.writeUnsignedInt(form_id.value());
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        if (stream.readBool()) {
            form_id = stream.readUnsignedInt();
        }
    }
};

} // namespace bedrock
