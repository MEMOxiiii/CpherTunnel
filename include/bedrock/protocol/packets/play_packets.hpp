// =============================================================================
// BedrockProtocol - Play/Game State Packets
// Packets for gameplay: text, title, time, game rules, etc.
// =============================================================================
#pragma once

#include "../packet.hpp"

namespace bedrock {

// ─── Text Packet Enums ───────────────────────────────────────────
enum class TextType : uint8_t {
    Raw             = 0,
    Chat            = 1,
    Translate       = 2,
    Popup           = 3,
    JukeboxPopup    = 4,
    Tip             = 5,
    SystemMessage   = 6,
    Whisper         = 7,
    Announcement    = 8,
    TextObjectWhisper     = 9,
    TextObject            = 10,
    TextObjectAnnouncement = 11
};

enum class TitleType : int32_t {
    Clear                   = 0,
    Reset                   = 1,
    Title                   = 2,
    Subtitle                = 3,
    Actionbar               = 4,
    Times                   = 5,
    TitleTextObject          = 6,
    SubtitleTextObject       = 7,
    ActionbarTextObject      = 8
};

// =============================================================================
// TextPacket (ID: 9)
// =============================================================================
class TextPacket : public Packet {
public:
    BEDROCK_PACKET(TextPacket, Text)

    TextType type = TextType::Raw;
    bool needs_translation = false;
    std::string source_name;     // player name (for Chat, Whisper, Announcement)
    std::string message;
    std::vector<std::string> parameters; // for Translate, Popup, JukeboxPopup
    std::string xuid;
    std::string platform_chat_id;
    std::string filtered_message;

    void write(BinaryStream& stream) const override {
        stream.writeByte(static_cast<uint8_t>(type));
        stream.writeBool(needs_translation);

        switch (type) {
            case TextType::Chat:
            case TextType::Whisper:
            case TextType::Announcement:
                stream.writeString(source_name);
                stream.writeString(message);
                break;
            case TextType::Raw:
            case TextType::Tip:
            case TextType::SystemMessage:
            case TextType::TextObjectWhisper:
            case TextType::TextObject:
            case TextType::TextObjectAnnouncement:
                stream.writeString(message);
                break;
            case TextType::Translate:
            case TextType::Popup:
            case TextType::JukeboxPopup:
                stream.writeString(message);
                stream.writeUnsignedVarInt(static_cast<uint32_t>(parameters.size()));
                for (auto& p : parameters) stream.writeString(p);
                break;
        }

        stream.writeString(xuid);
        stream.writeString(platform_chat_id);
        stream.writeString(filtered_message);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        type = static_cast<TextType>(stream.readByte());
        needs_translation = stream.readBool();

        switch (type) {
            case TextType::Chat:
            case TextType::Whisper:
            case TextType::Announcement:
                source_name = stream.readString();
                message = stream.readString();
                break;
            case TextType::Raw:
            case TextType::Tip:
            case TextType::SystemMessage:
            case TextType::TextObjectWhisper:
            case TextType::TextObject:
            case TextType::TextObjectAnnouncement:
                message = stream.readString();
                break;
            case TextType::Translate:
            case TextType::Popup:
            case TextType::JukeboxPopup:
                message = stream.readString();
                {
                    uint32_t count = stream.readUnsignedVarInt();
                    parameters.resize(count);
                    for (auto& p : parameters) p = stream.readString();
                }
                break;
        }

        xuid = stream.readString();
        platform_chat_id = stream.readString();
        filtered_message = stream.readString();
    }
};

// =============================================================================
// SetTimePacket (ID: 10)
// =============================================================================
class SetTimePacket : public Packet {
public:
    BEDROCK_PACKET(SetTimePacket, SetTime)

    int32_t time = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(time);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        time = stream.readVarInt();
    }
};

// =============================================================================
// SetTitlePacket (ID: 88)
// =============================================================================
class SetTitlePacket : public Packet {
public:
    BEDROCK_PACKET(SetTitlePacket, SetTitle)

    TitleType title_type = TitleType::Clear;
    std::string title_text;
    int32_t fade_in_time = 0;
    int32_t stay_time = 0;
    int32_t fade_out_time = 0;
    std::string xuid;
    std::string platform_online_id;
    std::string filtered_title_text;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(static_cast<int32_t>(title_type));
        stream.writeString(title_text);
        stream.writeVarInt(fade_in_time);
        stream.writeVarInt(stay_time);
        stream.writeVarInt(fade_out_time);
        stream.writeString(xuid);
        stream.writeString(platform_online_id);
        stream.writeString(filtered_title_text);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        title_type = static_cast<TitleType>(stream.readVarInt());
        title_text = stream.readString();
        fade_in_time = stream.readVarInt();
        stay_time = stream.readVarInt();
        fade_out_time = stream.readVarInt();
        xuid = stream.readString();
        platform_online_id = stream.readString();
        filtered_title_text = stream.readString();
    }
};

// =============================================================================
// SetDifficultyPacket (ID: 60)
// =============================================================================
class SetDifficultyPacket : public Packet {
public:
    BEDROCK_PACKET(SetDifficultyPacket, SetDifficulty)

    uint32_t difficulty = 0;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(difficulty);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        difficulty = stream.readUnsignedVarInt();
    }
};

// =============================================================================
// SetCommandsEnabledPacket (ID: 59)
// =============================================================================
class SetCommandsEnabledPacket : public Packet {
public:
    BEDROCK_PACKET(SetCommandsEnabledPacket, SetCommandsEnabled)

    bool commands_enabled = true;

    void write(BinaryStream& stream) const override {
        stream.writeBool(commands_enabled);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        commands_enabled = stream.readBool();
    }
};

// =============================================================================
// SetPlayerGameTypePacket (ID: 62)
// =============================================================================
class SetPlayerGameTypePacket : public Packet {
public:
    BEDROCK_PACKET(SetPlayerGameTypePacket, SetPlayerGameType)

    int32_t game_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(game_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        game_type = stream.readVarInt();
    }
};

// =============================================================================
// SetDefaultGameTypePacket (ID: 105)
// =============================================================================
class SetDefaultGameTypePacket : public Packet {
public:
    BEDROCK_PACKET(SetDefaultGameTypePacket, SetDefaultGameType)

    int32_t game_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(game_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        game_type = stream.readVarInt();
    }
};

// =============================================================================
// GameRulesChangedPacket (ID: 72)
// =============================================================================
#ifndef BEDROCK_GAMERULE_DEFINED
#define BEDROCK_GAMERULE_DEFINED
struct GameRule {
    std::string name;
    bool editable = true;
    uint32_t type = 0; // 1 = bool, 2 = int, 3 = float
    bool bool_value = false;
    int32_t int_value = 0;
    float float_value = 0.0f;
};
#endif

class GameRulesChangedPacket : public Packet {
public:
    BEDROCK_PACKET(GameRulesChangedPacket, GameRulesChanged)

    std::vector<GameRule> rules;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(static_cast<uint32_t>(rules.size()));
        for (auto& rule : rules) {
            stream.writeString(rule.name);
            stream.writeBool(rule.editable);
            stream.writeUnsignedVarInt(rule.type);
            switch (rule.type) {
                case 1: stream.writeBool(rule.bool_value); break;
                case 2: stream.writeVarInt(rule.int_value); break;
                case 3: stream.writeFloat(rule.float_value); break;
                default: break;
            }
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        uint32_t count = stream.readUnsignedVarInt();
        rules.resize(count);
        for (auto& rule : rules) {
            rule.name = stream.readString();
            rule.editable = stream.readBool();
            rule.type = stream.readUnsignedVarInt();
            switch (rule.type) {
                case 1: rule.bool_value = stream.readBool(); break;
                case 2: rule.int_value = stream.readVarInt(); break;
                case 3: rule.float_value = stream.readFloat(); break;
                default: break;
            }
        }
    }
};

// =============================================================================
// TransferPacket (ID: 85)
// =============================================================================
class TransferPacket : public Packet {
public:
    BEDROCK_PACKET(TransferPacket, Transfer)

    std::string server_address;
    uint16_t server_port = 19132;
    bool reload_world = false;

    void write(BinaryStream& stream) const override {
        stream.writeString(server_address);
        stream.writeUnsignedShort(server_port);
        stream.writeBool(reload_world);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        server_address = stream.readString();
        server_port = stream.readUnsignedShort();
        reload_world = stream.readBool();
    }
};

// =============================================================================
// SimpleEventPacket (ID: 64)
// =============================================================================
class SimpleEventPacket : public Packet {
public:
    BEDROCK_PACKET(SimpleEventPacket, SimpleEvent)

    uint16_t event_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedShort(event_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        event_type = stream.readUnsignedShort();
    }
};

// =============================================================================
// RespawnPacket (ID: 45)
// =============================================================================
class RespawnPacket : public Packet {
public:
    BEDROCK_PACKET(RespawnPacket, Respawn)

    Vec3 position;
    uint8_t state = 0; // 0=searching, 1=ready, 2=client_ready, 3=dead
    ActorRuntimeID runtime_entity_id;

    void write(BinaryStream& stream) const override {
        stream.writeVec3(position);
        stream.writeByte(state);
        stream.writeActorRuntimeID(runtime_entity_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readVec3();
        state = stream.readByte();
        runtime_entity_id = stream.readActorRuntimeID();
    }
};

// =============================================================================
// ShowCreditsPacket (ID: 75)
// =============================================================================
class ShowCreditsPacket : public Packet {
public:
    BEDROCK_PACKET(ShowCreditsPacket, ShowCredits)

    ActorRuntimeID player_id;
    int32_t status = 0;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(player_id);
        stream.writeVarInt(status);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        player_id = stream.readActorRuntimeID();
        status = stream.readVarInt();
    }
};

// =============================================================================
// SetHealthPacket (ID: 42)
// =============================================================================
class SetHealthPacket : public Packet {
public:
    BEDROCK_PACKET(SetHealthPacket, SetHealth)

    int32_t health = 20;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(health);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        health = stream.readVarInt();
    }
};

// =============================================================================
// SetSpawnPositionPacket (ID: 43)
// =============================================================================
class SetSpawnPositionPacket : public Packet {
public:
    BEDROCK_PACKET(SetSpawnPositionPacket, SetSpawnPosition)

    int32_t spawn_type = 0; // 0=player, 1=world
    NetworkBlockPos player_position;
    int32_t dimension = 0;
    NetworkBlockPos world_position;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(spawn_type);
        stream.writeNetworkBlockPos(player_position);
        stream.writeVarInt(dimension);
        stream.writeNetworkBlockPos(world_position);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        spawn_type = stream.readVarInt();
        player_position = stream.readNetworkBlockPos();
        dimension = stream.readVarInt();
        world_position = stream.readNetworkBlockPos();
    }
};

// =============================================================================
// ChangeDimensionPacket (ID: 61)
// =============================================================================
class ChangeDimensionPacket : public Packet {
public:
    BEDROCK_PACKET(ChangeDimensionPacket, ChangeDimension)

    int32_t dimension = 0;
    Vec3 position;
    bool respawn = false;
    std::optional<uint32_t> loading_screen_id;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(dimension);
        stream.writeVec3(position);
        stream.writeBool(respawn);
        stream.writeBool(loading_screen_id.has_value());
        if (loading_screen_id.has_value()) {
            stream.writeUnsignedInt(loading_screen_id.value());
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        dimension = stream.readVarInt();
        position = stream.readVec3();
        respawn = stream.readBool();
        if (stream.readBool()) {
            loading_screen_id = stream.readUnsignedInt();
        }
    }
};

// =============================================================================
// ToastRequestPacket (ID: 186)
// =============================================================================
class ToastRequestPacket : public Packet {
public:
    BEDROCK_PACKET(ToastRequestPacket, ToastRequest)

    std::string title;
    std::string content;

    void write(BinaryStream& stream) const override {
        stream.writeString(title);
        stream.writeString(content);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        title = stream.readString();
        content = stream.readString();
    }
};

// =============================================================================
// DeathInfoPacket (ID: 189)
// =============================================================================
class DeathInfoPacket : public Packet {
public:
    BEDROCK_PACKET(DeathInfoPacket, DeathInfo)

    std::string death_cause_message;
    std::vector<std::string> death_cause_params;

    void write(BinaryStream& stream) const override {
        stream.writeString(death_cause_message);
        stream.writeUnsignedVarInt(static_cast<uint32_t>(death_cause_params.size()));
        for (auto& p : death_cause_params) stream.writeString(p);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        death_cause_message = stream.readString();
        uint32_t count = stream.readUnsignedVarInt();
        death_cause_params.resize(count);
        for (auto& p : death_cause_params) p = stream.readString();
    }
};

// =============================================================================
// UpdatePlayerGameTypePacket (ID: 151)
// =============================================================================
class UpdatePlayerGameTypePacket : public Packet {
public:
    BEDROCK_PACKET(UpdatePlayerGameTypePacket, UpdatePlayerGameType)

    int32_t game_type = 0;
    ActorUniqueID player_id;
    uint64_t tick = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(game_type);
        stream.writeActorUniqueID(player_id);
        stream.writeUnsignedVarInt64(tick);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        game_type = stream.readVarInt();
        player_id = stream.readActorUniqueID();
        tick = stream.readUnsignedVarInt64();
    }
};

// =============================================================================
// SetLocalPlayerAsInitializedPacket (ID: 113)
// =============================================================================
class SetLocalPlayerAsInitializedPacket : public Packet {
public:
    BEDROCK_PACKET(SetLocalPlayerAsInitializedPacket, SetLocalPlayerAsInit)

    ActorRuntimeID runtime_entity_id;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(runtime_entity_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        runtime_entity_id = stream.readActorRuntimeID();
    }
};

// =============================================================================
// NetworkStackLatencyPacket (ID: 115)
// =============================================================================
class NetworkStackLatencyPacket : public Packet {
public:
    BEDROCK_PACKET(NetworkStackLatencyPacket, Ping)

    uint64_t timestamp = 0;
    bool from_server = false;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedInt64(timestamp);
        stream.writeBool(from_server);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        timestamp = stream.readUnsignedInt64();
        from_server = stream.readBool();
    }
};

// =============================================================================
// PacketViolationWarningPacket (ID: 156)
// =============================================================================
class PacketViolationWarningPacket : public Packet {
public:
    BEDROCK_PACKET(PacketViolationWarningPacket, PacketViolationWarning)

    int32_t violation_type = 0;
    int32_t severity = 0;
    int32_t packet_id_cause = 0;
    std::string context;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(violation_type);
        stream.writeVarInt(severity);
        stream.writeVarInt(packet_id_cause);
        stream.writeString(context);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        violation_type = stream.readVarInt();
        severity = stream.readVarInt();
        packet_id_cause = stream.readVarInt();
        context = stream.readString();
    }
};

// =============================================================================
// ChunkRadiusUpdatedPacket (ID: 70)
// =============================================================================
class ChunkRadiusUpdatedPacket : public Packet {
public:
    BEDROCK_PACKET(ChunkRadiusUpdatedPacket, ChunkRadiusUpdated)

    int32_t chunk_radius = 8;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(chunk_radius);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        chunk_radius = stream.readVarInt();
    }
};

// =============================================================================
// RequestChunkRadiusPacket (ID: 69)
// =============================================================================
class RequestChunkRadiusPacket : public Packet {
public:
    BEDROCK_PACKET(RequestChunkRadiusPacket, RequestChunkRadius)

    int32_t chunk_radius = 8;
    uint8_t max_chunk_radius = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(chunk_radius);
        stream.writeByte(max_chunk_radius);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        chunk_radius = stream.readVarInt();
        max_chunk_radius = stream.readByte();
    }
};

// =============================================================================
// UpdateAdventureSettingsPacket (ID: 188)
// =============================================================================
class UpdateAdventureSettingsPacket : public Packet {
public:
    BEDROCK_PACKET(UpdateAdventureSettingsPacket, UpdateAdventureSettings)

    bool no_pvm = false;
    bool no_mvp = false;
    bool immutable_world = false;
    bool show_name_tags = true;
    bool auto_jump = true;

    void write(BinaryStream& stream) const override {
        stream.writeBool(no_pvm);
        stream.writeBool(no_mvp);
        stream.writeBool(immutable_world);
        stream.writeBool(show_name_tags);
        stream.writeBool(auto_jump);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        no_pvm = stream.readBool();
        no_mvp = stream.readBool();
        immutable_world = stream.readBool();
        show_name_tags = stream.readBool();
        auto_jump = stream.readBool();
    }
};

} // namespace bedrock
