// =============================================================================
// BedrockProtocol - Login & Handshake Packets
// Handles the connection establishment flow:
//   RequestNetworkSettings -> NetworkSettings -> Login ->
//   ServerToClientHandshake -> ClientToServerHandshake -> PlayStatus
// =============================================================================
#pragma once

#include "../packet.hpp"
#include "../nbt.hpp"
#include <vector>

namespace bedrock {

// Forward declare GameRule (defined in play_packets.hpp, duplicated here for StartGamePacket)
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

// ─── Enums ───────────────────────────────────────────────────────
enum class PlayStatusType : int32_t {
    LoginSuccess                          = 0,
    LoginFailed_ClientOld                 = 1,
    LoginFailed_ServerOld                 = 2,
    PlayerSpawn                           = 3,
    LoginFailed_InvalidTenant             = 4,
    LoginFailed_EditionMismatchEduToVanilla = 5,
    LoginFailed_EditionMismatchVanillaToEdu = 6,
    LoginFailed_ServerFullSubClient       = 7,
    LoginFailed_EditorMismatchEditorToVanilla = 8,
    LoginFailed_EditorMismatchVanillaToEditor = 9,
};

enum class ResourcePackResponseType : uint8_t {
    None       = 0,
    Refused    = 1,
    SendPacks  = 2,
    AllPacksDownloaded = 3,
    Completed  = 4
};

enum class DisconnectFailReason : int32_t {
    Unknown = 0,
    CantConnectNoInternet = 1,
    NoPermissions = 2,
    UnrecoverableError = 3,
    ThirdPartyBlocked = 4,
    ThirdPartyNoInternet = 5,
    ThirdPartyBadLogin = 6,
    FullServer = 7,
    NoConnection = 8,
    PlayerNotFound = 9,
    PacketIdOutOfRange = 10,
    BadPacketHeader = 11,
    BadSubClientLogin = 12,
    OutOfOrderPacket = 13,
    ResourcePackException = 14,
    IncompatibleProtocol = 15,
    CdnHashError = 16,
    IncompatibleTenantedPlayer = 17,
    NotAuthenticated = 18,
    PlayerLeave = 19,
    ConnectionTimeout = 20,
    ServerShutdown = 21,
    PlayerDuplicate = 22,
    OtherError = 23,
};

// =============================================================================
// RequestNetworkSettingsPacket (ID: 193)
// Client -> Server: First packet to initiate connection
// =============================================================================
class RequestNetworkSettingsPacket : public Packet {
public:
    BEDROCK_PACKET(RequestNetworkSettingsPacket, RequestNetworkSettings)

    int32_t client_network_version = PROTOCOL_VERSION;

    void write(BinaryStream& stream) const override {
        stream.writeSignedBigEndianInt(client_network_version);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        client_network_version = stream.readSignedBigEndianInt();
    }

    Compressibility getCompressibility() const override {
        return Compressibility::Incompressible;
    }

    bool disallowBatching() const override { return true; }
};

// =============================================================================
// NetworkSettingsPacket (ID: 143)
// Server -> Client: Response with compression settings
// =============================================================================
class NetworkSettingsPacket : public Packet {
public:
    BEDROCK_PACKET(NetworkSettingsPacket, NetworkSettings)

    uint16_t compression_threshold = 1;
    CompressionAlgorithm compression_algorithm = CompressionAlgorithm::ZLib;
    bool client_throttle_enabled = false;
    uint8_t client_throttle_threshold = 0;
    float client_throttle_scalar = 0.0f;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedShort(compression_threshold);
        stream.writeUnsignedShort(static_cast<uint16_t>(compression_algorithm));
        stream.writeBool(client_throttle_enabled);
        stream.writeByte(client_throttle_threshold);
        stream.writeFloat(client_throttle_scalar);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        compression_threshold = stream.readUnsignedShort();
        compression_algorithm = static_cast<CompressionAlgorithm>(stream.readUnsignedShort());
        client_throttle_enabled = stream.readBool();
        client_throttle_threshold = stream.readByte();
        client_throttle_scalar = stream.readFloat();
    }

    Compressibility getCompressibility() const override {
        return Compressibility::Incompressible;
    }

    bool disallowBatching() const override { return true; }
};

// =============================================================================
// LoginPacket (ID: 1)
// Client -> Server: Authentication data (JWT chain + client data)
// =============================================================================
class LoginPacket : public Packet {
public:
    BEDROCK_PACKET(LoginPacket, Login)

    int32_t client_network_version = PROTOCOL_VERSION;
    std::string connection_request; // JWT chain data

    void write(BinaryStream& stream) const override {
        stream.writeSignedBigEndianInt(client_network_version);
        // Connection request is written as a string with VarInt length
        stream.writeString(connection_request);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        client_network_version = stream.readSignedBigEndianInt();
        connection_request = stream.readString();
    }

    Compressibility getCompressibility() const override {
        return Compressibility::Incompressible;
    }
};

// =============================================================================
// PlayStatusPacket (ID: 2)
// Server -> Client: Login result status
// =============================================================================
class PlayStatusPacket : public Packet {
public:
    BEDROCK_PACKET(PlayStatusPacket, PlayStatus)

    PlayStatusType status = PlayStatusType::LoginSuccess;

    void write(BinaryStream& stream) const override {
        stream.writeSignedBigEndianInt(static_cast<int32_t>(status));
    }

    void read(ReadOnlyBinaryStream& stream) override {
        status = static_cast<PlayStatusType>(stream.readSignedBigEndianInt());
    }
};

// =============================================================================
// ServerToClientHandshakePacket (ID: 3)
// Server -> Client: JWT with encryption key info
// =============================================================================
class ServerToClientHandshakePacket : public Packet {
public:
    BEDROCK_PACKET(ServerToClientHandshakePacket, ServerToClientHandshake)

    std::string handshake_jwt; // Base64 JWT containing salt + public key

    void write(BinaryStream& stream) const override {
        stream.writeString(handshake_jwt);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        handshake_jwt = stream.readString();
    }
};

// =============================================================================
// ClientToServerHandshakePacket (ID: 4)
// Client -> Server: Empty packet confirming encryption setup
// =============================================================================
class ClientToServerHandshakePacket : public Packet {
public:
    BEDROCK_PACKET(ClientToServerHandshakePacket, ClientToServerHandshake)

    void write(BinaryStream&) const override {}
    void read(ReadOnlyBinaryStream&) override {}
};

// =============================================================================
// DisconnectPacket (ID: 5)
// Bidirectional: Disconnect with optional message
// =============================================================================
class DisconnectPacket : public Packet {
public:
    BEDROCK_PACKET(DisconnectPacket, Disconnect)

    DisconnectFailReason reason = DisconnectFailReason::Unknown;
    bool skip_message = false;
    std::string message;
    std::string filtered_message;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(static_cast<int32_t>(reason));
        stream.writeBool(skip_message);
        if (!skip_message) {
            stream.writeString(message);
            stream.writeString(filtered_message);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        reason = static_cast<DisconnectFailReason>(stream.readVarInt());
        skip_message = stream.readBool();
        if (!skip_message) {
            message = stream.readString();
            filtered_message = stream.readString();
        }
    }
};

// =============================================================================
// ResourcePacksInfoPacket (ID: 6)
// Server -> Client: Available resource/behavior packs
// Format matches protocol v766+ (UUID pack IDs, world template, inline CDN URL)
// =============================================================================
struct ResourcePackInfoEntry {
    UUID pack_id;       // mce::UUID (16 bytes), changed from string in v766
    std::string version;
    uint64_t size = 0;
    std::string content_key;
    std::string sub_pack_name;
    std::string content_id;
    bool has_scripts = false;
    bool is_addon_pack = false;
    bool is_ray_tracing_capable = false;
    std::string cdn_url;        // inline per-entry, moved from separate list in v748
};

class ResourcePacksInfoPacket : public Packet {
public:
    BEDROCK_PACKET(ResourcePacksInfoPacket, ResourcePacksInfo)

    bool must_accept = false;
    bool has_addon_packs = false;
    bool has_scripts = false;
    bool force_disable_vibrant_visuals = false;  // added in v818
    UUID world_template_uuid = {};                // added in v766
    std::string world_template_version;           // added in v766
    std::vector<ResourcePackInfoEntry> resource_packs;

    void write(BinaryStream& stream) const override {
        stream.writeBool(must_accept);
        stream.writeBool(has_addon_packs);
        stream.writeBool(has_scripts);
        stream.writeBool(force_disable_vibrant_visuals);
        stream.writeUUID(world_template_uuid);
        stream.writeString(world_template_version);

        // Resource pack entries
        stream.writeUnsignedShort(static_cast<uint16_t>(resource_packs.size()));
        for (auto& pack : resource_packs) {
            stream.writeUUID(pack.pack_id);
            stream.writeString(pack.version);
            stream.writeUnsignedInt64(pack.size);
            stream.writeString(pack.content_key);
            stream.writeString(pack.sub_pack_name);
            stream.writeString(pack.content_id);
            stream.writeBool(pack.has_scripts);
            stream.writeBool(pack.is_addon_pack);
            stream.writeBool(pack.is_ray_tracing_capable);
            stream.writeString(pack.cdn_url);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        must_accept = stream.readBool();
        has_addon_packs = stream.readBool();
        has_scripts = stream.readBool();
        force_disable_vibrant_visuals = stream.readBool();
        world_template_uuid = stream.readUUID();
        world_template_version = stream.readString();

        uint16_t count = stream.readUnsignedShort();
        resource_packs.resize(count);
        for (auto& pack : resource_packs) {
            pack.pack_id = stream.readUUID();
            pack.version = stream.readString();
            pack.size = stream.readUnsignedInt64();
            pack.content_key = stream.readString();
            pack.sub_pack_name = stream.readString();
            pack.content_id = stream.readString();
            pack.has_scripts = stream.readBool();
            pack.is_addon_pack = stream.readBool();
            pack.is_ray_tracing_capable = stream.readBool();
            pack.cdn_url = stream.readString();
        }
    }
};

// =============================================================================
// ResourcePackStackPacket (ID: 7)
// Server -> Client: Ordered pack stack to apply
// =============================================================================
struct ResourcePackStackEntry {
    std::string pack_id;
    std::string version;
    std::string sub_pack_name;
};

class ResourcePackStackPacket : public Packet {
public:
    BEDROCK_PACKET(ResourcePackStackPacket, ResourcePackStack)

    bool must_accept = false;
    std::vector<ResourcePackStackEntry> texture_packs;
    std::string base_game_version;
    // Experiments
    std::vector<std::pair<std::string, bool>> experiments;
    bool has_previously_used_experiments = false;
    bool include_editor_packs = false; // added in v671

    void write(BinaryStream& stream) const override {
        stream.writeBool(must_accept);

        // Texture/resource pack stack (behavior packs removed in v893)
        stream.writeUnsignedVarInt(static_cast<uint32_t>(texture_packs.size()));
        for (auto& tp : texture_packs) {
            stream.writeString(tp.pack_id);
            stream.writeString(tp.version);
            stream.writeString(tp.sub_pack_name);
        }

        stream.writeString(base_game_version);

        stream.writeUnsignedInt(static_cast<uint32_t>(experiments.size()));
        for (auto& [name, enabled] : experiments) {
            stream.writeString(name);
            stream.writeBool(enabled);
        }
        stream.writeBool(has_previously_used_experiments);
        stream.writeBool(include_editor_packs);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        must_accept = stream.readBool();

        uint32_t tpCount = stream.readUnsignedVarInt();
        texture_packs.resize(tpCount);
        for (auto& tp : texture_packs) {
            tp.pack_id = stream.readString();
            tp.version = stream.readString();
            tp.sub_pack_name = stream.readString();
        }

        base_game_version = stream.readString();

        uint32_t expCount = stream.readUnsignedInt();
        experiments.resize(expCount);
        for (auto& [name, enabled] : experiments) {
            name = stream.readString();
            enabled = stream.readBool();
        }
        has_previously_used_experiments = stream.readBool();
        include_editor_packs = stream.readBool();
    }
};

// =============================================================================
// ResourcePackClientResponsePacket (ID: 8)
// Client -> Server: Resource pack download status
// =============================================================================
class ResourcePackClientResponsePacket : public Packet {
public:
    BEDROCK_PACKET(ResourcePackClientResponsePacket, ResourcePackClientResponse)

    ResourcePackResponseType response = ResourcePackResponseType::None;
    std::vector<std::string> pack_ids;

    void write(BinaryStream& stream) const override {
        stream.writeByte(static_cast<uint8_t>(response));
        stream.writeUnsignedShort(static_cast<uint16_t>(pack_ids.size()));
        for (auto& id : pack_ids) {
            stream.writeString(id);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        response = static_cast<ResourcePackResponseType>(stream.readByte());
        uint16_t count = stream.readUnsignedShort();
        pack_ids.resize(count);
        for (auto& id : pack_ids) {
            id = stream.readString();
        }
    }
};

// =============================================================================
// StartGamePacket (ID: 11)
// Server -> Client: Sent after resource packs are ready, contains all world info
// =============================================================================

// ── Sub-structures ───────────────────────────────────────────────

struct SpawnSettings {
    int16_t type = 0;
    std::string user_defined_biome_name;
    int32_t dimension = 0;

    void write(BinaryStream& stream) const {
        stream.writeSignedShort(type);
        stream.writeString(user_defined_biome_name);
        stream.writeVarInt(dimension);
    }
    void read(ReadOnlyBinaryStream& stream) {
        type = stream.readSignedShort();
        user_defined_biome_name = stream.readString();
        dimension = stream.readVarInt();
    }
};

struct ExperimentEntry {
    std::string name;
    bool enabled = false;
};

struct Experiments {
    std::vector<ExperimentEntry> active_toggles;
    std::vector<ExperimentEntry> always_on_toggles;
    bool were_any_ever_toggled = false;

    void write(BinaryStream& stream) const {
        uint32_t total = static_cast<uint32_t>(active_toggles.size() + always_on_toggles.size());
        stream.writeUnsignedInt(total);
        for (auto& e : active_toggles) {
            stream.writeString(e.name);
            stream.writeBool(e.enabled);
        }
        for (auto& e : always_on_toggles) {
            stream.writeString(e.name);
            stream.writeBool(e.enabled);
        }
        stream.writeBool(were_any_ever_toggled);
    }
    void read(ReadOnlyBinaryStream& stream) {
        uint32_t total = stream.readUnsignedInt();
        active_toggles.resize(total);
        for (auto& e : active_toggles) {
            e.name = stream.readString();
            e.enabled = stream.readBool();
        }
        // always_on_toggles are appended but share the same count
        // In practice, reading 'total' entries covers both lists
        were_any_ever_toggled = stream.readBool();
    }
};

struct EduSharedUriResource {
    std::string button_name;
    std::string link_uri;

    void write(BinaryStream& stream) const {
        stream.writeString(button_name);
        stream.writeString(link_uri);
    }
    void read(ReadOnlyBinaryStream& stream) {
        button_name = stream.readString();
        link_uri = stream.readString();
    }
};

struct SyncedPlayerMovementSettings {
    int32_t rewind_history_size = 40;
    bool server_authoritative_block_breaking = false;

    void write(BinaryStream& stream) const {
        stream.writeVarInt(rewind_history_size);
        stream.writeBool(server_authoritative_block_breaking);
    }
    void read(ReadOnlyBinaryStream& stream) {
        rewind_history_size = stream.readVarInt();
        server_authoritative_block_breaking = stream.readBool();
    }
};

struct BlockPropertyEntry {
    std::string block_name;
    std::shared_ptr<NbtTag> block_definition; // CompoundTag
};

struct ServerTelemetryData {
    std::string server_id;
    std::string scenario_id;
    std::string world_id;
    std::string owner_id;

    void write(BinaryStream& stream) const {
        stream.writeString(server_id);
        stream.writeString(scenario_id);
        stream.writeString(world_id);
        stream.writeString(owner_id);
    }
    void read(ReadOnlyBinaryStream& stream) {
        server_id = stream.readString();
        scenario_id = stream.readString();
        world_id = stream.readString();
        owner_id = stream.readString();
    }
};

struct LevelSettings {
    uint64_t seed = 0;
    SpawnSettings spawn_settings;
    int32_t generator_type = 0;
    int32_t game_type = 0;
    bool is_hardcore = false;
    int32_t difficulty = 0;
    BlockPos default_spawn_position;
    bool achievements_disabled = false;
    int32_t editor_world_type = 0;
    bool is_created_in_editor = false;
    bool is_exported_from_editor = false;
    int32_t day_cycle_stop_time = 0;
    int32_t education_edition_offer = 0;
    bool education_features_enabled = false;
    std::string education_product_id;
    float rain_level = 0.0f;
    float lightning_level = 0.0f;
    bool has_confirmed_platform_locked_content = false;
    bool is_multiplayer = false;
    bool lan_broadcast = false;
    int32_t xbox_live_broadcast_setting = 0;
    int32_t platform_broadcast_setting = 0;
    bool commands_enabled = false;
    bool texture_packs_required = false;
    std::vector<GameRule> game_rules;
    Experiments experiments;
    bool bonus_chest_enabled = false;
    bool start_with_map_enabled = false;
    int32_t player_permissions = 0;
    int32_t server_chunk_tick_range = 0;
    bool has_locked_behavior_pack = false;
    bool has_locked_resource_pack = false;
    bool is_from_locked_template = false;
    bool use_msa_gamertags_only = false;
    bool is_from_world_template = false;
    bool is_world_template_option_locked = false;
    bool only_spawn_v1_villagers = false;
    bool persona_disabled = false;
    bool custom_skins_disabled = false;
    bool emote_chat_muted = false;
    std::string base_game_version; // "0.0.0" format
    int32_t limited_world_width = 0;
    int32_t limited_world_depth = 0;
    bool nether_type = false;
    EduSharedUriResource edu_shared_uri_resource;
    bool force_experimental_gameplay = false;
    uint8_t chat_restriction_level = 0;
    bool disable_player_interactions = false;

    void write(BinaryStream& stream) const {
        stream.writeUnsignedInt64(seed);
        spawn_settings.write(stream);
        stream.writeVarInt(generator_type);
        stream.writeVarInt(game_type);
        stream.writeBool(is_hardcore);
        stream.writeVarInt(difficulty);
        stream.writeBlockPos(default_spawn_position);
        stream.writeBool(achievements_disabled);
        stream.writeVarInt(editor_world_type);
        stream.writeBool(is_created_in_editor);
        stream.writeBool(is_exported_from_editor);
        stream.writeVarInt(day_cycle_stop_time);
        stream.writeVarInt(education_edition_offer);
        stream.writeBool(education_features_enabled);
        stream.writeString(education_product_id);
        stream.writeFloat(rain_level);
        stream.writeFloat(lightning_level);
        stream.writeBool(has_confirmed_platform_locked_content);
        stream.writeBool(is_multiplayer);
        stream.writeBool(lan_broadcast);
        stream.writeVarInt(xbox_live_broadcast_setting);
        stream.writeVarInt(platform_broadcast_setting);
        stream.writeBool(commands_enabled);
        stream.writeBool(texture_packs_required);
        // Game rules
        stream.writeUnsignedVarInt(static_cast<uint32_t>(game_rules.size()));
        for (auto& rule : game_rules) {
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
        experiments.write(stream);
        stream.writeBool(bonus_chest_enabled);
        stream.writeBool(start_with_map_enabled);
        stream.writeVarInt(player_permissions);
        stream.writeSignedInt(server_chunk_tick_range);
        stream.writeBool(has_locked_behavior_pack);
        stream.writeBool(has_locked_resource_pack);
        stream.writeBool(is_from_locked_template);
        stream.writeBool(use_msa_gamertags_only);
        stream.writeBool(is_from_world_template);
        stream.writeBool(is_world_template_option_locked);
        stream.writeBool(only_spawn_v1_villagers);
        stream.writeBool(persona_disabled);
        stream.writeBool(custom_skins_disabled);
        stream.writeBool(emote_chat_muted);
        stream.writeString(base_game_version);
        stream.writeSignedInt(limited_world_width);
        stream.writeSignedInt(limited_world_depth);
        stream.writeBool(nether_type);
        edu_shared_uri_resource.write(stream);
        stream.writeBool(force_experimental_gameplay);
        stream.writeByte(chat_restriction_level);
        stream.writeBool(disable_player_interactions);
    }

    void read(ReadOnlyBinaryStream& stream) {
        seed = stream.readUnsignedInt64();
        spawn_settings.read(stream);
        generator_type = stream.readVarInt();
        game_type = stream.readVarInt();
        is_hardcore = stream.readBool();
        difficulty = stream.readVarInt();
        default_spawn_position = stream.readBlockPos();
        achievements_disabled = stream.readBool();
        editor_world_type = stream.readVarInt();
        is_created_in_editor = stream.readBool();
        is_exported_from_editor = stream.readBool();
        day_cycle_stop_time = stream.readVarInt();
        education_edition_offer = stream.readVarInt();
        education_features_enabled = stream.readBool();
        education_product_id = stream.readString();
        rain_level = stream.readFloat();
        lightning_level = stream.readFloat();
        has_confirmed_platform_locked_content = stream.readBool();
        is_multiplayer = stream.readBool();
        lan_broadcast = stream.readBool();
        xbox_live_broadcast_setting = stream.readVarInt();
        platform_broadcast_setting = stream.readVarInt();
        commands_enabled = stream.readBool();
        texture_packs_required = stream.readBool();
        // Game rules
        uint32_t rule_count = stream.readUnsignedVarInt();
        game_rules.resize(rule_count);
        for (auto& rule : game_rules) {
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
        experiments.read(stream);
        bonus_chest_enabled = stream.readBool();
        start_with_map_enabled = stream.readBool();
        player_permissions = stream.readVarInt();
        server_chunk_tick_range = stream.readSignedInt();
        has_locked_behavior_pack = stream.readBool();
        has_locked_resource_pack = stream.readBool();
        is_from_locked_template = stream.readBool();
        use_msa_gamertags_only = stream.readBool();
        is_from_world_template = stream.readBool();
        is_world_template_option_locked = stream.readBool();
        only_spawn_v1_villagers = stream.readBool();
        persona_disabled = stream.readBool();
        custom_skins_disabled = stream.readBool();
        emote_chat_muted = stream.readBool();
        base_game_version = stream.readString();
        limited_world_width = stream.readSignedInt();
        limited_world_depth = stream.readSignedInt();
        nether_type = stream.readBool();
        edu_shared_uri_resource.read(stream);
        force_experimental_gameplay = stream.readBool();
        chat_restriction_level = stream.readByte();
        disable_player_interactions = stream.readBool();
    }
};

class StartGamePacket : public Packet {
public:
    BEDROCK_PACKET(StartGamePacket, StartGame)

    // Core player info
    ActorUniqueID entity_id;
    ActorRuntimeID runtime_id;
    int32_t player_game_type = 0;
    Vec3 position;
    Vec2 rotation;

    // Level settings (huge sub-structure)
    LevelSettings settings;

    // Additional fields after LevelSettings
    std::string level_id;
    std::string level_name;
    std::string template_content_identity;
    bool is_trial = false;
    SyncedPlayerMovementSettings movement_settings;
    uint64_t level_current_time = 0;
    int32_t enchantment_seed = 0;

    // Block properties (data-driven block palette)
    std::vector<BlockPropertyEntry> block_properties;

    // More fields
    std::string multiplayer_correlation_id;
    bool enable_item_stack_net_manager = true;
    std::string server_version;
    std::shared_ptr<NbtTag> player_property_data; // CompoundTag
    uint64_t server_block_type_registry_checksum = 0;
    UUID world_template_id;
    bool server_enabled_client_side_generation = false;
    bool block_network_ids_are_hashes = true;
    bool server_auth_sound_enabled = false; // NetworkPermissions
    bool has_server_join_info = false;
    ServerTelemetryData server_telemetry_data;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(entity_id);
        stream.writeActorRuntimeID(runtime_id);
        stream.writeVarInt(player_game_type);
        stream.writeVec3(position);
        stream.writeVec2(rotation);

        settings.write(stream);

        stream.writeString(level_id);
        stream.writeString(level_name);
        stream.writeString(template_content_identity);
        stream.writeBool(is_trial);
        movement_settings.write(stream);
        stream.writeUnsignedInt64(level_current_time);
        stream.writeVarInt(enchantment_seed);

        // Block properties
        stream.writeUnsignedVarInt(static_cast<uint32_t>(block_properties.size()));
        for (auto& bp : block_properties) {
            stream.writeString(bp.block_name);
            if (bp.block_definition) {
                NbtTag::writeNetworkNbt(stream, *bp.block_definition);
            } else {
                // Write empty compound
                auto empty = NbtTag::makeCompound();
                NbtTag::writeNetworkNbt(stream, *empty);
            }
        }

        stream.writeString(multiplayer_correlation_id);
        stream.writeBool(enable_item_stack_net_manager);
        stream.writeString(server_version);

        if (player_property_data) {
            NbtTag::writeNetworkNbt(stream, *player_property_data);
        } else {
            auto empty = NbtTag::makeCompound();
            NbtTag::writeNetworkNbt(stream, *empty);
        }

        stream.writeUnsignedInt64(server_block_type_registry_checksum);
        stream.writeUUID(world_template_id);
        stream.writeBool(server_enabled_client_side_generation);
        stream.writeBool(block_network_ids_are_hashes);

        // NetworkPermissions
        stream.writeBool(server_auth_sound_enabled);

        stream.writeBool(has_server_join_info);
        server_telemetry_data.write(stream);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_id = stream.readActorUniqueID();
        runtime_id = stream.readActorRuntimeID();
        player_game_type = stream.readVarInt();
        position = stream.readVec3();
        rotation = stream.readVec2();

        settings.read(stream);

        level_id = stream.readString();
        level_name = stream.readString();
        template_content_identity = stream.readString();
        is_trial = stream.readBool();
        movement_settings.read(stream);
        level_current_time = stream.readUnsignedInt64();
        enchantment_seed = stream.readVarInt();

        // Block properties
        uint32_t bp_count = stream.readUnsignedVarInt();
        block_properties.resize(bp_count);
        for (auto& bp : block_properties) {
            bp.block_name = stream.readString();
            bp.block_definition = NbtTag::readNetworkNbt(stream);
        }

        multiplayer_correlation_id = stream.readString();
        enable_item_stack_net_manager = stream.readBool();
        server_version = stream.readString();
        player_property_data = NbtTag::readNetworkNbt(stream);
        server_block_type_registry_checksum = stream.readUnsignedInt64();
        world_template_id = stream.readUUID();
        server_enabled_client_side_generation = stream.readBool();
        block_network_ids_are_hashes = stream.readBool();

        // NetworkPermissions
        server_auth_sound_enabled = stream.readBool();

        has_server_join_info = stream.readBool();
        server_telemetry_data.read(stream);
    }
};

} // namespace bedrock
