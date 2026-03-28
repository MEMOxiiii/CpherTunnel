// Quick diagnostic: dump the StartGamePacket bytes for analysis
#include "cphertunnel/cphertunnel.hpp"
#include <iostream>
#include <cstdio>

using namespace cphertunnel;

int main() {
    // Create same StartGamePacket as test_server
    StartGamePacket sg;
    sg.entity_id = {1};
    sg.runtime_id = {1};
    sg.player_game_type = 1; // Creative
    sg.position = {0.0f, 10.0f, 0.0f};
    sg.rotation = {0.0f, 0.0f};

    auto& s = sg.settings;
    s.seed = 12345;
    s.spawn_settings.type = 0;
    s.spawn_settings.dimension = 0;
    s.generator_type = 2;
    s.game_type = 1;
    s.difficulty = 1;
    s.default_spawn_position = {0, 10, 0};
    s.achievements_disabled = true;
    s.editor_world_type = 0;
    s.is_created_in_editor = false;
    s.is_exported_from_editor = false;
    s.day_cycle_stop_time = 6000;
    s.education_features_enabled = false;
    s.rain_level = 0.0f;
    s.lightning_level = 0.0f;
    s.is_multiplayer = true;
    s.lan_broadcast = true;
    s.xbox_live_broadcast_setting = 0;
    s.platform_broadcast_setting = 0;
    s.commands_enabled = true;
    s.texture_packs_required = false;
    s.base_game_version = "*";
    s.limited_world_width = 0;
    s.limited_world_depth = 0;
    s.chat_restriction_level = 0;
    s.disable_player_interactions = false;
    s.server_chunk_tick_range = 4;
    s.player_permissions = 2;

    sg.level_id = "test_level";
    sg.level_name = "Flat World Test";
    sg.template_content_identity = "";
    sg.is_trial = false;
    sg.movement_settings.rewind_history_size = 40;
    sg.movement_settings.server_authoritative_block_breaking = false;
    sg.level_current_time = 6000;
    sg.enchantment_seed = 0;
    sg.multiplayer_correlation_id = "";
    sg.enable_item_stack_net_manager = true;
    sg.server_version = "1.26.1";
    sg.server_block_type_registry_checksum = 0;
    sg.world_template_id = {};
    sg.server_enabled_client_side_generation = false;
    sg.block_network_ids_are_hashes = true;
    sg.server_auth_sound_enabled = false;
    sg.has_server_join_info = false;

    // Serialize just the payload (no header)
    BinaryStream stream;
    sg.write(stream);
    auto& buf = stream.getBuffer();

    std::cout << "StartGamePacket payload: " << buf.size() << " bytes" << std::endl;

    // Annotated decode: read back field by field
    ReadOnlyBinaryStream rs(reinterpret_cast<const uint8_t*>(buf.data()), buf.size());

    auto dumpPos = [&](const char* name) {
        std::cout << "  @" << rs.getReadPointer() << ": " << name;
    };

    dumpPos("entity_id"); auto eid = rs.readActorUniqueID(); std::cout << " = " << eid.id << std::endl;
    dumpPos("runtime_id"); auto rid = rs.readActorRuntimeID(); std::cout << " = " << rid.id << std::endl;
    dumpPos("player_game_type"); auto pgt = rs.readVarInt(); std::cout << " = " << pgt << std::endl;
    dumpPos("position"); auto pos = rs.readVec3(); std::cout << " = {" << pos.x << ", " << pos.y << ", " << pos.z << "}" << std::endl;
    dumpPos("rotation"); auto rot = rs.readVec2(); std::cout << " = {" << rot.x << ", " << rot.y << "}" << std::endl;

    // LevelSettings
    std::cout << "  --- LevelSettings ---" << std::endl;
    dumpPos("seed"); auto seed = rs.readUnsignedInt64(); std::cout << " = " << seed << std::endl;
    dumpPos("spawn_settings.type"); auto st = rs.readSignedShort(); std::cout << " = " << st << std::endl;
    dumpPos("spawn_settings.biome"); auto sb = rs.readString(); std::cout << " = \"" << sb << "\"" << std::endl;
    dumpPos("spawn_settings.dim"); auto sd = rs.readVarInt(); std::cout << " = " << sd << std::endl;
    dumpPos("generator_type"); auto gt = rs.readVarInt(); std::cout << " = " << gt << std::endl;
    dumpPos("game_type"); auto gtp = rs.readVarInt(); std::cout << " = " << gtp << std::endl;
    dumpPos("is_hardcore"); auto hc = rs.readBool(); std::cout << " = " << hc << std::endl;
    dumpPos("difficulty"); auto diff = rs.readVarInt(); std::cout << " = " << diff << std::endl;
    dumpPos("default_spawn_pos"); auto dsp = rs.readBlockPos(); std::cout << " = {" << dsp.x << ", " << dsp.y << ", " << dsp.z << "}" << std::endl;
    dumpPos("achievements_disabled"); auto ad = rs.readBool(); std::cout << " = " << ad << std::endl;
    dumpPos("editor_world_type"); auto ewt = rs.readVarInt(); std::cout << " = " << ewt << std::endl;
    dumpPos("is_created_in_editor"); auto ice = rs.readBool(); std::cout << " = " << ice << std::endl;
    dumpPos("is_exported_from_editor"); auto ixe = rs.readBool(); std::cout << " = " << ixe << std::endl;
    dumpPos("day_cycle_stop_time"); auto dcst = rs.readVarInt(); std::cout << " = " << dcst << std::endl;
    dumpPos("education_edition_offer"); auto eeo = rs.readVarInt(); std::cout << " = " << eeo << std::endl;
    dumpPos("education_features"); auto ef = rs.readBool(); std::cout << " = " << ef << std::endl;
    dumpPos("education_product_id"); auto epi = rs.readString(); std::cout << " = \"" << epi << "\"" << std::endl;
    dumpPos("rain_level"); auto rl = rs.readFloat(); std::cout << " = " << rl << std::endl;
    dumpPos("lightning_level"); auto ll = rs.readFloat(); std::cout << " = " << ll << std::endl;
    dumpPos("platform_locked"); auto pl = rs.readBool(); std::cout << " = " << pl << std::endl;
    dumpPos("is_multiplayer"); auto im = rs.readBool(); std::cout << " = " << im << std::endl;
    dumpPos("lan_broadcast"); auto lb = rs.readBool(); std::cout << " = " << lb << std::endl;
    dumpPos("xbox_broadcast"); auto xb = rs.readVarInt(); std::cout << " = " << xb << std::endl;
    dumpPos("platform_broadcast"); auto pb = rs.readVarInt(); std::cout << " = " << pb << std::endl;
    dumpPos("commands_enabled"); auto ce = rs.readBool(); std::cout << " = " << ce << std::endl;
    dumpPos("texture_packs_required"); auto tpr = rs.readBool(); std::cout << " = " << tpr << std::endl;
    dumpPos("game_rules_count"); auto grc = rs.readUnsignedVarInt(); std::cout << " = " << grc << std::endl;
    dumpPos("experiments_count"); auto ec = rs.readUnsignedInt(); std::cout << " = " << ec << std::endl;
    dumpPos("experiments_toggled"); auto et = rs.readBool(); std::cout << " = " << et << std::endl;
    dumpPos("bonus_chest"); auto bce = rs.readBool(); std::cout << " = " << bce << std::endl;
    dumpPos("start_with_map"); auto swm = rs.readBool(); std::cout << " = " << swm << std::endl;
    dumpPos("player_permissions"); auto pp = rs.readVarInt(); std::cout << " = " << pp << std::endl;
    dumpPos("server_chunk_tick_range"); auto sctr = rs.readSignedInt(); std::cout << " = " << sctr << std::endl;
    dumpPos("locked_behavior_pack"); auto lbp = rs.readBool(); std::cout << " = " << lbp << std::endl;
    dumpPos("locked_resource_pack"); auto lrp = rs.readBool(); std::cout << " = " << lrp << std::endl;
    dumpPos("from_locked_template"); auto flt = rs.readBool(); std::cout << " = " << flt << std::endl;
    dumpPos("msa_gamertags_only"); auto mgo = rs.readBool(); std::cout << " = " << mgo << std::endl;
    dumpPos("from_world_template"); auto fwt = rs.readBool(); std::cout << " = " << fwt << std::endl;
    dumpPos("template_option_locked"); auto tol = rs.readBool(); std::cout << " = " << tol << std::endl;
    dumpPos("v1_villagers"); auto v1v = rs.readBool(); std::cout << " = " << v1v << std::endl;
    dumpPos("persona_disabled"); auto pdd = rs.readBool(); std::cout << " = " << pdd << std::endl;
    dumpPos("custom_skins_disabled"); auto csd = rs.readBool(); std::cout << " = " << csd << std::endl;
    dumpPos("emote_chat_muted"); auto ecm = rs.readBool(); std::cout << " = " << ecm << std::endl;
    dumpPos("base_game_version"); auto bgv = rs.readString(); std::cout << " = \"" << bgv << "\"" << std::endl;
    dumpPos("limited_world_width"); auto lww = rs.readSignedInt(); std::cout << " = " << lww << std::endl;
    dumpPos("limited_world_depth"); auto lwd = rs.readSignedInt(); std::cout << " = " << lwd << std::endl;
    dumpPos("nether_type"); auto nt = rs.readBool(); std::cout << " = " << nt << std::endl;
    dumpPos("edu_uri.button"); auto eub = rs.readString(); std::cout << " = \"" << eub << "\"" << std::endl;
    dumpPos("edu_uri.link"); auto eul = rs.readString(); std::cout << " = \"" << eul << "\"" << std::endl;
    dumpPos("force_experimental"); auto fe = rs.readBool(); std::cout << " = " << fe << std::endl;
    dumpPos("chat_restriction"); auto cr = rs.readByte(); std::cout << " = " << (int)cr << std::endl;
    dumpPos("disable_player_interactions"); auto dpi = rs.readBool(); std::cout << " = " << dpi << std::endl;
    std::cout << "  --- End LevelSettings ---" << std::endl;

    dumpPos("level_id"); auto lid = rs.readString(); std::cout << " = \"" << lid << "\"" << std::endl;
    dumpPos("level_name"); auto lnm = rs.readString(); std::cout << " = \"" << lnm << "\"" << std::endl;
    dumpPos("template_content_identity"); auto tci = rs.readString(); std::cout << " = \"" << tci << "\"" << std::endl;
    dumpPos("is_trial"); auto it = rs.readBool(); std::cout << " = " << it << std::endl;
    dumpPos("movement_settings.rewind"); auto msr = rs.readVarInt(); std::cout << " = " << msr << std::endl;
    dumpPos("movement_settings.sa_bb"); auto msb = rs.readBool(); std::cout << " = " << msb << std::endl;
    dumpPos("level_current_time"); auto lct = rs.readUnsignedInt64(); std::cout << " = " << lct << std::endl;
    dumpPos("enchantment_seed"); auto es = rs.readVarInt(); std::cout << " = " << es << std::endl;
    dumpPos("block_properties_count"); auto bpc = rs.readUnsignedVarInt(); std::cout << " = " << bpc << std::endl;
    dumpPos("multiplayer_correlation_id"); auto mci = rs.readString(); std::cout << " = \"" << mci << "\"" << std::endl;
    dumpPos("enable_item_stack_net_mgr"); auto eisnm = rs.readBool(); std::cout << " = " << eisnm << std::endl;
    dumpPos("server_version"); auto sv = rs.readString(); std::cout << " = \"" << sv << "\"" << std::endl;

    // player_property_data (CompoundTag NBT)
    dumpPos("player_property_data (NBT)");
    auto ppd = NbtTag::readNetworkNbt(rs);
    std::cout << " read OK (compound with " << (ppd ? ppd->compoundVal.size() : 0) << " entries)" << std::endl;

    dumpPos("block_registry_checksum"); auto brc = rs.readUnsignedInt64(); std::cout << " = " << brc << std::endl;
    dumpPos("world_template_id"); auto wti = rs.readUUID(); std::cout << " = {" << wti.most_significant << ", " << wti.least_significant << "}" << std::endl;
    dumpPos("client_side_generation"); auto csg = rs.readBool(); std::cout << " = " << csg << std::endl;
    dumpPos("block_ids_are_hashes"); auto biah = rs.readBool(); std::cout << " = " << biah << std::endl;
    dumpPos("server_auth_sound"); auto sas = rs.readBool(); std::cout << " = " << sas << std::endl;
    dumpPos("has_server_join_info"); auto hsji = rs.readBool(); std::cout << " = " << hsji << std::endl;

    // ServerTelemetryData
    dumpPos("telemetry.server_id"); auto tid = rs.readString(); std::cout << " = \"" << tid << "\"" << std::endl;
    dumpPos("telemetry.scenario_id"); auto tsid = rs.readString(); std::cout << " = \"" << tsid << "\"" << std::endl;
    dumpPos("telemetry.world_id"); auto twid = rs.readString(); std::cout << " = \"" << twid << "\"" << std::endl;
    dumpPos("telemetry.owner_id"); auto toid = rs.readString(); std::cout << " = \"" << toid << "\"" << std::endl;

    std::cout << "\nFinal read position: " << rs.getReadPointer() << " / " << buf.size() << std::endl;
    std::cout << "Overflow: " << (rs.hasOverflowed() ? "YES" : "no") << std::endl;
    std::cout << "Remaining: " << rs.getUnreadLength() << " bytes" << std::endl;

    // Full hex dump
    std::cout << "\nFull hex dump:" << std::endl;
    for (size_t i = 0; i < buf.size(); ++i) {
        char h[4]; snprintf(h, sizeof(h), "%02X ", (uint8_t)buf[i]);
        std::cout << h;
        if ((i + 1) % 32 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;

    return 0;
}
