// =============================================================================
// CpherTunnel - Comprehensive Test Suite
// Tests all major components: VarInt, BinaryStream, Packets, Codec, RakNet
// =============================================================================

#include "cphertunnel/cphertunnel.hpp"
#include <iostream>
#include <cassert>
#include <cstring>
#include <sstream>
#include <cmath>
#include <set>

// Simple test framework macros
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    static void test_##name(); \
    struct TestRegister_##name { \
        TestRegister_##name() { \
            std::cout << "  Running: " #name "... "; \
            try { \
                test_##name(); \
                std::cout << "PASSED" << std::endl; \
                tests_passed++; \
            } catch (const std::exception& e) { \
                std::cout << "FAILED: " << e.what() << std::endl; \
                tests_failed++; \
            } catch (...) { \
                std::cout << "FAILED: unknown exception" << std::endl; \
                tests_failed++; \
            } \
        } \
    } register_##name; \
    static void test_##name()

#define ASSERT_EQ(a, b) \
    if ((a) != (b)) { \
        std::ostringstream oss; \
        oss << "Expected " << (a) << " == " << (b) << " at line " << __LINE__; \
        throw std::runtime_error(oss.str()); \
    }

#define ASSERT_TRUE(expr) \
    if (!(expr)) { \
        throw std::runtime_error(std::string("Assertion failed at line ") + std::to_string(__LINE__)); \
    }

#define ASSERT_NEAR(a, b, eps) \
    if (std::abs((a) - (b)) > (eps)) { \
        std::ostringstream oss; \
        oss << "Expected " << (a) << " ~= " << (b) << " (eps=" << (eps) << ") at line " << __LINE__; \
        throw std::runtime_error(oss.str()); \
    }

#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)
#define SECTION(label) \
    static auto CONCAT(_section_, __COUNTER__) = (std::cout << "\n=== " label " ===" << std::endl, 0)

using namespace cphertunnel;

// =============================================================================
// SECTION: VarInt Tests
// =============================================================================
SECTION("VarInt Tests");

TEST(varint_unsigned_zero) {
    std::string buf;
    VarInt::writeUnsignedVarInt(buf, 0);
    ASSERT_EQ(buf.size(), 1u);
    ASSERT_EQ(static_cast<uint8_t>(buf[0]), 0);
    
    size_t offset = 0;
    uint32_t val = VarInt::readUnsignedVarInt(
        reinterpret_cast<const uint8_t*>(buf.data()), buf.size(), offset);
    ASSERT_EQ(val, 0u);
    ASSERT_EQ(offset, 1u);
}

TEST(varint_unsigned_small) {
    std::string buf;
    VarInt::writeUnsignedVarInt(buf, 127);
    ASSERT_EQ(buf.size(), 1u);
    ASSERT_EQ(static_cast<uint8_t>(buf[0]), 127);
    
    size_t off = 0;
    ASSERT_EQ(VarInt::readUnsignedVarInt(
        reinterpret_cast<const uint8_t*>(buf.data()), buf.size(), off), 127u);
}

TEST(varint_unsigned_medium) {
    std::string buf;
    VarInt::writeUnsignedVarInt(buf, 300);
    size_t off = 0;
    ASSERT_EQ(VarInt::readUnsignedVarInt(
        reinterpret_cast<const uint8_t*>(buf.data()), buf.size(), off), 300u);
}

TEST(varint_unsigned_large) {
    std::string buf;
    VarInt::writeUnsignedVarInt(buf, 0xFFFFFFFF);
    size_t off = 0;
    ASSERT_EQ(VarInt::readUnsignedVarInt(
        reinterpret_cast<const uint8_t*>(buf.data()), buf.size(), off), 0xFFFFFFFFu);
}

TEST(varint_signed_positive) {
    std::string buf;
    VarInt::writeVarInt(buf, 150);
    size_t off = 0;
    ASSERT_EQ(VarInt::readVarInt(
        reinterpret_cast<const uint8_t*>(buf.data()), buf.size(), off), 150);
}

TEST(varint_signed_negative) {
    std::string buf;
    VarInt::writeVarInt(buf, -150);
    size_t off = 0;
    ASSERT_EQ(VarInt::readVarInt(
        reinterpret_cast<const uint8_t*>(buf.data()), buf.size(), off), -150);
}

TEST(varint_signed_zero) {
    std::string buf;
    VarInt::writeVarInt(buf, 0);
    size_t off = 0;
    ASSERT_EQ(VarInt::readVarInt(
        reinterpret_cast<const uint8_t*>(buf.data()), buf.size(), off), 0);
}

TEST(varint64_large) {
    std::string buf;
    int64_t big = 0x7FFFFFFFFFFFFFFFLL;
    VarInt::writeVarInt64(buf, big);
    size_t off = 0;
    ASSERT_EQ(VarInt::readVarInt64(
        reinterpret_cast<const uint8_t*>(buf.data()), buf.size(), off), big);
}

TEST(varint64_negative) {
    std::string buf;
    VarInt::writeVarInt64(buf, -12345678LL);
    size_t off = 0;
    ASSERT_EQ(VarInt::readVarInt64(
        reinterpret_cast<const uint8_t*>(buf.data()), buf.size(), off), -12345678LL);
}

// =============================================================================
// SECTION: BinaryStream Tests
// =============================================================================
SECTION("BinaryStream Tests");

TEST(binarystream_bool) {
    BinaryStream write;
    write.writeBool(true);
    write.writeBool(false);
    
    ReadOnlyBinaryStream read(write.getBuffer());
    ASSERT_EQ(read.readBool(), true);
    ASSERT_EQ(read.readBool(), false);
}

TEST(binarystream_byte) {
    BinaryStream write;
    write.writeByte(0x42);
    write.writeByte(0xFF);
    
    ReadOnlyBinaryStream read(write.getBuffer());
    ASSERT_EQ(read.readByte(), 0x42);
    ASSERT_EQ(read.readByte(), 0xFF);
}

TEST(binarystream_short) {
    BinaryStream write;
    write.writeUnsignedShort(0x1234);
    write.writeSignedShort(0x5678);
    
    ReadOnlyBinaryStream read(write.getBuffer());
    ASSERT_EQ(read.readUnsignedShort(), 0x1234);
    ASSERT_EQ(read.readSignedShort(), 0x5678);
}

TEST(binarystream_int) {
    BinaryStream write;
    write.writeSignedInt(0x12345678);
    write.writeSignedBigEndianInt(static_cast<int32_t>(0xDEADBEEF));
    
    ReadOnlyBinaryStream read(write.getBuffer());
    ASSERT_EQ(read.readSignedInt(), 0x12345678);
    ASSERT_EQ(read.readSignedBigEndianInt(), static_cast<int32_t>(0xDEADBEEF));
}

TEST(binarystream_float) {
    BinaryStream write;
    write.writeFloat(3.14f);
    write.writeDouble(2.71828);
    
    ReadOnlyBinaryStream read(write.getBuffer());
    ASSERT_NEAR(read.readFloat(), 3.14f, 0.001f);
    ASSERT_NEAR(read.readDouble(), 2.71828, 0.00001);
}

TEST(binarystream_varint) {
    BinaryStream write;
    write.writeVarInt(42);
    write.writeVarInt(-42);
    write.writeUnsignedVarInt(300);
    
    ReadOnlyBinaryStream read(write.getBuffer());
    ASSERT_EQ(read.readVarInt(), 42);
    ASSERT_EQ(read.readVarInt(), -42);
    ASSERT_EQ(read.readUnsignedVarInt(), 300u);
}

TEST(binarystream_int64) {
    BinaryStream write;
    write.writeSignedInt64(0x123456789ABCDEF0LL);
    write.writeUnsignedVarInt64(0xFEDCBA9876543210ULL);
    
    ReadOnlyBinaryStream read(write.getBuffer());
    ASSERT_EQ(read.readSignedInt64(), 0x123456789ABCDEF0LL);
    ASSERT_EQ(read.readUnsignedVarInt64(), 0xFEDCBA9876543210ULL);
}

TEST(binarystream_string) {
    BinaryStream write;
    write.writeString("Hello, Bedrock!");
    write.writeString("");
    write.writeString("Unicode: \xC3\xA9\xC3\xA8"); // é è in UTF-8
    
    ReadOnlyBinaryStream read(write.getBuffer());
    ASSERT_EQ(read.readString(), "Hello, Bedrock!");
    ASSERT_EQ(read.readString(), "");
    ASSERT_EQ(read.readString(), "Unicode: \xC3\xA9\xC3\xA8");
}

TEST(binarystream_vec3) {
    BinaryStream write;
    Vec3 v{1.5f, 2.5f, 3.5f};
    write.writeVec3(v);
    
    ReadOnlyBinaryStream read(write.getBuffer());
    Vec3 result = read.readVec3();
    ASSERT_NEAR(result.x, 1.5f, 0.001f);
    ASSERT_NEAR(result.y, 2.5f, 0.001f);
    ASSERT_NEAR(result.z, 3.5f, 0.001f);
}

TEST(binarystream_blockpos) {
    BinaryStream write;
    BlockPos pos{-100, 64, 200};
    write.writeBlockPos(pos);
    
    ReadOnlyBinaryStream read(write.getBuffer());
    BlockPos result = read.readBlockPos();
    ASSERT_EQ(result.x, -100);
    ASSERT_EQ(result.y, 64);
    ASSERT_EQ(result.z, 200);
}

TEST(binarystream_uuid) {
    BinaryStream write;
    cphertunnel::UUID uuid{0x12345678ABCDEF01ULL, 0xFEDCBA9876543210ULL};
    write.writeUUID(uuid);
    
    ReadOnlyBinaryStream read(write.getBuffer());
    cphertunnel::UUID result = read.readUUID();
    ASSERT_EQ(result.most_significant, 0x12345678ABCDEF01ULL);
    ASSERT_EQ(result.least_significant, 0xFEDCBA9876543210ULL);
}

TEST(binarystream_byte_array) {
    BinaryStream write;
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04, 0x05};
    write.writeByteArray(data);
    
    ReadOnlyBinaryStream read(write.getBuffer());
    auto result = read.readByteArray();
    ASSERT_EQ(result.size(), 5u);
    ASSERT_EQ(result[0], 0x01);
    ASSERT_EQ(result[4], 0x05);
}

// =============================================================================
// SECTION: PacketIds Tests
// =============================================================================
SECTION("PacketId Tests");

TEST(packet_id_to_string) {
    ASSERT_EQ(packetIdToString(PacketId::Login), "Login");
    ASSERT_EQ(packetIdToString(PacketId::PlayStatus), "PlayStatus");
    ASSERT_EQ(packetIdToString(PacketId::Text), "Text");
    ASSERT_EQ(packetIdToString(PacketId::Disconnect), "Disconnect");
    ASSERT_EQ(packetIdToString(PacketId::Transfer), "Transfer");
}

// =============================================================================
// SECTION: Packet Serialization Tests
// =============================================================================
SECTION("Packet Serialization Tests");

// Helper: round-trip test for any packet
template<typename T>
void roundTripTest(const T& original) {
    // Serialize (payload only, no header)
    BinaryStream write_stream;
    original.write(write_stream);
    
    // Deserialize
    T decoded;
    ReadOnlyBinaryStream read_stream(write_stream.getBuffer());
    decoded.read(read_stream);
    
    // Re-serialize and compare bytes
    BinaryStream check_stream;
    decoded.write(check_stream);
    
    auto& buf1 = write_stream.getBuffer();
    auto& buf2 = check_stream.getBuffer();
    
    if (buf1.size() != buf2.size()) {
        throw std::runtime_error(
            "Size mismatch: " + std::to_string(buf1.size()) +
            " vs " + std::to_string(buf2.size()));
    }
    
    if (std::memcmp(buf1.data(), buf2.data(), buf1.size()) != 0) {
        throw std::runtime_error("Data mismatch in round-trip");
    }
}

TEST(packet_request_network_settings) {
    RequestNetworkSettingsPacket pkt;
    pkt.client_network_version = 944;
    roundTripTest(pkt);
}

TEST(packet_network_settings) {
    NetworkSettingsPacket pkt;
    pkt.compression_threshold = 256;
    pkt.compression_algorithm = CompressionAlgorithm::ZLib;
    pkt.client_throttle_enabled = true;
    pkt.client_throttle_threshold = 0;
    pkt.client_throttle_scalar = 0.0f;
    roundTripTest(pkt);
}

TEST(packet_play_status) {
    PlayStatusPacket pkt;
    pkt.status = PlayStatusType::LoginSuccess;
    roundTripTest(pkt);
}

TEST(packet_disconnect) {
    DisconnectPacket pkt;
    pkt.skip_message = false;
    pkt.message = "Server closed";
    roundTripTest(pkt);
}

TEST(packet_text) {
    TextPacket pkt;
    pkt.type = TextType::Chat;
    pkt.needs_translation = false;
    pkt.source_name = "Player1";
    pkt.message = "Hello, world!";
    pkt.xuid = "1234567890";
    pkt.platform_chat_id = "";
    pkt.filtered_message = "";
    roundTripTest(pkt);
}

TEST(packet_set_time) {
    SetTimePacket pkt;
    pkt.time = 12000;
    roundTripTest(pkt);
}

TEST(packet_set_title) {
    SetTitlePacket pkt;
    pkt.title_type = TitleType::Clear;
    pkt.title_text = "Welcome!";
    pkt.fade_in_time = 10;
    pkt.stay_time = 70;
    pkt.fade_out_time = 20;
    pkt.xuid = "";
    pkt.platform_online_id = "";
    pkt.filtered_title_text = "";
    roundTripTest(pkt);
}

TEST(packet_transfer) {
    TransferPacket pkt;
    pkt.server_address = "play.example.com";
    pkt.server_port = 19132;
    pkt.reload_world = false;
    roundTripTest(pkt);
}

TEST(packet_set_difficulty) {
    SetDifficultyPacket pkt;
    pkt.difficulty = 2; // Normal
    roundTripTest(pkt);
}

TEST(packet_set_commands_enabled) {
    SetCommandsEnabledPacket pkt;
    pkt.commands_enabled = true;
    roundTripTest(pkt);
}

TEST(packet_respawn) {
    RespawnPacket pkt;
    pkt.position = {0.0f, 64.0f, 0.0f};
    pkt.state = 1;
    pkt.runtime_entity_id = {42};
    roundTripTest(pkt);
}

TEST(packet_set_health) {
    SetHealthPacket pkt;
    pkt.health = 20;
    roundTripTest(pkt);
}

TEST(packet_set_spawn_position) {
    SetSpawnPositionPacket pkt;
    pkt.spawn_type = 1;
    pkt.player_position = {100, 64, 200};
    pkt.dimension = 0;
    pkt.world_position = {100, 64, 200};
    roundTripTest(pkt);
}

TEST(packet_change_dimension) {
    ChangeDimensionPacket pkt;
    pkt.dimension = 1; // Nether
    pkt.position = {0.0f, 64.0f, 0.0f};
    pkt.respawn = true;
    roundTripTest(pkt);
}

TEST(packet_toast_request) {
    ToastRequestPacket pkt;
    pkt.title = "Achievement";
    pkt.content = "You did it!";
    roundTripTest(pkt);
}

TEST(packet_death_info) {
    DeathInfoPacket pkt;
    pkt.death_cause_message = "lava";
    pkt.death_cause_params = {"Player fell into lava"};
    roundTripTest(pkt);
}

TEST(packet_network_stack_latency) {
    NetworkStackLatencyPacket pkt;
    pkt.timestamp = 123456789;
    pkt.from_server = true;
    roundTripTest(pkt);
}

TEST(packet_chunk_radius_updated) {
    ChunkRadiusUpdatedPacket pkt;
    pkt.chunk_radius = 12;
    roundTripTest(pkt);
}

TEST(packet_request_chunk_radius) {
    RequestChunkRadiusPacket pkt;
    pkt.chunk_radius = 16;
    pkt.max_chunk_radius = 32;
    roundTripTest(pkt);
}

// World packets
TEST(packet_update_block) {
    UpdateBlockPacket pkt;
    pkt.position = {10, 64, 20};
    pkt.block_runtime_id = 100;
    pkt.flags = 3;
    pkt.layer = 0;
    roundTripTest(pkt);
}

TEST(packet_block_event) {
    BlockEventPacket pkt;
    pkt.position = {-5, 32, 15};
    pkt.event_type = 1;
    pkt.event_data = 0;
    roundTripTest(pkt);
}

TEST(packet_level_event) {
    LevelEventPacket pkt;
    pkt.event_id = 2001; // Block break particles
    pkt.position = {1.0f, 64.0f, 2.0f};
    pkt.data = 42;
    roundTripTest(pkt);
}

TEST(packet_level_sound_event) {
    LevelSoundEventPacket pkt;
    pkt.sound_id = 100;
    pkt.position = {5.0f, 70.0f, 10.0f};
    pkt.extra_data = 0;
    pkt.entity_type = "minecraft:player";
    pkt.is_baby_mob = false;
    pkt.is_global = false;
    roundTripTest(pkt);
}

TEST(packet_spawn_particle_effect) {
    SpawnParticleEffectPacket pkt;
    pkt.dimension_id = 0;
    pkt.entity_id = {-1};
    pkt.position = {10.0f, 65.0f, 20.0f};
    pkt.particle_name = "minecraft:campfire_smoke";
    roundTripTest(pkt);
}

// Entity packets
TEST(packet_remove_actor) {
    RemoveActorPacket pkt;
    pkt.entity_id = {123};
    roundTripTest(pkt);
}

TEST(packet_actor_event) {
    ActorEventPacket pkt;
    pkt.runtime_id = {42};
    pkt.event_id = 4;
    pkt.data = 0;
    roundTripTest(pkt);
}

TEST(packet_mob_effect) {
    MobEffectPacket pkt;
    pkt.runtime_id = {10};
    pkt.event_id = 1;
    pkt.effect_id = 10;
    pkt.amplifier = 1;
    pkt.particles = true;
    pkt.duration = 600;
    pkt.tick = 0;
    roundTripTest(pkt);
}

TEST(packet_set_actor_motion) {
    SetActorMotionPacket pkt;
    pkt.runtime_id = {5};
    pkt.motion = {0.0f, 0.4f, 0.0f};
    pkt.tick = 0;
    roundTripTest(pkt);
}

TEST(packet_animate) {
    AnimatePacket pkt;
    pkt.action = 1;
    pkt.runtime_id = {7};
    roundTripTest(pkt);
}

// Inventory packets
TEST(packet_container_open) {
    ContainerOpenPacket pkt;
    pkt.window_id = 1;
    pkt.type = 0;
    pkt.position = {100, 64, 200};
    pkt.entity_unique_id = {-1};
    roundTripTest(pkt);
}

TEST(packet_container_close) {
    ContainerClosePacket pkt;
    pkt.window_id = 1;
    pkt.server_initiated = true;
    roundTripTest(pkt);
}

TEST(packet_anvil_damage) {
    AnvilDamagePacket pkt;
    pkt.damage = 2;
    pkt.position = {10, 70, 20};
    roundTripTest(pkt);
}

// Command packets
TEST(packet_command_request) {
    CommandRequestPacket pkt;
    pkt.command = "/gamemode creative";
    pkt.command_origin_type = 0;
    pkt.command_origin_uuid = {};
    pkt.request_id = "test-id";
    roundTripTest(pkt);
}

TEST(packet_settings_command) {
    SettingsCommandPacket pkt;
    pkt.command = "/time set day";
    pkt.suppress_output = false;
    roundTripTest(pkt);
}

TEST(packet_script_message) {
    ScriptMessagePacket pkt;
    pkt.message_id = "mymod:hello";
    pkt.message_value = "{\"key\":\"value\"}";
    roundTripTest(pkt);
}

// Resource pack packets
TEST(packet_resource_pack_data_info) {
    ResourcePackDataInfoPacket pkt;
    pkt.resource_name = "test-pack-id";
    pkt.chunk_size = 1048576;
    pkt.chunk_count = 10;
    pkt.file_size = 10485760;
    pkt.file_hash = "abc123";
    pkt.is_premium = false;
    pkt.pack_type = 1;
    roundTripTest(pkt);
}

TEST(packet_resource_pack_chunk_request) {
    ResourcePackChunkRequestPacket pkt;
    pkt.resource_name = "test-pack-id";
    pkt.chunk_index = 3;
    roundTripTest(pkt);
}

// Camera packets
TEST(packet_camera_shake) {
    CameraShakePacket pkt;
    pkt.intensity = 0.5f;
    pkt.seconds = 2.0f;
    pkt.shake_type = 0;
    pkt.shake_action = 0;
    roundTripTest(pkt);
}

// UI packets
TEST(packet_modal_form_request) {
    ModalFormRequestPacket pkt;
    pkt.form_id = 42;
    pkt.form_json = R"({"type":"form","title":"Test","content":"Hello"})";
    roundTripTest(pkt);
}

TEST(packet_modal_form_response) {
    ModalFormResponsePacket pkt;
    pkt.form_id = 42;
    pkt.has_response_data = true;
    pkt.response_json = "0";
    pkt.has_cancel_reason = false;
    roundTripTest(pkt);
}

TEST(packet_emote) {
    EmotePacket pkt;
    pkt.runtime_id = {1};
    pkt.emote_id = "emote_1";
    pkt.xuid = "12345";
    pkt.platform_id = "test";
    pkt.flags = 0;
    roundTripTest(pkt);
}

TEST(packet_set_hud) {
    SetHudPacket pkt;
    pkt.hud_elements = {0, 1, 2};
    pkt.visibility = 0;
    roundTripTest(pkt);
}

// Sound packets
TEST(packet_play_sound) {
    PlaySoundPacket pkt;
    pkt.sound_name = "mob.zombie.say";
    pkt.position = {10, 64, 20};
    pkt.volume = 1.0f;
    pkt.pitch = 1.0f;
    roundTripTest(pkt);
}

TEST(packet_stop_sound) {
    StopSoundPacket pkt;
    pkt.sound_name = "mob.zombie.say";
    pkt.stop_all = false;
    roundTripTest(pkt);
}

// Movement packets
TEST(packet_move_player) {
    MovePlayerPacket pkt;
    pkt.runtime_id = {1};
    pkt.position = {100.0f, 65.0f, 200.0f};
    pkt.rotation = {0.0f, 90.0f};
    pkt.head_yaw = 90.0f;
    pkt.mode = MovePlayerMode::Reset;
    pkt.on_ground = true;
    pkt.riding_runtime_id = {0};
    pkt.tick = 100;
    roundTripTest(pkt);
}

TEST(packet_player_action) {
    PlayerActionPacket pkt;
    pkt.runtime_id = {1};
    pkt.action = PlayerActionType::StartBreak;
    pkt.block_position = {10, 64, 20};
    pkt.result_position = {10, 64, 20};
    pkt.face = 1;
    roundTripTest(pkt);
}

// Misc packets
TEST(packet_client_cache_status) {
    ClientCacheStatusPacket pkt;
    pkt.cache_supported = true;
    roundTripTest(pkt);
}

TEST(packet_simulation_type) {
    SimulationTypePacket pkt;
    pkt.simulation_type = 1; // Editor
    roundTripTest(pkt);
}

TEST(packet_debug_info) {
    DebugInfoPacket pkt;
    pkt.entity_id = {42};
    pkt.data = "{\"debug_key\": \"debug_value\"}";
    roundTripTest(pkt);
}

TEST(packet_player_fog) {
    PlayerFogPacket pkt;
    pkt.fog_stack = {"minecraft:fog_default", "minecraft:fog_the_end"};
    roundTripTest(pkt);
}

TEST(packet_education_settings) {
    EducationSettingsPacket pkt;
    pkt.code_builder_uri = "https://example.com/codebuilder";
    pkt.code_builder_title = "Code Builder";
    pkt.can_resize_code_builder = true;
    pkt.has_override_uri = false;
    pkt.has_quiz = false;
    roundTripTest(pkt);
}

TEST(packet_open_sign) {
    OpenSignPacket pkt;
    pkt.position = {10, 64, 20};
    pkt.front_side = true;
    roundTripTest(pkt);
}

TEST(packet_serverbound_loading_screen) {
    ServerboundLoadingScreenPacket pkt;
    pkt.type = 1;
    pkt.loading_screen_id = 42u;
    roundTripTest(pkt);
}

// =============================================================================
// SECTION: Remaining Packet Round-Trip Tests (comprehensive coverage)
// All 119 packet classes must have round-trip tests
// =============================================================================
SECTION("Remaining Packet Round-Trip Tests");

// --- Camera Packets ---
TEST(packet_camera) {
    CameraPacket pkt;
    pkt.camera_entity_id = {5};
    pkt.target_player_id = {1};
    roundTripTest(pkt);
}

TEST(packet_camera_aim_assist_presets) {
    CameraAimAssistPresetsPacket pkt;
    // Empty presets data
    roundTripTest(pkt);
}

TEST(packet_camera_presets) {
    CameraPresetsPacket pkt;
    // Raw NBT data - empty compound
    BinaryStream nbt_s;
    auto root = NbtTag::makeCompound();
    NbtTag::writeNetworkNbt(nbt_s, *root);
    auto& buf = nbt_s.getBuffer();
    pkt.presets_data.assign(buf.begin(), buf.end());
    roundTripTest(pkt);
}

TEST(packet_camera_instruction) {
    CameraInstructionPacket pkt;
    // Uses raw bytes
    roundTripTest(pkt);
}

TEST(packet_camera_aim_assist) {
    CameraAimAssistPacket pkt;
    // Uses raw data
    roundTripTest(pkt);
}

TEST(packet_client_camera_aim_assist) {
    ClientCameraAimAssistPacket pkt;
    roundTripTest(pkt);
}

TEST(packet_camera_aim_assist_actor_priority) {
    CameraAimAssistActorPriorityPacket pkt;
    roundTripTest(pkt);
}

TEST(packet_camera_spline) {
    CameraSplinePacket pkt;
    roundTripTest(pkt);
}

// --- Command Packets ---
TEST(packet_command_output) {
    CommandOutputPacket pkt;
    pkt.command_origin_type = 0;
    pkt.command_origin_uuid = {};
    pkt.request_id = "req-1";
    pkt.success_count = 1;
    pkt.output_type = 0;
    roundTripTest(pkt);
}

TEST(packet_remove_objective) {
    RemoveObjectivePacket pkt;
    pkt.objective_name = "test_obj";
    roundTripTest(pkt);
}

TEST(packet_set_display_objective) {
    SetDisplayObjectivePacket pkt;
    pkt.display_slot = "sidebar";
    pkt.objective_name = "test_obj";
    pkt.display_name = "Test";
    pkt.criteria_name = "dummy";
    pkt.sort_order = 0;
    roundTripTest(pkt);
}

TEST(packet_update_soft_enum) {
    UpdateSoftEnumPacket pkt;
    pkt.enum_name = "myEnum";
    pkt.values = {"val1", "val2"};
    pkt.update_type = SoftEnumUpdateType::Add;
    roundTripTest(pkt);
}

TEST(packet_available_commands) {
    AvailableCommandsPacket pkt;
    // Uses raw data - empty commands
    roundTripTest(pkt);
}

// --- Entity Packets ---
TEST(packet_interact) {
    InteractPacket pkt;
    pkt.action = 4; // OpenInventory
    pkt.target_runtime_id = {0};
    roundTripTest(pkt);
}

TEST(packet_take_item_actor) {
    TakeItemActorPacket pkt;
    pkt.item_runtime_id = {10};
    pkt.player_runtime_id = {1};
    roundTripTest(pkt);
}

TEST(packet_hurt_armor) {
    HurtArmorPacket pkt;
    pkt.cause = 1;
    pkt.damage = 5;
    pkt.armor_slots = 0xF;
    roundTripTest(pkt);
}

TEST(packet_player_armor_damage) {
    PlayerArmorDamagePacket pkt;
    pkt.damage_bits = 0;
    roundTripTest(pkt);
}

TEST(packet_animate_entity) {
    AnimateEntityPacket pkt;
    pkt.animation = "animation.creeper.swelling";
    pkt.next_state = "default";
    pkt.stop_expression = "query.any_animation_finished";
    pkt.stop_expression_version = 0;
    pkt.controller = "";
    pkt.blend_out_time = 0.0f;
    pkt.runtime_ids = {{1}, {2}};
    roundTripTest(pkt);
}

TEST(packet_sync_actor_property) {
    SyncActorPropertyPacket pkt;
    pkt.property_data = NbtTag::makeCompound();
    roundTripTest(pkt);
}

TEST(packet_set_last_hurt_by) {
    SetLastHurtByPacket pkt;
    pkt.entity_type_id = 1;
    roundTripTest(pkt);
}

TEST(packet_add_painting) {
    AddPaintingPacket pkt;
    pkt.entity_id = {100};
    pkt.runtime_id = {100};
    pkt.position = {10.0f, 64.0f, 20.0f};
    pkt.direction = 2;
    pkt.motive = "Kebab";
    roundTripTest(pkt);
}

TEST(packet_change_mob_property) {
    ChangeMobPropertyPacket pkt;
    pkt.entity_unique_id = {1};
    pkt.property = "minecraft:test";
    pkt.bool_value = true;
    pkt.string_value = "";
    pkt.int_value = 0;
    pkt.float_value = 0.0f;
    roundTripTest(pkt);
}

TEST(packet_add_volume_entity) {
    AddVolumeEntityPacket pkt;
    pkt.entity_net_id = 1;
    pkt.dimension_type = 0;
    pkt.engine_version = "1.20.0";
    pkt.json_identifier = "";
    pkt.instance_name = "";
    pkt.min_bounds = {0, 0, 0};
    pkt.max_bounds = {16, 16, 16};
    // Note: read() uses readRemainingBytes() for components_nbt,
    // so round-trip won't work. Test serialize only.
    BinaryStream ws;
    pkt.write(ws);
    ASSERT_TRUE(ws.getBuffer().size() > 0);
}

TEST(packet_set_actor_link) {
    SetActorLinkPacket pkt;
    pkt.ridden_id = {1};
    pkt.rider_id = {2};
    pkt.type = 0;
    pkt.immediate = false;
    pkt.rider_initiated = false;
    pkt.vehicle_angular_velocity = 0.0f;
    roundTripTest(pkt);
}

// --- Inventory Packets ---
TEST(packet_container_set_data) {
    ContainerSetDataPacket pkt;
    pkt.window_id = 0;
    pkt.property = 1;
    pkt.value = 200;
    roundTripTest(pkt);
}

TEST(packet_player_hotbar) {
    PlayerHotbarPacket pkt;
    pkt.selected_hotbar_slot = 3;
    pkt.window_id = 0;
    pkt.select_hotbar_slot = true;
    roundTripTest(pkt);
}

TEST(packet_gui_data_pick_item) {
    GuiDataPickItemPacket pkt;
    pkt.item_description = "Stone";
    pkt.item_effects = "";
    pkt.hotbar_slot = 0;
    roundTripTest(pkt);
}

TEST(packet_container_registry_cleanup) {
    ContainerRegistryCleanupPacket pkt;
    pkt.removed_containers = {0, 1, 2};
    roundTripTest(pkt);
}

TEST(packet_player_toggle_crafter_slot_request) {
    PlayerToggleCrafterSlotRequestPacket pkt;
    pkt.x = 10;
    pkt.y = 64;
    pkt.z = 20;
    pkt.slot = 3;
    pkt.disabled = true;
    roundTripTest(pkt);
}

TEST(packet_set_player_inventory_options) {
    SetPlayerInventoryOptionsPacket pkt;
    pkt.left_tab = 0;
    pkt.right_tab = 0;
    pkt.filtering = false;
    pkt.layout = 0;
    pkt.crafting_layout = 0;
    roundTripTest(pkt);
}

TEST(packet_completed_using_item) {
    CompletedUsingItemPacket pkt;
    pkt.item_id = 1;
    pkt.use_method = 2;
    roundTripTest(pkt);
}

TEST(packet_player_start_item_cooldown) {
    PlayerStartItemCooldownPacket pkt;
    pkt.item_category = "ender_pearl";
    pkt.cooldown_ticks = 20;
    roundTripTest(pkt);
}

TEST(packet_update_equip) {
    UpdateEquipPacket pkt;
    pkt.container_id = 1;
    pkt.type = 0;
    pkt.size = 0;
    pkt.entity_unique_id = {1};
    // NBT data - empty compound
    BinaryStream nbt_s;
    auto root = NbtTag::makeCompound();
    NbtTag::writeNetworkNbt(nbt_s, *root);
    auto& buf = nbt_s.getBuffer();
    pkt.nbt_data.assign(buf.begin(), buf.end());
    roundTripTest(pkt);
}

TEST(packet_update_trade) {
    UpdateTradePacket pkt;
    pkt.container_id = 1;
    pkt.type = 15; // Trading
    pkt.size = 0;
    pkt.trader_tier = 1;
    pkt.entity_unique_id = {5};
    pkt.last_trading_player = {1};
    pkt.display_name = "Villager";
    pkt.using_economy_trade = true;
    // NBT data - empty compound
    BinaryStream nbt_s;
    auto root = NbtTag::makeCompound();
    NbtTag::writeNetworkNbt(nbt_s, *root);
    auto& buf = nbt_s.getBuffer();
    pkt.nbt_data.assign(buf.begin(), buf.end());
    roundTripTest(pkt);
}

// --- Login Packets ---
TEST(packet_server_to_client_handshake) {
    ServerToClientHandshakePacket pkt;
    pkt.handshake_jwt = "eyJhbGciOiJFUzM4NCIsIng1dSI6IiJ9.test.signature";
    roundTripTest(pkt);
}

TEST(packet_client_to_server_handshake) {
    ClientToServerHandshakePacket pkt;
    roundTripTest(pkt);
}

// --- Movement Packets ---
TEST(packet_move_actor_absolute) {
    MoveActorAbsolutePacket pkt;
    pkt.runtime_id = {5};
    pkt.flags = 1; // onGround
    pkt.position = {100.0f, 64.0f, 200.0f};
    pkt.rotation_x = 0.0f;
    pkt.rotation_y = 90.0f;
    pkt.rotation_z = 0.0f;
    roundTripTest(pkt);
}

TEST(packet_correct_player_move_prediction) {
    CorrectPlayerMovePredictionPacket pkt;
    pkt.position = {100.0f, 64.0f, 200.0f};
    pkt.delta = {0.0f, 0.0f, 0.0f};
    pkt.on_ground = true;
    pkt.tick = 500;
    pkt.prediction_type = 0; // Player
    roundTripTest(pkt);
}

TEST(packet_motion_prediction_hints) {
    MotionPredictionHintsPacket pkt;
    pkt.entity_runtime_id = {1};
    pkt.velocity = {0.5f, 1.0f, -0.3f};
    pkt.on_ground = true;
    roundTripTest(pkt);
}

TEST(packet_server_player_post_move_position) {
    ServerPlayerPostMovePositionPacket pkt;
    pkt.position = {10.0f, 65.0f, 20.0f};
    roundTripTest(pkt);
}

TEST(packet_movement_effect) {
    MovementEffectPacket pkt;
    pkt.entity_runtime_id = {1};
    pkt.effect_type = 0;
    pkt.duration = 100;
    pkt.tick = 500;
    roundTripTest(pkt);
}

// --- Play Packets ---
TEST(packet_set_player_game_type) {
    SetPlayerGameTypePacket pkt;
    pkt.game_type = 1; // Creative
    roundTripTest(pkt);
}

TEST(packet_set_default_game_type) {
    SetDefaultGameTypePacket pkt;
    pkt.game_type = 0; // Survival
    roundTripTest(pkt);
}

TEST(packet_game_rules_changed) {
    GameRulesChangedPacket pkt;
    GameRule r1;
    r1.name = "pvp";
    r1.type = 1;
    r1.bool_value = true;
    r1.editable = false;
    GameRule r2;
    r2.name = "commandblockoutput";
    r2.type = 1;
    r2.bool_value = false;
    r2.editable = false;
    pkt.rules = {r1, r2};
    roundTripTest(pkt);
}

TEST(packet_simple_event) {
    SimpleEventPacket pkt;
    pkt.event_type = 1; // CommandsEnabled
    roundTripTest(pkt);
}

TEST(packet_show_credits) {
    ShowCreditsPacket pkt;
    pkt.player_id = {1};
    pkt.status = 0;
    roundTripTest(pkt);
}

TEST(packet_update_player_game_type) {
    UpdatePlayerGameTypePacket pkt;
    pkt.game_type = 1;
    pkt.player_id = {1};
    pkt.tick = 100;
    roundTripTest(pkt);
}

TEST(packet_set_local_player_as_initialized) {
    SetLocalPlayerAsInitializedPacket pkt;
    pkt.runtime_entity_id = {1};
    roundTripTest(pkt);
}

TEST(packet_network_stack_latency_roundtrip) {
    NetworkStackLatencyPacket pkt;
    pkt.timestamp = 123456789;
    pkt.from_server = true;
    roundTripTest(pkt);
}

TEST(packet_packet_violation_warning) {
    PacketViolationWarningPacket pkt;
    pkt.violation_type = 0;
    pkt.severity = 1;
    pkt.packet_id_cause = 44;
    pkt.context = "test violation";
    roundTripTest(pkt);
}

// --- World Packets ---
TEST(packet_update_block_synced) {
    UpdateBlockSyncedPacket pkt;
    pkt.position = {10, 64, 20};
    pkt.block_runtime_id = 100;
    pkt.flags = 3;
    pkt.layer = 0;
    pkt.entity_unique_id = {1};
    pkt.update_type = 0;
    roundTripTest(pkt);
}

TEST(packet_block_actor_data) {
    BlockActorDataPacket pkt;
    pkt.position = {10, 64, 20};
    // NBT data
    pkt.nbt_data = NbtTag::makeCompound();
    pkt.nbt_data->compoundVal["id"] = NbtTag::makeString("Sign");
    roundTripTest(pkt);
}

TEST(packet_level_event_generic) {
    LevelEventGenericPacket pkt;
    pkt.event_id = 2000;
    pkt.event_data = NbtTag::makeCompound();
    roundTripTest(pkt);
}

TEST(packet_spawn_experience_orb) {
    SpawnExperienceOrbPacket pkt;
    pkt.position = {10.0f, 64.0f, 20.0f};
    pkt.count = 7;
    roundTripTest(pkt);
}

TEST(packet_update_sub_chunk_blocks) {
    UpdateSubChunkBlocksPacket pkt;
    pkt.sub_chunk_pos = {0, 0, 0};
    // Empty block updates
    roundTripTest(pkt);
}

TEST(packet_on_screen_texture_animation) {
    OnScreenTextureAnimationPacket pkt;
    pkt.animation_type = 1;
    roundTripTest(pkt);
}

// --- UI Packets ---
TEST(packet_server_settings_request) {
    ServerSettingsRequestPacket pkt;
    roundTripTest(pkt);
}

TEST(packet_server_settings_response) {
    ServerSettingsResponsePacket pkt;
    pkt.form_id = 10;
    pkt.form_json = R"({"type":"custom_form","title":"Settings","content":[]})";
    roundTripTest(pkt);
}

TEST(packet_clientbound_close_form) {
    ClientboundCloseFormPacket pkt;
    roundTripTest(pkt);
}

TEST(packet_emote_list) {
    EmoteListPacket pkt;
    pkt.player_id = {1};
    // Empty emote list
    roundTripTest(pkt);
}

TEST(packet_npc_request) {
    NpcRequestPacket pkt;
    pkt.entity_runtime_id = {5};
    pkt.request_type = 0;
    pkt.command = "test";
    pkt.action_type = 0;
    pkt.scene_name = "default";
    roundTripTest(pkt);
}

TEST(packet_npc_dialogue) {
    NpcDialoguePacket pkt;
    pkt.entity_unique_id = 5;
    pkt.action_type = 0;
    pkt.dialogue = "Hello traveler!";
    pkt.scene_name = "greeting";
    pkt.npc_name = "Villager";
    pkt.action_json = "[]";
    roundTripTest(pkt);
}

TEST(packet_show_store_offer) {
    ShowStoreOfferPacket pkt;
    pkt.offer_id = "offer-123";
    pkt.show_type = 0;
    roundTripTest(pkt);
}

TEST(packet_show_profile) {
    ShowProfilePacket pkt;
    pkt.xuid = "12345678";
    roundTripTest(pkt);
}

TEST(packet_player_skin) {
    PlayerSkinPacket pkt;
    pkt.player_uuid = {};
    // Minimal skin data - just round trip the raw bytes
    roundTripTest(pkt);
}

TEST(packet_book_edit) {
    BookEditPacket pkt;
    pkt.action = 0; // ReplacePage
    pkt.inventory_slot = 0;
    roundTripTest(pkt);
}

TEST(packet_award_achievement) {
    AwardAchievementPacket pkt;
    pkt.achievement_id = 42;
    roundTripTest(pkt);
}

TEST(packet_clientbound_data_driven_ui_show_screen) {
    ClientboundDataDrivenUIShowScreenPacket pkt;
    pkt.screen_name = "test_screen";
    pkt.screen_data = "{}";
    roundTripTest(pkt);
}

TEST(packet_clientbound_data_driven_ui_reload) {
    ClientboundDataDrivenUIReloadPacket pkt;
    roundTripTest(pkt);
}

TEST(packet_clientbound_data_driven_ui_close_screen) {
    ClientboundDataDrivenUICloseScreenPacket pkt;
    pkt.form_id = 0;
    roundTripTest(pkt);
}

// --- Misc Packets ---
TEST(packet_actor_pick_request) {
    ActorPickRequestPacket pkt;
    pkt.entity_id = 42;
    pkt.hotbar_slot = 1;
    pkt.with_data = false;
    roundTripTest(pkt);
}

TEST(packet_block_pick_request_roundtrip) {
    BlockPickRequestPacket pkt;
    pkt.position = {10, 64, 20};
    pkt.add_user_data = true;
    pkt.hotbar_slot = 1;
    roundTripTest(pkt);
}

TEST(packet_map_info_request) {
    MapInfoRequestPacket pkt;
    pkt.map_id = {100};
    roundTripTest(pkt);
}

TEST(packet_map_create_locked_copy) {
    MapCreateLockedCopyPacket pkt;
    pkt.original_map_id = {100};
    pkt.new_map_id = {101};
    roundTripTest(pkt);
}

TEST(packet_lectern_update) {
    LecternUpdatePacket pkt;
    pkt.page = 2;
    pkt.total_pages = 10;
    pkt.position = {10, 64, 20};
    roundTripTest(pkt);
}

TEST(packet_multiplayer_settings) {
    MultiplayerSettingsPacket pkt;
    pkt.action_type = 0;
    roundTripTest(pkt);
}

TEST(packet_photo_transfer) {
    PhotoTransferPacket pkt;
    pkt.photo_name = "photo1";
    pkt.photo_data = "raw_image_data";
    pkt.book_id = "book-1";
    pkt.photo_type = 0;
    pkt.source_type = 0;
    pkt.owner_entity_unique_id = {1};
    pkt.new_photo_name = "";
    roundTripTest(pkt);
}

TEST(packet_purchase_receipt) {
    PurchaseReceiptPacket pkt;
    pkt.receipt_ids = {"receipt-1", "receipt-2"};
    roundTripTest(pkt);
}

TEST(packet_automation_client_connect) {
    AutomationClientConnectPacket pkt;
    pkt.server_uri = "ws://localhost:8080";
    roundTripTest(pkt);
}

TEST(packet_lab_table) {
    LabTablePacket pkt;
    pkt.action_type = 0;
    pkt.position = {10, 64, 20};
    pkt.reaction_type = 0;
    roundTripTest(pkt);
}

TEST(packet_add_behavior_tree) {
    AddBehaviorTreePacket pkt;
    pkt.behavior_tree_json = R"({"format_version":"1.16.0"})";
    roundTripTest(pkt);
}

TEST(packet_edu_uri_resource) {
    EduUriResourcePacket pkt;
    pkt.link_uri = "https://education.minecraft.net";
    pkt.button_name = "Learn More";
    roundTripTest(pkt);
}

TEST(packet_create_photo) {
    CreatePhotoPacket pkt;
    pkt.entity_unique_id = {1};
    pkt.photo_name = "screenshot";
    pkt.item_name = "Photo";
    roundTripTest(pkt);
}

TEST(packet_lesson_progress) {
    LessonProgressPacket pkt;
    pkt.action = 0;
    pkt.score = 100;
    pkt.activity_id = "lesson-1";
    roundTripTest(pkt);
}

TEST(packet_code_builder) {
    CodeBuilderPacket pkt;
    pkt.url = "https://codebuilder.example.com";
    pkt.should_open = true;
    roundTripTest(pkt);
}

TEST(packet_code_builder_source) {
    CodeBuilderSourcePacket pkt;
    pkt.operation = 0;
    pkt.category = 0;
    pkt.source_code = "";
    roundTripTest(pkt);
}

TEST(packet_ticking_areas_load_status) {
    TickingAreasLoadStatusPacket pkt;
    pkt.waiting_for_preload = false;
    roundTripTest(pkt);
}

TEST(packet_serverbound_diagnostics) {
    ServerboundDiagnosticsPacket pkt;
    pkt.avg_fps = 60.0f;
    pkt.avg_server_sim_tick_time = 50.0f;
    pkt.avg_client_sim_tick_time = 16.0f;
    pkt.avg_begin_frame_time = 1.0f;
    pkt.avg_input_time = 0.5f;
    pkt.avg_render_time = 10.0f;
    pkt.avg_end_frame_time = 1.0f;
    pkt.avg_remaining_time_percent = 5.0f;
    pkt.avg_unaccounted_time_percent = 2.0f;
    roundTripTest(pkt);
}

TEST(packet_refresh_entitlements) {
    RefreshEntitlementsPacket pkt;
    roundTripTest(pkt);
}

TEST(packet_server_stats) {
    ServerStatsPacket pkt;
    pkt.server_time = 1000.0f;
    pkt.network_time = 50.0f;
    roundTripTest(pkt);
}

TEST(packet_update_client_input_locks) {
    UpdateClientInputLocksPacket pkt;
    pkt.lock_component_data = 0;
    pkt.server_position = {0.0f, 64.0f, 0.0f};
    roundTripTest(pkt);
}

TEST(packet_update_client_options) {
    UpdateClientOptionsPacket pkt;
    // Raw data
    roundTripTest(pkt);
}

TEST(packet_clientbound_control_scheme_set) {
    ClientboundControlSchemeSetPacket pkt;
    pkt.control_scheme = 0;
    roundTripTest(pkt);
}

TEST(packet_editor_network) {
    EditorNetworkPacket pkt;
    pkt.editor_data = NbtTag::makeCompound();
    roundTripTest(pkt);
}

TEST(packet_unlocked_recipes) {
    UnlockedRecipesPacket pkt;
    pkt.unlock_type = 0;
    pkt.recipes = {"minecraft:crafting_table", "minecraft:wooden_pickaxe"};
    roundTripTest(pkt);
}

TEST(packet_player_video_capture) {
    PlayerVideoCapturePacket pkt;
    // Raw data
    roundTripTest(pkt);
}

TEST(packet_clientbound_texture_shift) {
    ClientboundTextureShiftPacket pkt;
    // Raw data
    roundTripTest(pkt);
}

TEST(packet_clientbound_data_store) {
    ClientboundDataStorePacket pkt;
    // Raw data
    roundTripTest(pkt);
}

TEST(packet_serverbound_data_store) {
    ServerboundDataStorePacket pkt;
    // Raw data
    roundTripTest(pkt);
}

TEST(packet_clientbound_debug_renderer) {
    ClientboundDebugRendererPacket pkt;
    pkt.type = ClientboundDebugRendererPacket::Type::ClearAllDebugRenderers;
    roundTripTest(pkt);
}

TEST(packet_current_structure_feature) {
    CurrentStructureFeaturePacket pkt;
    pkt.current_structure_feature = "minecraft:village";
    roundTripTest(pkt);
}

TEST(packet_debug_drawer) {
    DebugDrawerPacket pkt;
    // Raw data
    roundTripTest(pkt);
}

TEST(packet_feature_registry) {
    FeatureRegistryPacket pkt;
    // Empty feature list
    roundTripTest(pkt);
}

TEST(packet_game_test_request) {
    GameTestRequestPacket pkt;
    pkt.max_tests_per_batch = 10;
    pkt.repeat_count = 1;
    pkt.rotation = 0;
    pkt.stop_on_failure = true;
    pkt.test_pos = {0, 64, 0};
    pkt.tests_per_row = 5;
    pkt.test_name = "test_example";
    roundTripTest(pkt);
}

TEST(packet_game_test_results) {
    GameTestResultsPacket pkt;
    pkt.succeeded = true;
    pkt.error = "";
    pkt.test_name = "test_example";
    roundTripTest(pkt);
}

TEST(packet_voxel_shapes) {
    VoxelShapesPacket pkt;
    // Raw data
    roundTripTest(pkt);
}

// --- Resource Pack Packets ---
TEST(packet_resource_pack_chunk_data) {
    ResourcePackChunkDataPacket pkt;
    pkt.resource_name = "test-pack";
    pkt.chunk_index = 0;
    pkt.byte_offset = 0;
    pkt.chunk_data = {0x00, 0x01, 0x02, 0x03};
    roundTripTest(pkt);
}

TEST(packet_resource_packs_ready_for_validation) {
    ResourcePacksReadyForValidationPacket pkt;
    // Empty pack info
    roundTripTest(pkt);
}

// --- Packet ID Coverage Verification ---
TEST(packet_id_uniqueness) {
    // Verify all packet IDs in enum are unique
    std::set<uint32_t> seen_ids;
    auto check = [&](PacketId id) {
        uint32_t val = static_cast<uint32_t>(id);
        if (seen_ids.count(val)) {
            throw std::runtime_error("Duplicate packet ID: " + std::to_string(val));
        }
        seen_ids.insert(val);
    };
    // Spot check some important IDs
    check(PacketId::Login);
    check(PacketId::PlayStatus);
    check(PacketId::Disconnect);
    check(PacketId::Text);
    check(PacketId::MovePlayer);
    check(PacketId::PlayerAction);
    check(PacketId::PlayerAuthInput);
    check(PacketId::FullChunkData);
    check(PacketId::NetworkChunkPublisherUpdate);
    check(PacketId::LevelSoundEvent);
    check(PacketId::ContainerOpen);
    check(PacketId::ContainerClose);
    ASSERT_EQ(seen_ids.size(), 12u);
}

TEST(packet_id_to_string_all_known) {
    // Verify all implemented packet IDs have names
    ASSERT_TRUE(std::string(packetIdToString(PacketId::RequestNetworkSettings)) != "Unknown");
    ASSERT_TRUE(std::string(packetIdToString(PacketId::NetworkSettings)) != "Unknown");
    ASSERT_TRUE(std::string(packetIdToString(PacketId::Login)) != "Unknown");
    ASSERT_TRUE(std::string(packetIdToString(PacketId::StartGame)) != "Unknown");
    ASSERT_TRUE(std::string(packetIdToString(PacketId::MovePlayer)) != "Unknown");
    ASSERT_TRUE(std::string(packetIdToString(PacketId::UpdateBlock)) != "Unknown");
    ASSERT_TRUE(std::string(packetIdToString(PacketId::LevelSoundEvent)) != "Unknown");
    ASSERT_TRUE(std::string(packetIdToString(PacketId::ContainerOpen)) != "Unknown");
    ASSERT_TRUE(std::string(packetIdToString(PacketId::UpdateAbilities)) != "Unknown");
    ASSERT_TRUE(std::string(packetIdToString(PacketId::Ping)) != "Unknown");
}

// Full serialize+deserialize with header (not just payload)
TEST(packet_full_serialize_deserialize) {
    TextPacket original;
    original.type = TextType::Chat;
    original.source_name = "TestPlayer";
    original.message = "Hello World!";
    original.needs_translation = false;
    original.xuid = "";
    original.platform_chat_id = "";

    // Full serialize (header + payload)
    std::string data = original.serialize();
    ASSERT_TRUE(data.size() > 4);

    // Full deserialize
    TextPacket decoded;
    bool ok = decoded.deserialize(
        reinterpret_cast<const uint8_t*>(data.data()), data.size());
    ASSERT_TRUE(ok);
    ASSERT_EQ(static_cast<int>(decoded.type), 1);
    ASSERT_EQ(decoded.source_name, "TestPlayer");
    ASSERT_EQ(decoded.message, "Hello World!");
}

// =============================================================================
// SECTION: PacketCodec Tests
// =============================================================================
SECTION("PacketCodec Tests");

TEST(packet_codec_single) {
    TextPacket original;
    original.type = TextType::Chat;
    original.needs_translation = false;
    original.source_name = "Server";
    original.message = "Test message";
    original.xuid = "";
    original.platform_chat_id = "";
    original.filtered_message = "";
    
    // Encode
    auto encoded = PacketCodec::encodeSingle(original);
    ASSERT_TRUE(encoded.size() > 0);
    
    // Decode and verify packet ID
    bool found = false;
    PacketCodec::decodeBatch(
        reinterpret_cast<const uint8_t*>(encoded.data()), encoded.size(),
        [&found](const PacketCodec::DecodedSubPacket& sub) {
            ASSERT_EQ(static_cast<int>(sub.header.packet_id),
                      static_cast<int>(PacketId::Text));
            found = true;
        });
    ASSERT_TRUE(found);
}

TEST(packet_codec_batch) {
    TextPacket text;
    text.type = TextType::Raw;
    text.needs_translation = false;
    text.source_name = "";
    text.message = "Raw message";
    text.xuid = "";
    text.platform_chat_id = "";
    text.filtered_message = "";
    
    SetTimePacket time;
    time.time = 6000;
    
    std::vector<const Packet*> packets = {&text, &time};
    auto batch = PacketCodec::encodeBatch(packets);
    ASSERT_TRUE(batch.size() > 0);
    
    // Decode back
    int count = 0;
    PacketCodec::decodeBatch(
        reinterpret_cast<const uint8_t*>(batch.data()), batch.size(),
        [&count](const PacketCodec::DecodedSubPacket& sub) {
            PacketId id = static_cast<PacketId>(sub.header.packet_id);
            if (count == 0) {
                ASSERT_EQ(static_cast<int>(id), static_cast<int>(PacketId::Text));
            } else if (count == 1) {
                ASSERT_EQ(static_cast<int>(id), static_cast<int>(PacketId::SetTime));
            }
            count++;
        });
    ASSERT_EQ(count, 2);
}

// =============================================================================
// SECTION: RakNet Tests
// =============================================================================
SECTION("RakNet Tests");

TEST(raknet_bitstream_basic) {
    raknet::BitStream bs;
    bs.writeUint8(0x42);
    bs.writeUint16BE(0x1234);
    bs.writeUint24LE(0x123456);
    bs.writeUint32BE(0xDEADBEEF);
    
    bs.resetRead();
    ASSERT_EQ(bs.readUint8(), 0x42);
    ASSERT_EQ(bs.readUint16BE(), 0x1234);
    ASSERT_EQ(bs.readUint24LE(), 0x123456u);
    ASSERT_EQ(bs.readUint32BE(), 0xDEADBEEFu);
}

TEST(raknet_bitstream_bits) {
    raknet::BitStream bs;
    bs.writeBit(true);
    bs.writeBit(false);
    bs.writeBit(true);
    
    bs.resetRead();
    ASSERT_EQ(bs.readBit(), true);
    ASSERT_EQ(bs.readBit(), false);
    ASSERT_EQ(bs.readBit(), true);
}

TEST(raknet_bitstream_string) {
    raknet::BitStream bs;
    bs.writeString("Hello RakNet");
    
    bs.resetRead();
    ASSERT_EQ(bs.readString(), "Hello RakNet");
}

TEST(raknet_magic_validation) {
    ASSERT_TRUE(raknet::validateMagic(raknet::OFFLINE_MESSAGE_MAGIC));
    
    uint8_t bad_magic[16] = {};
    ASSERT_TRUE(!raknet::validateMagic(bad_magic));
}

TEST(raknet_uint24le) {
    uint8_t buf[3];
    raknet::writeUint24LE(buf, 0x123456);
    ASSERT_EQ(raknet::readUint24LE(buf), 0x123456u);
    
    raknet::writeUint24LE(buf, 0);
    ASSERT_EQ(raknet::readUint24LE(buf), 0u);
    
    raknet::writeUint24LE(buf, 0xFFFFFF);
    ASSERT_EQ(raknet::readUint24LE(buf), 0xFFFFFFu);
}

TEST(raknet_frame_encode_decode) {
    raknet::Frame frame;
    frame.reliability = raknet::Reliability::ReliableOrdered;
    frame.reliable_message_index = 42;
    frame.ordering_index = 7;
    frame.ordering_channel = 0;
    frame.body = {0x01, 0x02, 0x03, 0x04, 0x05};
    
    raknet::BitStream write_bs;
    frame.encode(write_bs);
    
    write_bs.resetRead();
    raknet::Frame decoded = raknet::Frame::decode(write_bs);
    
    ASSERT_EQ(static_cast<int>(decoded.reliability),
              static_cast<int>(raknet::Reliability::ReliableOrdered));
    ASSERT_EQ(decoded.reliable_message_index, 42u);
    ASSERT_EQ(decoded.ordering_index, 7u);
    ASSERT_EQ(decoded.ordering_channel, 0);
    ASSERT_EQ(decoded.body.size(), 5u);
    ASSERT_EQ(decoded.body[0], 0x01);
    ASSERT_EQ(decoded.body[4], 0x05);
}

TEST(raknet_datagram_encode_decode) {
    raknet::Datagram dg;
    dg.flags = raknet::FlagValid;
    dg.sequence_number = 12345;
    
    raknet::Frame f1;
    f1.reliability = raknet::Reliability::Reliable;
    f1.reliable_message_index = 0;
    f1.body = {0xFE, 0x01, 0x02};
    dg.frames.push_back(f1);
    
    raknet::Frame f2;
    f2.reliability = raknet::Reliability::Unreliable;
    f2.body = {0xAA, 0xBB};
    dg.frames.push_back(f2);
    
    auto encoded = dg.encode();
    auto decoded = raknet::Datagram::decode(encoded.data(), encoded.size());
    
    ASSERT_EQ(decoded.sequence_number, 12345u);
    ASSERT_EQ(decoded.frames.size(), 2u);
    ASSERT_EQ(decoded.frames[0].body.size(), 3u);
    ASSERT_EQ(decoded.frames[0].body[0], 0xFE);
    ASSERT_EQ(decoded.frames[1].body.size(), 2u);
}

TEST(raknet_ack_encode_decode) {
    std::set<uint32_t> sequences = {1, 2, 3, 5, 6, 10};
    auto ack = raknet::AckNak::buildAck(sequences);
    ASSERT_TRUE(ack.is_ack);
    ASSERT_EQ(ack.ranges.size(), 3u); // [1-3], [5-6], [10-10]
    
    auto encoded = ack.encode();
    auto decoded = raknet::AckNak::decode(encoded.data(), encoded.size());
    
    ASSERT_TRUE(decoded.is_ack);
    auto result = decoded.getSequenceNumbers();
    ASSERT_EQ(result.size(), 6u);
    ASSERT_TRUE(result.count(1));
    ASSERT_TRUE(result.count(3));
    ASSERT_TRUE(result.count(5));
    ASSERT_TRUE(result.count(10));
}

TEST(raknet_split_assembler) {
    raknet::SplitPacketAssembler assembler;
    
    // Simulate 3-part split
    raknet::Frame f1;
    f1.is_split = true;
    f1.split_count = 3;
    f1.split_id = 1;
    f1.split_index = 0;
    f1.body = {0x01, 0x02};
    
    raknet::Frame f2;
    f2.is_split = true;
    f2.split_count = 3;
    f2.split_id = 1;
    f2.split_index = 2; // Out of order
    f2.body = {0x05, 0x06};
    
    raknet::Frame f3;
    f3.is_split = true;
    f3.split_count = 3;
    f3.split_id = 1;
    f3.split_index = 1;
    f3.body = {0x03, 0x04};
    
    auto r1 = assembler.addFragment(f1);
    ASSERT_TRUE(!r1.has_value());
    
    auto r2 = assembler.addFragment(f2);
    ASSERT_TRUE(!r2.has_value());
    
    auto r3 = assembler.addFragment(f3);
    ASSERT_TRUE(r3.has_value());
    
    auto& assembled = r3.value();
    ASSERT_EQ(assembled.size(), 6u);
    ASSERT_EQ(assembled[0], 0x01);
    ASSERT_EQ(assembled[1], 0x02);
    ASSERT_EQ(assembled[2], 0x03);
    ASSERT_EQ(assembled[3], 0x04);
    ASSERT_EQ(assembled[4], 0x05);
    ASSERT_EQ(assembled[5], 0x06);
}

TEST(raknet_ordering_system) {
    raknet::OrderingSystem ordering;
    
    // Send packets out of order
    auto r2 = ordering.processOrdered(0, 2, {0x03}); // Future, buffered
    ASSERT_EQ(r2.size(), 0u);
    
    auto r0 = ordering.processOrdered(0, 0, {0x01}); // Expected
    ASSERT_EQ(r0.size(), 1u);
    ASSERT_EQ(r0[0][0], 0x01);
    
    auto r1 = ordering.processOrdered(0, 1, {0x02}); // Delivers both 1 and buffered 2
    ASSERT_EQ(r1.size(), 2u);
    ASSERT_EQ(r1[0][0], 0x02);
    ASSERT_EQ(r1[1][0], 0x03);
}

TEST(raknet_reliability_layer) {
    raknet::ReliabilityLayer layer;
    
    // Create a reliable ordered frame
    auto frame = layer.createFrame({0x01, 0x02, 0x03}, raknet::Reliability::ReliableOrdered);
    ASSERT_EQ(frame.reliable_message_index, 0u);
    ASSERT_EQ(frame.ordering_index, 0u);
    ASSERT_EQ(frame.body.size(), 3u);
    
    // Build datagrams
    auto datagrams = layer.buildDatagrams({frame}, 1400);
    ASSERT_EQ(datagrams.size(), 1u);
    ASSERT_EQ(datagrams[0].frames.size(), 1u);
}

TEST(raknet_system_address) {
    raknet::SystemAddress addr("127.0.0.1", 19132);
    ASSERT_EQ(addr.getPort(), 19132);
    ASSERT_EQ(addr.getIP(), "127.0.0.1");
    ASSERT_EQ(addr.toString(), "127.0.0.1:19132");
    
    // Write and read back
    std::vector<uint8_t> buf;
    addr.writeTo(buf);
    ASSERT_EQ(buf.size(), 7u); // 1 (version) + 4 (IP) + 2 (port)
    
    size_t offset = 0;
    auto read_addr = raknet::SystemAddress::readFrom(buf.data(), offset);
    ASSERT_EQ(read_addr.getPort(), 19132);
    ASSERT_EQ(read_addr.getIP(), "127.0.0.1");
}

// =============================================================================
// SECTION: Network Layer Tests
// =============================================================================
SECTION("Network Layer Tests");

TEST(network_no_compression) {
    network::NoCompression comp;
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04, 0x05};
    
    auto compressed = comp.compress(data.data(), data.size());
    ASSERT_EQ(compressed.size(), data.size());
    
    auto decompressed = comp.decompress(compressed.data(), compressed.size());
    ASSERT_EQ(decompressed.size(), data.size());
    ASSERT_EQ(decompressed[0], 0x01);
    ASSERT_EQ(decompressed[4], 0x05);
}

TEST(network_no_encryption) {
    network::NoEncryption enc;
    ASSERT_TRUE(!enc.isEnabled());
    
    std::vector<uint8_t> data = {0xFE, 0x01, 0x02, 0x03};
    auto encrypted = enc.encrypt(data);
    ASSERT_EQ(encrypted.size(), data.size());
    
    auto decrypted = enc.decrypt(encrypted);
    ASSERT_EQ(decrypted.size(), data.size());
    ASSERT_EQ(decrypted[0], 0xFE);
}

TEST(network_batch_processor) {
    network::BatchPacketProcessor processor;
    
    // Create a simple packet
    TextPacket pkt;
    pkt.type = TextType::Raw;
    pkt.needs_translation = false;
    pkt.source_name = "";
    pkt.message = "Hello";
    pkt.xuid = "";
    pkt.platform_chat_id = "";
    pkt.filtered_message = "";
    
    // Encode
    auto encoded = processor.encodeSingle(pkt);
    ASSERT_TRUE(encoded.size() > 0);
    
    // Decode
    auto decoded = processor.decode(encoded);
    ASSERT_EQ(decoded.size(), 1u);
    ASSERT_EQ(static_cast<int>(decoded[0].first), static_cast<int>(PacketId::Text));
}

// =============================================================================
// SECTION: NBT Tests
// =============================================================================
SECTION("NBT Tests");

TEST(nbt_compound_roundtrip) {
    // Build a compound tag using factory methods
    auto root = NbtTag::makeCompound();
    root->compoundVal["byte_val"] = NbtTag::makeByte(42);
    root->compoundVal["short_val"] = NbtTag::makeShort(1234);
    root->compoundVal["int_val"] = NbtTag::makeInt(100000);
    root->compoundVal["long_val"] = NbtTag::makeLong(9876543210LL);
    root->compoundVal["float_val"] = NbtTag::makeFloat(3.14f);
    root->compoundVal["double_val"] = NbtTag::makeDouble(2.71828);
    root->compoundVal["string_val"] = NbtTag::makeString("Hello NBT!");
    
    auto byte_array = std::make_shared<NbtTag>(NbtTagType::ByteArray);
    byte_array->byteArrayVal = {1, 2, 3, 4, 5};
    root->compoundVal["byte_array"] = byte_array;
    
    auto int_array = std::make_shared<NbtTag>(NbtTagType::IntArray);
    int_array->intArrayVal = {100, 200, 300};
    root->compoundVal["int_array"] = int_array;
    
    auto list = std::make_shared<NbtTag>(NbtTagType::List);
    list->listTagType = NbtTagType::String;
    list->listVal.push_back(NbtTag::makeString("one"));
    list->listVal.push_back(NbtTag::makeString("two"));
    root->compoundVal["list_val"] = list;
    
    // Serialize
    BinaryStream write;
    NbtTag::writeNetworkNbt(write, *root);
    
    // Deserialize
    ReadOnlyBinaryStream read(write.getBuffer());
    auto decoded = NbtTag::readNetworkNbt(read);
    
    ASSERT_TRUE(decoded != nullptr);
    ASSERT_EQ(static_cast<int>(decoded->type), static_cast<int>(NbtTagType::Compound));
    
    // Verify values
    auto it_byte = decoded->compoundVal.find("byte_val");
    ASSERT_TRUE(it_byte != decoded->compoundVal.end());
    ASSERT_EQ(it_byte->second->byteVal, 42);
    
    auto it_int = decoded->compoundVal.find("int_val");
    ASSERT_TRUE(it_int != decoded->compoundVal.end());
    ASSERT_EQ(it_int->second->intVal, 100000);
    
    auto it_str = decoded->compoundVal.find("string_val");
    ASSERT_TRUE(it_str != decoded->compoundVal.end());
    ASSERT_EQ(it_str->second->stringVal, "Hello NBT!");
}

// =============================================================================
// SECTION: Offline Message Builder Tests
// =============================================================================
SECTION("Offline Message Tests");

TEST(offline_unconnected_pong) {
    auto pong = raknet::offline::buildUnconnectedPong(12345, 0xDEADBEEF, "MCPE;Test;944;1.26.1;0;10;");
    ASSERT_TRUE(pong.size() > 0);
    ASSERT_EQ(pong[0], static_cast<uint8_t>(raknet::MessageId::UnconnectedPong));
    
    auto parsed = raknet::offline::parseUnconnectedPong(pong.data(), pong.size());
    ASSERT_EQ(parsed.time, 12345);
    ASSERT_EQ(parsed.server_guid, 0xDEADBEEFu);
    ASSERT_EQ(parsed.server_info, "MCPE;Test;944;1.26.1;0;10;");
}

TEST(offline_open_connection_reply1) {
    auto reply = raknet::offline::buildOpenConnectionReply1(0x12345678, false, 1400);
    auto parsed = raknet::offline::parseOpenConnectionReply1(reply.data(), reply.size());
    
    ASSERT_EQ(parsed.server_guid, 0x12345678u);
    ASSERT_EQ(parsed.use_security, false);
    ASSERT_EQ(parsed.mtu_size, 1400);
}

TEST(offline_open_connection_reply2) {
    raknet::SystemAddress client_addr("192.168.1.100", 12345);
    auto reply = raknet::offline::buildOpenConnectionReply2(0xABCDEF, client_addr, 1200, false);
    auto parsed = raknet::offline::parseOpenConnectionReply2(reply.data(), reply.size());
    
    ASSERT_EQ(parsed.server_guid, 0xABCDEFu);
    ASSERT_EQ(parsed.mtu_size, 1200);
    ASSERT_EQ(parsed.encryption_enabled, false);
}

// =============================================================================
// SECTION: Mock Client - Comprehensive Round-Trip Tests for All Remaining Packets
// =============================================================================
SECTION("Mock Client - Remaining Packet Round-Trip Tests");

TEST(mock_add_actor) {
    AddActorPacket pkt;
    pkt.target_actor_id = {100};
    pkt.target_runtime_id = {100};
    pkt.actor_type = "minecraft:zombie";
    pkt.position = {10.0f, 64.0f, 20.0f};
    pkt.velocity = {0.0f, 0.0f, 0.0f};
    pkt.rotation = {0.0f, 0.0f};
    pkt.y_head_rotation = 45.0f;
    pkt.y_body_rotation = 45.0f;
    roundTripTest(pkt);
}

TEST(mock_add_item_actor) {
    AddItemActorPacket pkt;
    pkt.target_actor_id = {200};
    pkt.target_runtime_id = {200};
    pkt.position = {5.0f, 70.0f, 15.0f};
    pkt.velocity = {0.0f, -0.1f, 0.0f};
    pkt.from_fishing = false;
    roundTripTest(pkt);
}

TEST(mock_add_player) {
    AddPlayerPacket pkt;
    pkt.player_name = "TestPlayer";
    pkt.target_runtime_id = {300};
    pkt.platform_chat_id = "";
    pkt.position = {0.0f, 64.0f, 0.0f};
    pkt.velocity = {0.0f, 0.0f, 0.0f};
    pkt.rotation = {0.0f, 0.0f};
    pkt.y_head_rotation = 0.0f;
    pkt.player_game_type = 0;
    pkt.device_id = "";
    pkt.build_platform = 0;
    roundTripTest(pkt);
}

TEST(mock_agent_action_event) {
    AgentActionEventPacket pkt;
    pkt.request_id = "req-123";
    pkt.action = AgentActionEventPacket::AgentAction::None;
    pkt.response = "{}";
    roundTripTest(pkt);
}

TEST(mock_agent_animation) {
    AgentAnimationPacket pkt;
    pkt.animation = AgentAnimationPacket::AnimationType::ArmSwing;
    pkt.runtime_id = {42};
    roundTripTest(pkt);
}

TEST(mock_available_actor_identifiers) {
    AvailableActorIdentifiersPacket pkt;
    roundTripTest(pkt);
}

TEST(mock_biome_definition_list) {
    BiomeDefinitionListPacket pkt;
    roundTripTest(pkt);
}

TEST(mock_boss_event) {
    BossEventPacket pkt;
    pkt.target_actor_id = {500};
    pkt.event_type = BossEventPacket::EventType::Remove;
    roundTripTest(pkt);
}

TEST(mock_client_cache_blob_status) {
    ClientCacheBlobStatusPacket pkt;
    pkt.missing_ids = {1001, 1002};
    pkt.found_ids = {2001};
    roundTripTest(pkt);
}

TEST(mock_client_cache_miss_response) {
    ClientCacheMissResponsePacket pkt;
    ClientCacheMissResponsePacket::BlobEntry entry;
    entry.blob_id = 12345;
    entry.blob_data = "test_blob_data";
    pkt.missing_blobs.push_back(entry);
    roundTripTest(pkt);
}

TEST(mock_client_movement_prediction_sync) {
    ClientMovementPredictionSyncPacket pkt;
    pkt.flags_low = 0;
    pkt.flags_high = 0;
    pkt.bb_scale = 1.0f;
    pkt.bb_width = 0.6f;
    pkt.bb_height = 1.8f;
    pkt.actor_unique_id = {42};
    pkt.is_flying = false;
    roundTripTest(pkt);
}

TEST(mock_clientbound_attribute_layer_sync) {
    ClientboundAttributeLayerSyncPacket pkt;
    roundTripTest(pkt);
}

TEST(mock_command_block_update) {
    CommandBlockUpdatePacket pkt;
    pkt.is_block = true;
    pkt.block_position = {10, 64, 20};
    pkt.command_block_mode = 0;
    pkt.redstone_mode = false;
    pkt.is_conditional = false;
    pkt.command = "/say hello";
    pkt.last_output = "";
    pkt.name = "test_cmd";
    pkt.filtered_name = "test_cmd";
    pkt.track_output = false;
    pkt.tick_delay = 0;
    pkt.execute_on_first_tick = true;
    roundTripTest(pkt);
}

TEST(mock_compressed_biome_definition_list) {
    CompressedBiomeDefinitionListPacket pkt;
    roundTripTest(pkt);
}

TEST(mock_crafting_data) {
    CraftingDataPacket pkt;
    pkt.clear_recipes = true;
    roundTripTest(pkt);
}

TEST(mock_creative_content) {
    CreativeContentPacket pkt;
    roundTripTest(pkt);
}

TEST(mock_dimension_data) {
    DimensionDataPacket pkt;
    DimensionDefinition def;
    def.name = "minecraft:overworld";
    def.height_max = 320;
    def.height_min = -64;
    def.generator_type = 0;
    pkt.definitions.push_back(def);
    roundTripTest(pkt);
}

TEST(mock_graphics_override_parameter) {
    GraphicsOverrideParameterPacket pkt;
    roundTripTest(pkt);
}

TEST(mock_inventory_content) {
    InventoryContentPacket pkt;
    pkt.inventory_id = 0;
    roundTripTest(pkt);
}

TEST(mock_inventory_slot) {
    InventorySlotPacket pkt;
    pkt.container_id = 0;
    pkt.slot = 0;
    roundTripTest(pkt);
}

TEST(mock_inventory_transaction) {
    InventoryTransactionPacket pkt;
    pkt.legacy_request_id = 0;
    pkt.transaction_type = InventoryTransactionPacket::TransactionType::Normal;
    roundTripTest(pkt);
}

TEST(mock_item_registry) {
    ItemRegistryPacket pkt;
    roundTripTest(pkt);
}

TEST(mock_item_stack_request) {
    ItemStackRequestPacket pkt;
    roundTripTest(pkt);
}

TEST(mock_item_stack_response) {
    ItemStackResponsePacket pkt;
    roundTripTest(pkt);
}

TEST(mock_jigsaw_structure_data) {
    JigsawStructureDataPacket pkt;
    roundTripTest(pkt);
}

TEST(mock_keep_alive) {
    KeepAlivePacket pkt;
    roundTripTest(pkt);
}

TEST(mock_legacy_telemetry_event) {
    LegacyTelemetryEventPacket pkt;
    pkt.target_actor_id = {1};
    pkt.event_type = 0;
    pkt.use_player_id = false;
    roundTripTest(pkt);
}

TEST(mock_level_chunk) {
    LevelChunkPacket pkt;
    pkt.position = {0, 0};
    pkt.dimension_id = 0;
    pkt.client_needs_to_request_subchunks = false;
    pkt.sub_chunk_count = 0;
    pkt.cache_enabled = false;
    pkt.serialized_chunk_data = "";
    roundTripTest(pkt);
}

TEST(mock_locator_bar) {
    LocatorBarPacket pkt;
    roundTripTest(pkt);
}

TEST(mock_login) {
    LoginPacket pkt;
    pkt.client_network_version = 944;
    pkt.connection_request = "test_jwt_chain";
    roundTripTest(pkt);
}

TEST(mock_map_data) {
    MapDataPacket pkt;
    pkt.map_id = {1};
    pkt.type_flags = 0;
    pkt.dimension = 0;
    pkt.is_locked = false;
    pkt.map_origin = {0, 0, 0};
    roundTripTest(pkt);
}

TEST(mock_mob_armor_equipment) {
    MobArmorEquipmentPacket pkt;
    pkt.target_runtime_id = {42};
    roundTripTest(pkt);
}

TEST(mock_mob_equipment) {
    MobEquipmentPacket pkt;
    pkt.target_runtime_id = {42};
    pkt.slot = 0;
    pkt.selected_slot = 0;
    pkt.container_id = 0;
    roundTripTest(pkt);
}

TEST(mock_move_actor_delta) {
    MoveActorDeltaPacket pkt;
    pkt.runtime_id = {42};
    pkt.header = 0;
    roundTripTest(pkt);
}

TEST(mock_network_chunk_publisher_update) {
    NetworkChunkPublisherUpdatePacket pkt;
    pkt.position = {0, 64, 0};
    pkt.radius = 128;
    roundTripTest(pkt);
}

TEST(mock_party_changed) {
    PartyChangedPacket pkt;
    pkt.party_id = "party-123";
    roundTripTest(pkt);
}

TEST(mock_ping) {
    PingPacket pkt;
    pkt.send_time = 123456789;
    roundTripTest(pkt);
}

TEST(mock_player_auth_input) {
    PlayerAuthInputPacket pkt;
    pkt.rotation = {0.0f, 0.0f};
    pkt.position = {0.0f, 64.0f, 0.0f};
    pkt.move_vector = {0.0f, 0.0f};
    pkt.head_rotation = 0.0f;
    pkt.input_data_lo = 0;
    pkt.input_data_hi = 0;
    pkt.input_mode = 0;
    pkt.play_mode = 0;
    pkt.interaction_model = 0;
    pkt.interact_rotation = {0.0f, 0.0f};
    pkt.pos_delta = {0.0f, 0.0f, 0.0f};
    pkt.analog_move_vector = {0.0f, 0.0f};
    pkt.camera_orientation = {0.0f, 0.0f, 0.0f};
    pkt.raw_move_vector = {0.0f, 0.0f};
    roundTripTest(pkt);
}

TEST(mock_player_client_input_permissions) {
    PlayerClientInputPermissionsPacket pkt;
    pkt.permission_flags = 0;
    roundTripTest(pkt);
}

TEST(mock_player_enchant_options) {
    PlayerEnchantOptionsPacket pkt;
    roundTripTest(pkt);
}

TEST(mock_player_list) {
    PlayerListPacket pkt;
    pkt.action = PlayerListPacket::Action::Remove;
    roundTripTest(pkt);
}

TEST(mock_player_location) {
    PlayerLocationPacket pkt;
    pkt.type = PlayerLocationPacket::Type::Hide;
    pkt.target_actor_id = {42};
    roundTripTest(pkt);
}

TEST(mock_player_update_entity_overrides) {
    PlayerUpdateEntityOverridesPacket pkt;
    pkt.target_id = {42};
    pkt.property_index = 0;
    pkt.update_type = PlayerUpdateEntityOverridesPacket::UpdateType::ClearOverride;
    roundTripTest(pkt);
}

TEST(mock_position_tracking_db_client_request) {
    PositionTrackingDBClientRequestPacket pkt;
    pkt.action = 0;
    pkt.tracking_id = 1;
    roundTripTest(pkt);
}

TEST(mock_position_tracking_db_server_broadcast) {
    PositionTrackingDBServerBroadcastPacket pkt;
    pkt.action = PositionTrackingDBServerBroadcastPacket::Action::Update;
    pkt.tracking_id = 1;
    roundTripTest(pkt);
}

TEST(mock_remove_volume_entity) {
    RemoveVolumeEntityPacket pkt;
    pkt.entity_net_id = 100;
    pkt.dimension_type = 0;
    roundTripTest(pkt);
}

TEST(mock_request_ability) {
    RequestAbilityPacket pkt;
    pkt.ability = 0;
    pkt.value_type = RequestAbilityPacket::ValueType::Bool;
    pkt.bool_value = true;
    pkt.float_value = 0.0f;
    roundTripTest(pkt);
}

TEST(mock_request_permissions) {
    RequestPermissionsPacket pkt;
    pkt.target_player_raw_id = 1;
    pkt.player_permission_level = 1;
    pkt.custom_permission_flags = 0;
    roundTripTest(pkt);
}

TEST(mock_resource_pack_client_response) {
    ResourcePackClientResponsePacket pkt;
    pkt.response = ResourcePackResponseType::None;
    roundTripTest(pkt);
}

TEST(mock_resource_pack_stack) {
    ResourcePackStackPacket pkt;
    pkt.must_accept = false;
    pkt.base_game_version = "1.26.1";
    pkt.has_previously_used_experiments = false;
    pkt.include_editor_packs = false;
    roundTripTest(pkt);
}

TEST(mock_resource_packs_info) {
    ResourcePacksInfoPacket pkt;
    pkt.must_accept = false;
    pkt.has_addon_packs = false;
    pkt.has_scripts = false;
    pkt.force_disable_vibrant_visuals = false;
    pkt.world_template_version = "";
    roundTripTest(pkt);
}

TEST(mock_script_custom_event) {
    ScriptCustomEventPacket pkt;
    pkt.event_name = "test:event";
    pkt.event_data = "{\"key\":\"value\"}";
    roundTripTest(pkt);
}

TEST(mock_server_script_debug_drawer) {
    ServerScriptDebugDrawerPacket pkt;
    pkt.draw_data = "";
    roundTripTest(pkt);
}

TEST(mock_serverbound_data_driven_screen_closed) {
    ServerboundDataDrivenScreenClosedPacket pkt;
    pkt.form_id = 1;
    pkt.close_reason = ServerboundDataDrivenScreenClosedPacket::CloseReason::ProgrammaticClose;
    roundTripTest(pkt);
}

TEST(mock_serverbound_pack_setting_change) {
    ServerboundPackSettingChangePacket pkt;
    pkt.pack_setting_name = "test_setting";
    roundTripTest(pkt);
}

TEST(mock_set_actor_data) {
    SetActorDataPacket pkt;
    pkt.target_runtime_id = {42};
    roundTripTest(pkt);
}

TEST(mock_set_score) {
    SetScorePacket pkt;
    pkt.type = SetScorePacket::Type::Remove;
    roundTripTest(pkt);
}

TEST(mock_set_scoreboard_identity) {
    SetScoreboardIdentityPacket pkt;
    pkt.type = SetScoreboardIdentityPacket::Type::Clear;
    roundTripTest(pkt);
}

TEST(mock_start_game) {
    StartGamePacket pkt;
    pkt.entity_id = {1};
    pkt.runtime_id = {1};
    pkt.player_game_type = 0;
    pkt.position = {0.0f, 64.0f, 0.0f};
    pkt.rotation = {0.0f, 0.0f};
    pkt.level_id = "level-1";
    pkt.level_name = "Test World";
    pkt.template_content_identity = "";
    pkt.is_trial = false;
    pkt.level_current_time = 0;
    pkt.enchantment_seed = 0;
    pkt.multiplayer_correlation_id = "";
    pkt.enable_item_stack_net_manager = true;
    pkt.server_version = "1.26.1";
    pkt.server_block_type_registry_checksum = 0;
    pkt.server_enabled_client_side_generation = false;
    pkt.block_network_ids_are_hashes = true;
    pkt.server_auth_sound_enabled = false;
    pkt.has_server_join_info = false;
    roundTripTest(pkt);
}

TEST(mock_structure_block_update) {
    StructureBlockUpdatePacket pkt;
    pkt.block_position = {0, 64, 0};
    pkt.structure_data_raw = "";
    pkt.trigger = false;
    pkt.is_waterlogged = false;
    roundTripTest(pkt);
}

TEST(mock_structure_template_data_request) {
    StructureTemplateDataRequestPacket pkt;
    pkt.structure_name = "test_structure";
    pkt.position = {0, 64, 0};
    pkt.palette_name = "";
    pkt.ignore_entities = false;
    pkt.ignore_blocks = false;
    pkt.allow_non_ticking_chunks = false;
    pkt.structure_size = {1, 1, 1};
    pkt.structure_offset = {0, 0, 0};
    pkt.last_edit_player = {0};
    pkt.rotation = 0;
    pkt.mirror = 0;
    pkt.animation_mode = 0;
    pkt.animation_seconds = 0.0f;
    pkt.integrity_value = 100.0f;
    pkt.integrity_seed = 0;
    pkt.rotation_pivot = {0.0f, 0.0f, 0.0f};
    pkt.requested_operation = 0;
    roundTripTest(pkt);
}

TEST(mock_structure_template_data_response) {
    StructureTemplateDataResponsePacket pkt;
    pkt.structure_name = "test_structure";
    pkt.response_type = StructureTemplateDataResponsePacket::ResponseType::None;
    roundTripTest(pkt);
}

TEST(mock_sub_chunk) {
    SubChunkPacket pkt;
    pkt.cache_enabled = false;
    pkt.dimension_type = 0;
    pkt.center_pos = {0, 0, 0};
    roundTripTest(pkt);
}

TEST(mock_sub_chunk_request) {
    SubChunkRequestPacket pkt;
    pkt.dimension_type = 0;
    pkt.center_pos = {0, 0, 0};
    roundTripTest(pkt);
}

TEST(mock_sub_client_login) {
    SubClientLoginPacket pkt;
    pkt.connection_request = "test_jwt";
    roundTripTest(pkt);
}

TEST(mock_sync_world_clocks) {
    SyncWorldClocksPacket pkt;
    roundTripTest(pkt);
}

TEST(mock_trim_data) {
    TrimDataPacket pkt;
    TrimDataPacket::TrimPattern pattern;
    pattern.item_name = "minecraft:coast_armor_trim_smithing_template";
    pattern.pattern_id = "coast";
    pkt.patterns.push_back(pattern);
    TrimDataPacket::TrimMaterial material;
    material.material_id = "iron";
    material.color = "#CECECE";
    material.item_name = "minecraft:iron_ingot";
    pkt.materials.push_back(material);
    roundTripTest(pkt);
}

TEST(mock_update_abilities) {
    UpdateAbilitiesPacket pkt;
    roundTripTest(pkt);
}

TEST(mock_update_adventure_settings) {
    UpdateAdventureSettingsPacket pkt;
    pkt.no_pvm = false;
    pkt.no_mvp = false;
    pkt.immutable_world = false;
    pkt.show_name_tags = true;
    pkt.auto_jump = true;
    roundTripTest(pkt);
}

TEST(mock_update_attributes) {
    UpdateAttributesPacket pkt;
    pkt.runtime_id = {42};
    roundTripTest(pkt);
}


// =============================================================================
// Main
// =============================================================================
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "CpherTunnel Test Suite" << std::endl;
    std::cout << "Protocol Version: " << PROTOCOL_VERSION << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Tests are auto-registered and run via static initialization above
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Results: " << tests_passed << " passed, " 
              << tests_failed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return tests_failed > 0 ? 1 : 0;
}
