// =============================================================================
// Test Server: A minimal Bedrock server that a real client can join
// Sends a flat world floor (stone + grass) so the player can see terrain
// Protocol Version: 944 (Minecraft Bedrock 1.26.1)
// =============================================================================

#include "cphertunnel/cphertunnel.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include <map>
#include <set>

using namespace cphertunnel;
using namespace cphertunnel::network;
using namespace cphertunnel::raknet;

// =============================================================================
// RawPacket: A packet built from raw pre-serialized bytes
// Used for packets we don't have full classes for (UpdateAbilities, etc.)
// =============================================================================
class RawPacket : public Packet {
public:
    PacketId packet_id_;
    std::string payload_;

    RawPacket(PacketId id, std::string payload)
        : packet_id_(id), payload_(std::move(payload)) {}

    PacketId getId() const override { return packet_id_; }
    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(payload_);
    }
    void read(ReadOnlyBinaryStream&) override {}
};

// =============================================================================
// Block Runtime ID Hashing (FNV1a-32)
// When block_network_ids_are_hashes = true, runtime IDs are FNV1a hashes
// of the canonical block state NBT
// =============================================================================
static uint32_t fnv1a_32(const uint8_t* data, size_t len) {
    uint32_t hash = 0x811c9dc5u;
    for (size_t i = 0; i < len; ++i) {
        hash ^= data[i];
        hash *= 0x01000193u;
    }
    return hash;
}

// Serialize a block state to NBT and hash it to get the runtime ID
static uint32_t computeBlockRuntimeId(const std::string& block_name) {
    // Build NBT: CompoundTag { "name": StringTag(block_name), "states": CompoundTag {} }
    BinaryStream temp;

    // Root compound tag header
    temp.writeByte(static_cast<uint8_t>(NbtTagType::Compound)); // tag type
    temp.writeUnsignedShort(0); // empty root name

    // "name" field: String tag
    temp.writeByte(static_cast<uint8_t>(NbtTagType::String));
    temp.writeUnsignedShort(4); // "name" length
    temp.write("name", 4);
    temp.writeUnsignedShort(static_cast<uint16_t>(block_name.size()));
    temp.write(block_name.data(), block_name.size());

    // "states" field: Empty compound tag
    temp.writeByte(static_cast<uint8_t>(NbtTagType::Compound));
    temp.writeUnsignedShort(6); // "states" length
    temp.write("states", 6);
    temp.writeByte(static_cast<uint8_t>(NbtTagType::End)); // end of inner compound

    // End of root compound
    temp.writeByte(static_cast<uint8_t>(NbtTagType::End));

    auto& buf = temp.getBuffer();
    return fnv1a_32(reinterpret_cast<const uint8_t*>(buf.data()), buf.size());
}

// Pre-computed block runtime IDs
static uint32_t AIR_RUNTIME_ID = 0;
static uint32_t STONE_RUNTIME_ID = 0;
static uint32_t GRASS_RUNTIME_ID = 0;
static uint32_t DIRT_RUNTIME_ID = 0;
static uint32_t BEDROCK_RUNTIME_ID = 0;

static void initBlockRuntimeIds() {
    AIR_RUNTIME_ID = computeBlockRuntimeId("minecraft:air");
    STONE_RUNTIME_ID = computeBlockRuntimeId("minecraft:stone");
    GRASS_RUNTIME_ID = computeBlockRuntimeId("minecraft:grass_block");
    DIRT_RUNTIME_ID = computeBlockRuntimeId("minecraft:dirt");
    BEDROCK_RUNTIME_ID = computeBlockRuntimeId("minecraft:bedrock");

    std::cout << "Block Runtime IDs (FNV1a hashes):" << std::endl;
    std::cout << "  air:         " << AIR_RUNTIME_ID << std::endl;
    std::cout << "  stone:       " << STONE_RUNTIME_ID << std::endl;
    std::cout << "  grass_block: " << GRASS_RUNTIME_ID << std::endl;
    std::cout << "  dirt:        " << DIRT_RUNTIME_ID << std::endl;
    std::cout << "  bedrock:     " << BEDROCK_RUNTIME_ID << std::endl;
}

// =============================================================================
// Sub-Chunk Serialization (Bedrock format)
// =============================================================================

// Write a signed (zigzag-encoded) VarInt to a raw byte buffer
static void writeSignedVarIntToBuffer(std::string& buf, int32_t value) {
    uint32_t uval = static_cast<uint32_t>(value);
    // ZigZag encode for signed VarInt
    uval = (uval << 1) ^ (static_cast<uint32_t>(value >> 31));
    do {
        uint8_t byte = uval & 0x7F;
        uval >>= 7;
        if (uval != 0) byte |= 0x80;
        buf.push_back(static_cast<char>(byte));
    } while (uval != 0);
}

// Write an unsigned VarInt to a raw byte buffer (for palette sizes)
static void writeUnsignedVarIntToBuffer(std::string& buf, uint32_t value) {
    do {
        uint8_t byte = value & 0x7F;
        value >>= 7;
        if (value != 0) byte |= 0x80;
        buf.push_back(static_cast<char>(byte));
    } while (value != 0);
}

// Build a sub-chunk where all 4096 blocks are the same runtime ID
static std::string buildUniformSubChunk(uint32_t runtime_id, int8_t y_index) {
    std::string data;

    // Sub-chunk version 9
    data.push_back(9);
    // 1 storage layer
    data.push_back(1);
    // Y index for this sub-chunk
    data.push_back(static_cast<char>(y_index));

    // Storage layer: 0 bits per block (all same block), runtime IDs
    // palette_and_flag = (bits_per_block << 1) | is_runtime_id
    // = (0 << 1) | 1 = 1
    data.push_back(1);

    // No block indices needed (0 bits * 4096 = 0 bytes)

    // For 0 bpb: NO palette size - just the single palette entry directly
    // Palette entry: the block runtime ID (signed/zigzag VarInt)
    writeSignedVarIntToBuffer(data, static_cast<int32_t>(runtime_id));

    return data;
}

// Build a sub-chunk with two block types: one block type for the bottom layer (y=0),
// another for the rest (y=1..15)
static std::string buildTwoLayerSubChunk(uint32_t bottom_id, uint32_t fill_id, int8_t y_index) {
    std::string data;

    // Sub-chunk version 9
    data.push_back(9);
    // 1 storage layer
    data.push_back(1);
    // Y index
    data.push_back(static_cast<char>(y_index));

    // 1 bit per block = (1 << 1) | 1 = 3
    data.push_back(3);

    // Block indices: 4096 blocks, 1 bit each = 4096 bits = 128 uint32_t words
    // Block order in Bedrock: (x << 8) | (z << 4) | y
    // For each (x,z) column: y=0 is index 0 (bottom_id), y=1..15 is index 1 (fill_id)
    // In bits per word (32 blocks per word): the pattern for each column of 16 is:
    // bit 0 = 0 (palette index 0 = bottom_id), bits 1-15 = 1 (palette index 1 = fill_id)

    // Each column occupies 16 bits: 0xFFFE (y=0 is bit 0 = 0, y=1..15 = 1)
    // Two columns per uint32_t: 0xFFFEFFFE
    // But wait - blocks are packed sequentially in XZY order:
    // block(0,0,0), block(0,0,1), ..., block(0,0,15), block(0,1,0), ..., block(0,1,15), ...
    // With 1 bit per block, 32 blocks fit in one uint32_t
    // First 32 blocks: x=0,z=0,y=0..15 (16 blocks) + x=0,z=1,y=0..15 (16 blocks)
    // Pattern: 0xFFFEFFFE for each pair of columns

    uint32_t word = 0xFFFEFFFEu;  // Two columns: bottom=0, rest=1
    for (int i = 0; i < 128; ++i) {
        // Little-endian uint32_t
        data.push_back(static_cast<char>(word & 0xFF));
        data.push_back(static_cast<char>((word >> 8) & 0xFF));
        data.push_back(static_cast<char>((word >> 16) & 0xFF));
        data.push_back(static_cast<char>((word >> 24) & 0xFF));
    }

    // Palette size: signed/zigzag VarInt, then entries as signed/zigzag VarInt
    writeSignedVarIntToBuffer(data, 2);
    writeSignedVarIntToBuffer(data, static_cast<int32_t>(bottom_id));
    writeSignedVarIntToBuffer(data, static_cast<int32_t>(fill_id));

    return data;
}

// =============================================================================
// Build serialized chunk data for a flat world
// =============================================================================
static std::string buildFlatChunkData() {
    std::string data;

    // World height: -64 to 319 (24 sub-chunks, indices -4 to 19)
    // Flat world layout:
    //   y = -64  : bedrock (sub-chunk -4, layer 0)
    //   y = -63 to -2 : stone (sub-chunks -4 rest, -3, -2, -1 except top)
    //   y = -1   : dirt (sub-chunk -1, top layer)
    //   y = 0    : grass (sub-chunk 0, bottom layer)
    //   y >= 1   : air

    // Sub-chunk -4 (y=-64 to -49): bottom layer bedrock, rest stone
    data += buildTwoLayerSubChunk(BEDROCK_RUNTIME_ID, STONE_RUNTIME_ID, -4);

    // Sub-chunks -3, -2 (y=-48 to -17): all stone
    data += buildUniformSubChunk(STONE_RUNTIME_ID, -3);
    data += buildUniformSubChunk(STONE_RUNTIME_ID, -2);

    // Sub-chunk -1 (y=-16 to -1): bottom 15 layers stone, top layer dirt
    // Using uniform stone for simplicity (close enough for visual)
    data += buildUniformSubChunk(STONE_RUNTIME_ID, -1);

    // Sub-chunk 0 (y=0 to 15): bottom layer grass_block, rest air
    data += buildTwoLayerSubChunk(GRASS_RUNTIME_ID, AIR_RUNTIME_ID, 0);

    // Sub-chunks 1-19 (y=16 to 319): all air
    for (int8_t i = 1; i <= 19; ++i) {
        data += buildUniformSubChunk(AIR_RUNTIME_ID, i);
    }

    // Biome data: Use the "new" biome encoding since 1.18
    // Each sub-chunk has its own biome palette
    // Simple approach: for each of the 24 sub-chunks, write a single-biome palette
    for (int i = 0; i < 24; ++i) {
        // Biome palette for this sub-chunk: 0 bits = single biome
        // palette_and_flag = (0 << 1) | 0 = 0
        // But actually biome encoding doesn't use the runtime flag bit
        // The format is just: bits_per_block (not shifted), then palette
        // For single biome: write the biome ID directly as a VarInt
        // Actually, biome sections are similar to block storage but simpler:
        // If the section is uniform, write a single palette entry

        // Biome section header: single biome (0 bits per block)
        // Encoding: palette entry count of 1, then the biome ID
        // Actually, for biome data in Bedrock, the format for uniform biome is:
        // byte: (0 << 1) = 0 (0 bits per block, no runtime flag)
        // varint: biome ID (plains = 1)
        data.push_back(0); // 0 bits per block, no runtime flag
        // For 0 bpb: single palette entry directly (signed/zigzag VarInt)
        writeSignedVarIntToBuffer(data, 1); // biome ID: plains = 1
    }

    // Border blocks: 0 = no border blocks
    data.push_back(0);

    // No tile entities (block entities)
    // Nothing to write - end of data

    return data;
}

// =============================================================================
// Ability flag indices for UpdateAbilitiesPacket
// =============================================================================
enum AbilityIndex {
    ABILITY_BUILD = 0,
    ABILITY_MINE = 1,
    ABILITY_DOORS_AND_SWITCHES = 2,
    ABILITY_OPEN_CONTAINERS = 3,
    ABILITY_ATTACK_PLAYERS = 4,
    ABILITY_ATTACK_MOBS = 5,
    ABILITY_OPERATOR_COMMANDS = 6,
    ABILITY_TELEPORT = 7,
    ABILITY_INVULNERABLE = 8,
    ABILITY_FLYING = 9,
    ABILITY_MAY_FLY = 10,
    ABILITY_INSTABUILD = 11,
    ABILITY_LIGHTNING = 12,
    ABILITY_FLY_SPEED = 13,
    ABILITY_WALK_SPEED = 14,
    ABILITY_MUTED = 15,
    ABILITY_WORLD_BUILDER = 16,
    ABILITY_NO_CLIP = 17,
    ABILITY_PRIVILEGED_BUILDER = 18,
    ABILITY_VERTICAL_FLY_SPEED = 19,
};

static RawPacket buildUpdateAbilitiesPacket(int64_t entity_unique_id, bool flying = false) {
    BinaryStream s;
    // SerializedAbilitiesData
    s.writeSignedInt64(entity_unique_id); // Target Player Raw Id (int64 LE)
    s.writeByte(2);  // Player Permissions: Operator
    s.writeByte(1);  // Command Permissions: GameDirectors

    // Layers count
    s.writeUnsignedVarInt(2); // Base + Commands

    // --- Layer 1: Base (type=1) ---
    s.writeUnsignedShort(1); // SerializedLayer = Base

    // abilities_set: define all 20 abilities
    uint32_t abilities_set = 0xFFFFF; // bits 0-19
    s.writeUnsignedInt(abilities_set);

    // ability_values for Creative mode
    uint32_t ability_values = 0;
    ability_values |= (1 << ABILITY_BUILD);
    ability_values |= (1 << ABILITY_MINE);
    ability_values |= (1 << ABILITY_DOORS_AND_SWITCHES);
    ability_values |= (1 << ABILITY_OPEN_CONTAINERS);
    ability_values |= (1 << ABILITY_ATTACK_PLAYERS);
    ability_values |= (1 << ABILITY_ATTACK_MOBS);
    ability_values |= (1 << ABILITY_OPERATOR_COMMANDS);
    ability_values |= (1 << ABILITY_TELEPORT);
    ability_values |= (1 << ABILITY_INVULNERABLE);
    if (flying) ability_values |= (1 << ABILITY_FLYING);
    ability_values |= (1 << ABILITY_MAY_FLY);
    ability_values |= (1 << ABILITY_INSTABUILD);
    // Lightning = 0
    ability_values |= (1 << ABILITY_FLY_SPEED);
    ability_values |= (1 << ABILITY_WALK_SPEED);
    // Muted = 0, WorldBuilder = 0, NoClip = 0, PrivilegedBuilder = 0
    ability_values |= (1 << ABILITY_VERTICAL_FLY_SPEED);
    s.writeUnsignedInt(ability_values);

    s.writeFloat(0.05f);  // FlySpeed
    s.writeFloat(0.05f);  // VerticalFlySpeed
    s.writeFloat(0.1f);   // WalkSpeed

    // --- Layer 2: Commands (type=3) ---
    s.writeUnsignedShort(3); // SerializedLayer = Commands
    s.writeUnsignedInt(1 << ABILITY_OPERATOR_COMMANDS); // abilities_set
    s.writeUnsignedInt(1 << ABILITY_OPERATOR_COMMANDS); // ability_values = true
    s.writeFloat(0.0f);  // FlySpeed (unused)
    s.writeFloat(0.0f);  // VerticalFlySpeed (unused)
    s.writeFloat(0.0f);  // WalkSpeed (unused)

    return RawPacket(PacketId::UpdateAbilities, std::string(s.getBuffer()));
}

static RawPacket buildUpdateAttributesPacket(uint64_t runtime_id) {
    BinaryStream s;
    s.writeActorRuntimeID({runtime_id}); // Target actor

    // Attribute count
    s.writeUnsignedVarInt(3); // health, movement, player.hunger

    // Attribute: minecraft:health
    s.writeFloat(0.0f);   // min
    s.writeFloat(20.0f);  // max
    s.writeFloat(20.0f);  // current
    s.writeFloat(0.0f);   // default_min
    s.writeFloat(20.0f);  // default_max
    s.writeFloat(20.0f);  // default_value
    s.writeString("minecraft:health");
    s.writeUnsignedVarInt(0); // 0 modifiers

    // Attribute: minecraft:movement (player walk speed)
    s.writeFloat(0.0f);           // min
    s.writeFloat(3.4028235e+38f); // max (FLT_MAX)
    s.writeFloat(0.1f);           // current (normal walk speed)
    s.writeFloat(0.0f);           // default_min
    s.writeFloat(3.4028235e+38f); // default_max
    s.writeFloat(0.1f);           // default_value
    s.writeString("minecraft:movement");
    s.writeUnsignedVarInt(0); // 0 modifiers

    // Attribute: minecraft:player.hunger
    s.writeFloat(0.0f);   // min
    s.writeFloat(20.0f);  // max
    s.writeFloat(20.0f);  // current
    s.writeFloat(0.0f);   // default_min
    s.writeFloat(20.0f);  // default_max
    s.writeFloat(20.0f);  // default_value
    s.writeString("minecraft:player.hunger");
    s.writeUnsignedVarInt(0); // 0 modifiers

    // Tick (PlayerInputTick) = 0 (initial)
    s.writeUnsignedVarInt64(0);

    return RawPacket(PacketId::UpdateAttributes, std::string(s.getBuffer()));
}

static RawPacket buildUpdateAdventureSettingsPacket() {
    BinaryStream s;
    s.writeBool(false); // no PvM (no Player vs Mobs = false → PvM allowed)
    s.writeBool(false); // no MvP (no Mobs vs Player = false → MvP allowed)
    s.writeBool(false); // immutable world = false
    s.writeBool(true);  // show name tags = true
    s.writeBool(true);  // auto jump = true
    return RawPacket(PacketId::UpdateAdventureSettings, std::string(s.getBuffer()));
}

// =============================================================================
// MOTD formatting for Bedrock
// =============================================================================
static std::string buildMotd(uint16_t port) {
    // Format: MCPE;motd;protocol;version;players;max_players;server_id;level_name;gamemode;...
    return "MCPE;Test Server;924;1.21.60;0;20;"
           + std::to_string(port) + ";Flat World;Creative;1;19132;19132;0;";
}

// =============================================================================
// Per-client state tracking
// =============================================================================
enum class ClientState {
    WaitingForRequestNetworkSettings,
    WaitingForLogin,
    WaitingForResourcePackResponse,
    WaitingForChunkRadiusRequest,
    Spawned,
};

struct ClientInfo {
    SystemAddress address;
    RakNetGUID guid;
    ClientState state = ClientState::WaitingForRequestNetworkSettings;
    bool chunks_sent = false;
    uint64_t tick_count = 0;
    uint64_t last_time_sync = 0;
    uint64_t auth_input_count = 0;
    bool is_flying = false;
};

// =============================================================================
// Logging helper - writes to both stdout and a log file
// =============================================================================
static FILE* g_logFile = nullptr;

static void logMsg(const std::string& msg) {
    std::cout << msg << std::endl;
    if (g_logFile) {
        fprintf(g_logFile, "%s\n", msg.c_str());
        fflush(g_logFile);
    }
}

// =============================================================================
// Main
// =============================================================================
int main(int argc, char* argv[]) {
    // Force unbuffered stdout for diagnostic output
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    // Open direct log file
    g_logFile = fopen("test_server_direct.log", "w");
    if (g_logFile) setvbuf(g_logFile, NULL, _IONBF, 0);

    uint16_t port = 19132;
    if (argc >= 2) port = static_cast<uint16_t>(std::atoi(argv[1]));

    initBlockRuntimeIds();

    std::cout << "\n=== Bedrock Test Server ===" << std::endl;
    std::cout << "Protocol: " << PROTOCOL_VERSION << " (Minecraft " << MINECRAFT_VERSION_STRING << ")" << std::endl;
    std::cout << "Port: " << port << std::endl;

    // Create server
    auto handler = ConnectionHandler::createServer(port);
    if (!handler) {
        std::cerr << "Failed to start server on port " << port << std::endl;
        return 1;
    }

    handler->setMaxConnections(20);
    handler->setServerInfo(buildMotd(port));

    std::cout << "Server started! Waiting for connections..." << std::endl;
    std::cout << "Connect with Minecraft Bedrock 1.26.1 to 127.0.0.1:" << port << std::endl;

    std::map<std::string, ClientInfo> clients;

    // Helper: log serialized packet size
    auto logPacketSize = [](const char* name, const cphertunnel::Packet& pkt) {
        auto serialized = pkt.serialize();
        std::cout << "    [size] " << name << ": " << serialized.size() << " bytes (payload)" << std::endl;
    };

    // Pre-build chunk data (reusable for all chunks)
    std::string flat_chunk_data = buildFlatChunkData();
    std::cout << "Flat chunk data size: " << flat_chunk_data.size() << " bytes" << std::endl;

    while (true) {
      try {
        auto events = handler->tick();

        for (auto& evt : events) {
            std::string addr_str = evt.address.toString();

            switch (evt.type) {
                case ConnectionEvent::Type::Connected: {
                    std::cout << "[+] Client connected: " << addr_str << std::endl;
                    ClientInfo ci;
                    ci.address = evt.address;
                    ci.guid = evt.guid;
                    ci.state = ClientState::WaitingForRequestNetworkSettings;
                    clients[addr_str] = ci;
                    break;
                }

                case ConnectionEvent::Type::Disconnected: {
                    auto it = clients.find(addr_str);
                    std::string state_str = "unknown";
                    if (it != clients.end()) {
                        switch (it->second.state) {
                            case ClientState::WaitingForRequestNetworkSettings: state_str = "WaitingForRequestNetworkSettings"; break;
                            case ClientState::WaitingForLogin: state_str = "WaitingForLogin"; break;
                            case ClientState::WaitingForResourcePackResponse: state_str = "WaitingForResourcePackResponse"; break;
                            case ClientState::WaitingForChunkRadiusRequest: state_str = "WaitingForChunkRadiusRequest"; break;
                            case ClientState::Spawned: state_str = "Spawned"; break;
                        }
                    }
                    logMsg("[-] Client disconnected: " + addr_str + " (state=" + state_str + ")");
                    clients.erase(addr_str);
                    break;
                }

                case ConnectionEvent::Type::PacketReceived: {
                    auto it = clients.find(addr_str);
                    if (it == clients.end()) break;
                    auto& client = it->second;

                    std::cout << "[<] Received: " << packetIdToString(evt.packet_id)
                              << " (" << static_cast<int>(evt.packet_id) << ")"
                              << " from " << addr_str
                              << " [" << evt.raw_data.size() << " bytes]" << std::endl;

                    // (hex dumping moved below, after spammy packet filter)

                    // Only hex dump for non-spammy packets
                    bool suppress_hex = (evt.packet_id == PacketId::PlayerAuthInput);
                    if (!suppress_hex) {
                        size_t dump_len = std::min(evt.raw_data.size(), (size_t)32);
                        std::cout << "    hex: ";
                        for (size_t i = 0; i < dump_len; ++i) {
                            char hex[4];
                            snprintf(hex, sizeof(hex), "%02X ", evt.raw_data[i]);
                            std::cout << hex;
                        }
                        if (evt.raw_data.size() > 32) std::cout << "...";
                        std::cout << std::endl;
                    }

                    switch (evt.packet_id) {

                    // ─── Step 1: Client requests network settings ────
                    case PacketId::RequestNetworkSettings: {
                        RequestNetworkSettingsPacket req;
                        {
                            ReadOnlyBinaryStream rs(evt.raw_data.data(), evt.raw_data.size());
                            req.read(rs);
                        }

                        std::cout << "    Client protocol version: " << req.client_network_version << std::endl;

                        if (req.client_network_version != PROTOCOL_VERSION) {
                            std::cout << "    Protocol mismatch (expected " << PROTOCOL_VERSION
                                      << "), but continuing anyway..." << std::endl;
                        }

                        // Send NetworkSettings
                        // IMPORTANT: Must send BEFORE enabling compression header,
                        // because this packet itself is sent without the header byte
                        NetworkSettingsPacket settings;
                        settings.compression_threshold = 0; // 0 = disable compression
                        settings.compression_algorithm = CompressionAlgorithm::None;
                        settings.client_throttle_enabled = false;
                        settings.client_throttle_threshold = 0;
                        settings.client_throttle_scalar = 0.0f;
                        handler->sendPacket(evt.address, settings);

                        std::cout << "[>] Sent NetworkSettings (algorithm=None, threshold=0 [disabled])" << std::endl;

                        // NOW enable compression header on our side (after sending NetworkSettings)
                        auto* bc = handler->getConnection(evt.address);
                        if (bc) {
                            bc->getProcessor().setCompressionInitialized(true);
                        }

                        client.state = ClientState::WaitingForLogin;
                        break;
                    }

                    // ─── Step 2: Client sends Login ──────────────────
                    case PacketId::Login: {
                        LoginPacket login;
                        {
                            ReadOnlyBinaryStream rs(evt.raw_data.data(), evt.raw_data.size());
                            login.read(rs);
                        }

                        std::cout << "    Login received (protocol=" << login.client_network_version << ")" << std::endl;

                        // Send PlayStatus: Login Success
                        PlayStatusPacket play_status;
                        play_status.status = PlayStatusType::LoginSuccess;
                        handler->sendPacket(evt.address, play_status);
                        std::cout << "[>] Sent PlayStatus(LoginSuccess)" << std::endl;

                        // Send ResourcePacksInfo (no resource packs)
                        ResourcePacksInfoPacket packs_info;
                        packs_info.must_accept = false;
                        packs_info.has_addon_packs = false;
                        packs_info.has_scripts = false;
                        handler->sendPacket(evt.address, packs_info);
                        std::cout << "[>] Sent ResourcePacksInfo (empty)" << std::endl;

                        client.state = ClientState::WaitingForResourcePackResponse;
                        break;
                    }

                    // ─── Step 3: Client responds to resource packs ───
                    case PacketId::ResourcePackClientResponse: {
                        ResourcePackClientResponsePacket resp;
                        {
                            ReadOnlyBinaryStream rs(evt.raw_data.data(), evt.raw_data.size());
                            resp.read(rs);
                        }

                        std::cout << "    ResourcePack response: " << static_cast<int>(resp.response) << std::endl;

                        if (resp.response == ResourcePackResponseType::AllPacksDownloaded ||
                            resp.response == ResourcePackResponseType::Completed ||
                            resp.response == ResourcePackResponseType::None) {

                            if (resp.response != ResourcePackResponseType::Completed) {
                                // Send ResourcePackStack
                                ResourcePackStackPacket stack;
                                stack.must_accept = false;
                                stack.base_game_version = "*";
                                handler->sendPacket(evt.address, stack);
                                std::cout << "[>] Sent ResourcePackStack" << std::endl;
                                break;
                            }

                            // ─── Send StartGamePacket ────────────────
                            StartGamePacket start_game;
                            start_game.entity_id = {1};
                            start_game.runtime_id = {1};
                            start_game.player_game_type = 1; // Creative
                            start_game.position = {0.0f, 1.0f, 0.0f}; // On the grass surface
                            start_game.rotation = {0.0f, 0.0f};

                            // Level settings
                            auto& s = start_game.settings;
                            s.seed = 12345;
                            s.spawn_settings.type = 0;
                            s.spawn_settings.dimension = 0;
                            s.generator_type = 2; // Flat
                            s.game_type = 1; // Creative
                            s.difficulty = 1; // Easy
                            s.default_spawn_position = {0, 1, 0};
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
                            s.player_permissions = 2; // Operator

                            start_game.level_id = "test_level";
                            start_game.level_name = "Flat World Test";
                            start_game.template_content_identity = "";
                            start_game.is_trial = false;
                            start_game.movement_settings.rewind_history_size = 40;
                            start_game.movement_settings.server_authoritative_block_breaking = false;
                            start_game.level_current_time = 6000;
                            start_game.enchantment_seed = 0;
                            start_game.multiplayer_correlation_id = "";
                            start_game.enable_item_stack_net_manager = true;
                            start_game.server_version = MINECRAFT_VERSION_STRING;
                            start_game.server_block_type_registry_checksum = 0;
                            start_game.world_template_id = {};
                            start_game.server_enabled_client_side_generation = false;
                            start_game.block_network_ids_are_hashes = true;
                            start_game.server_auth_sound_enabled = false;
                            start_game.has_server_join_info = false;

                            handler->sendPacket(evt.address, start_game);
                            logPacketSize("StartGamePacket", start_game);
                            // Full hex dump of StartGamePacket payload (without header)
                            {
                                BinaryStream sgDump;
                                start_game.write(sgDump);
                                auto& sgBuf = sgDump.getBuffer();
                                std::cout << "    [hex dump] StartGamePacket payload (" << sgBuf.size() << " bytes):" << std::endl;
                                std::cout << "    ";
                                for (size_t i = 0; i < sgBuf.size(); ++i) {
                                    char h[4]; snprintf(h, sizeof(h), "%02X ", (uint8_t)sgBuf[i]);
                                    std::cout << h;
                                    if ((i + 1) % 32 == 0) std::cout << std::endl << "    ";
                                }
                                std::cout << std::endl;
                            }
                            std::cout << "[>] Sent StartGamePacket" << std::endl;

                            // DIAGNOSTIC: skip companion packets to test if StartGamePacket alone works
                            #if 1
                            // ItemRegistry MUST be sent immediately after StartGamePacket
                            ItemRegistryPacket items;
                            {
                                BinaryStream is;
                                is.writeUnsignedVarInt(0); // 0 items
                                auto& buf = is.getBuffer();
                                items.items_data.assign(buf.begin(), buf.end());
                            }
                            handler->sendPacket(evt.address, items);
                            logPacketSize("ItemRegistryPacket", items);
                            std::cout << "[>] Sent ItemRegistry (empty, 0 items)" << std::endl;

                            // Send BiomeDefinitionList (empty - v800+ format)
                            auto biomes = BiomeDefinitionListPacket::makeEmpty();
                            handler->sendPacket(evt.address, biomes);
                            logPacketSize("BiomeDefinitionListPacket", biomes);
                            std::cout << "[>] Sent BiomeDefinitionList (empty)" << std::endl;

                            // Send AvailableActorIdentifiers (NBT with empty "idlist")
                            AvailableActorIdentifiersPacket actors;
                            {
                                BinaryStream nbt_stream;
                                auto root = NbtTag::makeCompound();
                                // Must have "idlist" key per protocol constraint
                                auto idlist = std::make_shared<NbtTag>(NbtTagType::List);
                                idlist->listTagType = NbtTagType::Compound;
                                // Empty list - 0 entries
                                root->compoundVal["idlist"] = idlist;
                                NbtTag::writeNetworkNbt(nbt_stream, *root);
                                auto& buf = nbt_stream.getBuffer();
                                actors.identifier_list_nbt.assign(buf.begin(), buf.end());
                            }
                            handler->sendPacket(evt.address, actors);
                            logPacketSize("AvailableActorIdentifiersPacket", actors);
                            std::cout << "[>] Sent AvailableActorIdentifiers (empty idlist)" << std::endl;

                            // Send CreativeContent (0 groups, 0 entries - non-null registry)
                            CreativeContentPacket creative;
                            {
                                BinaryStream cs;
                                cs.writeUnsignedVarInt(0); // 0 groups
                                cs.writeUnsignedVarInt(0); // 0 entries
                                auto& buf = cs.getBuffer();
                                creative.items_data.assign(buf.begin(), buf.end());
                            }
                            handler->sendPacket(evt.address, creative);
                            logPacketSize("CreativeContentPacket", creative);
                            std::cout << "[>] Sent CreativeContent (0 groups, 0 entries)" << std::endl;
                            #else
                            std::cout << "[>] SKIPPED all companion packets (diagnostic mode)" << std::endl;
                            #endif

                            // Send UpdateAbilities (Creative mode abilities)
                            {
                                auto abilities_pkt = buildUpdateAbilitiesPacket(1);
                                handler->sendPacket(evt.address, abilities_pkt);
                                std::cout << "[>] Sent UpdateAbilities (Creative perms)" << std::endl;
                            }

                            // Send UpdateAdventureSettings
                            {
                                auto adv_pkt = buildUpdateAdventureSettingsPacket();
                                handler->sendPacket(evt.address, adv_pkt);
                                std::cout << "[>] Sent UpdateAdventureSettings" << std::endl;
                            }

                            // Send UpdateAttributes (health, movement, hunger)
                            {
                                auto attr_pkt = buildUpdateAttributesPacket(1);
                                handler->sendPacket(evt.address, attr_pkt);
                                std::cout << "[>] Sent UpdateAttributes (health, movement, hunger)" << std::endl;
                            }

                            client.state = ClientState::WaitingForChunkRadiusRequest;
                        }
                        break;
                    }

                    // ─── Step 4: Client requests chunk radius ────────
                    case PacketId::RequestChunkRadius: {
                        RequestChunkRadiusPacket req;
                        {
                            ReadOnlyBinaryStream rs(evt.raw_data.data(), evt.raw_data.size());
                            req.read(rs);
                        }
                        std::cout << "    Requested chunk radius: " << req.chunk_radius << std::endl;

                        int32_t granted_radius = std::min(req.chunk_radius, (int32_t)4);

                        ChunkRadiusUpdatedPacket radius_resp;
                        radius_resp.chunk_radius = granted_radius;
                        handler->sendPacket(evt.address, radius_resp);
                        std::cout << "[>] Sent ChunkRadiusUpdated (radius=" << granted_radius << ")" << std::endl;

                        // Send chunks around spawn
                        if (!client.chunks_sent) {
                            // Send flat chunks in a grid around spawn
                            int chunk_count = 0;
                            std::vector<ChunkPos> sent_chunks;
                            for (int cx = -granted_radius; cx <= granted_radius; ++cx) {
                                for (int cz = -granted_radius; cz <= granted_radius; ++cz) {
                                    LevelChunkPacket chunk;
                                    chunk.position = {cx, cz};
                                    chunk.dimension_id = 0;
                                    chunk.client_needs_to_request_subchunks = false;
                                    chunk.sub_chunk_count = 24; // all 24 sub-chunks
                                    chunk.cache_enabled = false;
                                    chunk.serialized_chunk_data = flat_chunk_data;
                                    handler->sendPacket(evt.address, chunk);
                                    sent_chunks.push_back({cx, cz});
                                    ++chunk_count;
                                }
                            }
                            std::cout << "[>] Sent " << chunk_count << " LevelChunkPackets" << std::endl;

                            // Send NetworkChunkPublisherUpdate (defines the center of chunk loading)
                            NetworkChunkPublisherUpdatePacket publisher;
                            publisher.position = {0, 1, 0};
                            publisher.radius = granted_radius * 16;
                            publisher.saved_chunks = std::move(sent_chunks);
                            handler->sendPacket(evt.address, publisher);
                            std::cout << "[>] Sent NetworkChunkPublisherUpdate" << std::endl;

                            client.chunks_sent = true;
                        }

                        // Send PlayStatus: PlayerSpawn
                        PlayStatusPacket spawn_status;
                        spawn_status.status = PlayStatusType::PlayerSpawn;
                        handler->sendPacket(evt.address, spawn_status);
                        std::cout << "[>] Sent PlayStatus(PlayerSpawn)" << std::endl;

                        // Send initial MovePlayer to firmly establish position
                        {
                            MovePlayerPacket mp;
                            mp.runtime_id = {1};
                            mp.position = {0.0f, 1.0f, 0.0f};
                            mp.rotation = {0.0f, 0.0f};
                            mp.head_yaw = 0.0f;
                            mp.mode = MovePlayerMode::Teleport;
                            mp.on_ground = true;
                            mp.riding_runtime_id = {0};
                            mp.teleport_cause = 0;
                            mp.teleport_source_type = 0;
                            mp.tick = 0;
                            handler->sendPacket(evt.address, mp);
                            std::cout << "[>] Sent initial MovePlayer(Teleport) to (0, 1, 0)" << std::endl;
                        }

                        client.state = ClientState::Spawned;
                        break;
                    }

                    // ─── Handle SetLocalPlayerAsInitialized ──────────
                    case PacketId::SetLocalPlayerAsInit: {
                        logMsg("    Player initialized and in-game!");
                        break;
                    }

                    // ─── Handle ServerboundLoadingScreen ─────────────
                    case PacketId::ServerboundLoadingScreen: {
                        ReadOnlyBinaryStream rs(evt.raw_data.data(), evt.raw_data.size());
                        int32_t type = rs.readVarInt();
                        { char buf[80]; snprintf(buf, sizeof(buf), "    Loading screen event: type=%d", type); logMsg(buf); }
                        break;
                    }

                    // ─── Handle PlayerAuthInput ──────────────────────
                    case PacketId::PlayerAuthInput: {
                        client.auth_input_count++;
                        if (evt.raw_data.size() >= 20) {
                            // Read position from raw bytes (always at fixed offset 8-19)
                            float pitch, yaw, px, py, pz;
                            memcpy(&pitch, evt.raw_data.data() + 0, 4);
                            memcpy(&yaw,   evt.raw_data.data() + 4, 4);
                            memcpy(&px,    evt.raw_data.data() + 8, 4);
                            memcpy(&py,    evt.raw_data.data() + 12, 4);
                            memcpy(&pz,    evt.raw_data.data() + 16, 4);

                            // Log position periodically (~every 5 seconds)
                            if (client.auth_input_count % 100 == 1) {
                                char buf[256]; snprintf(buf, sizeof(buf),
                                    "    [tick %llu] pos=(%.2f, %.2f, %.2f) yaw=%.2f pitch=%.2f",
                                    (unsigned long long)client.auth_input_count, px, py, pz, yaw, pitch);
                                logMsg(buf);
                            }
                        }
                        break;
                    }

                    // ─── Handle ClientCacheStatus ────────────────────
                    case PacketId::ClientCacheStatus: {
                        // Client reports cache support - acknowledge silently
                        break;
                    }

                    // ─── Handle NetworkStackLatency (ping) ───────────
                    case PacketId::Ping: {
                        NetworkStackLatencyPacket ping;
                        {
                            ReadOnlyBinaryStream rs(evt.raw_data.data(), evt.raw_data.size());
                            ping.read(rs);
                        }
                        // Echo back with from_server = true
                        NetworkStackLatencyPacket pong;
                        pong.timestamp = ping.timestamp;
                        pong.from_server = true;
                        handler->sendPacket(evt.address, pong);
                        break;
                    }

                    // ─── Handle LevelSoundEvent ──────────────────────
                    case PacketId::LevelSoundEvent: {
                        // Echo back to sender so they hear their own sound
                        LevelSoundEventPacket sound;
                        {
                            ReadOnlyBinaryStream rs(evt.raw_data.data(), evt.raw_data.size());
                            sound.read(rs);
                        }
                        handler->sendPacket(evt.address, sound);
                        break;
                    }

                    // ─── Handle RequestAbility (fly toggle etc.) ─────
                    case PacketId::RequestAbility: {
                        // Client requests to toggle an ability (e.g. flying)
                        if (evt.raw_data.size() >= 1) {
                            ReadOnlyBinaryStream rs(evt.raw_data.data(), evt.raw_data.size());
                            int32_t ability_type = rs.readVarInt();
                            uint8_t value_type = rs.readByte();
                            bool bool_value = false;
                            if (value_type == 1) { // bool type
                                bool_value = rs.readBool();
                            }
                            char buf[128];
                            snprintf(buf, sizeof(buf), "    RequestAbility: type=%d val=%d", ability_type, bool_value ? 1 : 0);
                            logMsg(buf);

                            // If requesting flying ability (type 9)
                            if (ability_type == ABILITY_FLYING) {
                                client.is_flying = bool_value;
                            }

                            // Re-send UpdateAbilities with updated flying state
                            auto abilities_pkt = buildUpdateAbilitiesPacket(1, client.is_flying);
                            handler->sendPacket(evt.address, abilities_pkt);
                        }
                        break;
                    }

                    // ─── Handle InventoryTransaction ─────────────────
                    case PacketId::InventoryTransaction: {
                        break; // Acknowledge silently for now
                    }

                    // ─── Handle ItemStackRequest ─────────────────────
                    case PacketId::ItemStackRequest: {
                        break; // Acknowledge silently for now
                    }

                    // ─── Handle Interact ─────────────────────────────
                    case PacketId::Interact: {
                        if (!evt.raw_data.empty()) {
                            uint8_t action = evt.raw_data[0];
                            if (action == 4) { // OpenInventory
                                logMsg("    Player requested inventory open");
                                ContainerOpenPacket co;
                                co.window_id = 0;  // Player inventory
                                co.type = 255;     // -1 as uint8 = INVENTORY type
                                co.position = {0, 0, 0};
                                co.entity_unique_id = {-1};
                                handler->sendPacket(evt.address, co);
                            }
                        }
                        break;
                    }

                    // ─── Handle Animate (arm swing etc.) ─────────────
                    case PacketId::Animate: {
                        break; // Acknowledge silently
                    }

                    // ─── Handle Text (chat messages) ─────────────────
                    case PacketId::Text: {
                        break; // Acknowledge silently
                    }

                    // ─── Handle PlayerAction (crouch, jump, block break, etc.) ────
                    case PacketId::PlayerAction: {
                        PlayerActionPacket pa;
                        {
                            ReadOnlyBinaryStream rs(evt.raw_data.data(), evt.raw_data.size());
                            pa.read(rs);
                        }
                        int action_int = static_cast<int>(pa.action);

                        // Handle Creative block destruction
                        if (pa.action == PlayerActionType::CreativePlayerDestroyBlock ||
                            pa.action == PlayerActionType::PredictDestroyBlock) {
                            // Send LevelEvent::ParticlesDestroyBlock (2001) for sound + particles
                            LevelEventPacket destroy_evt;
                            destroy_evt.event_id = 2001; // ParticlesDestroyBlock
                            destroy_evt.position = {
                                static_cast<float>(pa.block_position.x),
                                static_cast<float>(pa.block_position.y),
                                static_cast<float>(pa.block_position.z)
                            };
                            // data = block runtime ID (grass for surface, stone below)
                            destroy_evt.data = static_cast<int32_t>(
                                pa.block_position.y == 0 ? GRASS_RUNTIME_ID : STONE_RUNTIME_ID);
                            handler->sendPacket(evt.address, destroy_evt);

                            char buf[128];
                            snprintf(buf, sizeof(buf), "    Block destroyed at (%d, %d, %d)",
                                pa.block_position.x, pa.block_position.y, pa.block_position.z);
                            logMsg(buf);
                        }
                        else if (pa.action == PlayerActionType::StartBreak) {
                            // Send LevelEvent::UpdateBlockCracking (3600)
                            LevelEventPacket crack_evt;
                            crack_evt.event_id = 3600; // StartBlockCracking
                            crack_evt.position = {
                                static_cast<float>(pa.block_position.x),
                                static_cast<float>(pa.block_position.y),
                                static_cast<float>(pa.block_position.z)
                            };
                            crack_evt.data = 65536 / 5; // break time ticks (fast for test)
                            handler->sendPacket(evt.address, crack_evt);
                        }
                        else if (pa.action == PlayerActionType::AbortBreak ||
                                 pa.action == PlayerActionType::StopBreak) {
                            // Send LevelEvent::StopBlockCracking (3601)
                            LevelEventPacket stop_evt;
                            stop_evt.event_id = 3601; // StopBlockCracking
                            stop_evt.position = {
                                static_cast<float>(pa.block_position.x),
                                static_cast<float>(pa.block_position.y),
                                static_cast<float>(pa.block_position.z)
                            };
                            stop_evt.data = 0;
                            handler->sendPacket(evt.address, stop_evt);
                        }
                        else if (pa.action == PlayerActionType::StartFlying) {
                            client.is_flying = true;
                            auto abilities_pkt = buildUpdateAbilitiesPacket(1, true);
                            handler->sendPacket(evt.address, abilities_pkt);
                            logMsg("    Player started flying");
                        }
                        else if (pa.action == PlayerActionType::StopFlying) {
                            client.is_flying = false;
                            auto abilities_pkt = buildUpdateAbilitiesPacket(1, false);
                            handler->sendPacket(evt.address, abilities_pkt);
                            logMsg("    Player stopped flying");
                        }
                        else if (pa.action == PlayerActionType::MissedSwing) {
                            // Send attack-no-damage sound
                            LevelSoundEventPacket sound;
                            sound.sound_id = 42; // AttackNoDamage
                            // Use last known position (approximate)
                            sound.position = {
                                static_cast<float>(pa.block_position.x),
                                static_cast<float>(pa.block_position.y),
                                static_cast<float>(pa.block_position.z)
                            };
                            sound.extra_data = -1;
                            sound.entity_type = "";
                            sound.is_baby_mob = false;
                            sound.is_global = false;
                            handler->sendPacket(evt.address, sound);
                        }
                        break;
                    }

                    // ─── Handle BlockPickRequest (middle-click) ──────
                    case PacketId::BlockPickRequest: {
                        break; // Acknowledge silently
                    }

                    // ─── Handle PlayerEquipment (hotbar select) ──────
                    case PacketId::PlayerEquipment: {
                        break; // Acknowledge silently
                    }

                    // ─── Handle EmoteList ─────────────────────────────
                    case PacketId::EmoteList: {
                        break; // Acknowledge silently
                    }

                    // ─── Handle ContainerClose ────────────────────────
                    case PacketId::ContainerClose: {
                        // Echo back to confirm container is closed
                        if (evt.raw_data.size() >= 2) {
                            ContainerClosePacket cc;
                            cc.window_id = evt.raw_data[0];
                            cc.container_type = evt.raw_data[1];
                            cc.server_initiated = false;
                            handler->sendPacket(evt.address, cc);
                        }
                        break;
                    }

                    case PacketId::Disconnect: {
                        std::cout << "    [!] Client sent Disconnect packet" << std::endl;
                        if (!evt.raw_data.empty()) {
                            ReadOnlyBinaryStream rs(evt.raw_data.data(), evt.raw_data.size());
                            int32_t reason = rs.readVarInt();
                            bool has_message = rs.readBool();
                            std::string message;
                            if (has_message) message = rs.readString();
                            std::cout << "    Disconnect reason: " << reason;
                            if (has_message) std::cout << " message: " << message;
                            std::cout << std::endl;
                        }
                        break;
                    }

                    default:
                        // Log unknown packets with their ID
                        std::cout << "    [?] Unhandled packet ID: "
                                  << static_cast<int>(evt.packet_id)
                                  << " (" << evt.raw_data.size() << " bytes)" << std::endl;
                        break;
                    }
                    break;
                }

                default:
                    break;
            }
        }

        // Send periodic time sync to keep daytime (every ~5 seconds)
        for (auto& [addr, ci] : clients) {
            if (ci.state == ClientState::Spawned) {
                ci.tick_count++;
                if (ci.tick_count - ci.last_time_sync >= 100) {
                    SetTimePacket time_pkt;
                    time_pkt.time = 6000; // Noon
                    handler->sendPacket(ci.address, time_pkt);
                    ci.last_time_sync = ci.tick_count;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      } catch (const std::exception& e) {
          logMsg(std::string("[!] Exception in main loop: ") + e.what());
      } catch (...) {
          logMsg("[!] Unknown exception in main loop");
      }
    }

    handler->shutdown();
    return 0;
}
