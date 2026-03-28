// CpherTunnel - StartGamePacket
#pragma once
#include "../packet.hpp"
#include "../nbt.hpp"
namespace cphertunnel {

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
    CPHERTUNNEL_PACKET(StartGamePacket, StartGame)

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

} // namespace cphertunnel
