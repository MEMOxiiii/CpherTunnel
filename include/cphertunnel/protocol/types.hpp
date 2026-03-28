// =============================================================================
// CpherTunnel - Core Types
// Protocol Version: 944 (Minecraft Bedrock 1.26.1)
// =============================================================================
#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <memory>
#include <optional>
#include <variant>
#include <functional>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <chrono>

namespace cphertunnel {

// ─── Protocol Constants ──────────────────────────────────────────
static constexpr int32_t  PROTOCOL_VERSION          = 944;
static constexpr const char* MINECRAFT_VERSION_STRING = "1.26.1";
static constexpr uint64_t RAKNET_MAGIC[] = {0x00ffff00fefefefe, 0xfdfdfdfd12345678};
static constexpr uint8_t  RAKNET_MAGIC_BYTES[] = {
    0x00, 0xff, 0xff, 0x00, 0xfe, 0xfe, 0xfe, 0xfe,
    0xfd, 0xfd, 0xfd, 0xfd, 0x12, 0x34, 0x56, 0x78
};
static constexpr size_t   RAKNET_MAGIC_SIZE         = 16;
static constexpr uint8_t  RAKNET_PROTOCOL_VERSION   = 11;
static constexpr uint8_t  GAME_PACKET_ID            = 0xFE; // ID_USER_PACKET_MINECRAFT
static constexpr size_t   MAX_PACKET_SIZE           = 0xA00000; // 10 MB
static constexpr uint32_t PACKET_ID_MASK            = 0x3FF;    // 10-bit packet ID

// ─── Forward Declarations ────────────────────────────────────────
class BinaryStream;
class ReadOnlyBinaryStream;
class Packet;

// ─── Basic Types ─────────────────────────────────────────────────
using byte_t = uint8_t;
using ByteBuffer = std::vector<byte_t>;

struct Vec3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;
};

struct Vec2 {
    float x = 0.0f, y = 0.0f;
};

struct BlockPos {
    int32_t x = 0, y = 0, z = 0;
};

struct NetworkBlockPos {
    int32_t x = 0;
    uint32_t y = 0;
    int32_t z = 0;
};

struct ChunkPos {
    int32_t x = 0, z = 0;
};

struct UUID {
    uint64_t most_significant = 0;
    uint64_t least_significant = 0;

    bool operator==(const UUID& o) const {
        return most_significant == o.most_significant &&
               least_significant == o.least_significant;
    }
    bool operator!=(const UUID& o) const { return !(*this == o); }
};

struct ActorUniqueID {
    int64_t id = 0;
    bool operator==(const ActorUniqueID& o) const { return id == o.id; }
};

struct ActorRuntimeID {
    uint64_t id = 0;
    bool operator==(const ActorRuntimeID& o) const { return id == o.id; }
};

// ─── Enums ───────────────────────────────────────────────────────
enum class GameType : int32_t {
    Survival  = 0,
    Creative  = 1,
    Adventure = 2,
    Spectator = 6,
    Default   = 5
};

enum class Difficulty : int32_t {
    Peaceful = 0,
    Easy     = 1,
    Normal   = 2,
    Hard     = 3
};

enum class Dimension : int32_t {
    Overworld = 0,
    Nether    = 1,
    TheEnd    = 2
};

enum class InputMode : int32_t {
    Undefined           = 0,
    Mouse               = 1,
    Touch               = 2,
    GamePad             = 3,
    MotionController    = 4
};

enum class DeviceOS : int32_t {
    Unknown       = 0,
    Android       = 1,
    iOS           = 2,
    OSX           = 3,
    FireOS        = 4,
    GearVR        = 5,
    Hololens      = 6,
    Windows10     = 7,
    Win32         = 8,
    Dedicated     = 9,
    tvOS          = 10,
    PlayStation   = 11,
    Nintendo      = 12,
    Xbox          = 13,
    WindowsPhone  = 14,
    Linux         = 15
};

enum class SubClientId : uint8_t {
    PrimaryClient = 0,
    Client1       = 1,
    Client2       = 2,
    Client3       = 3
};

enum class Compressibility : uint8_t {
    Compressible   = 0,
    Incompressible = 1
};

enum class CompressionAlgorithm : uint16_t {
    ZLib   = 0,
    Snappy = 1,
    None   = 0xFFFF
};

enum class NetworkPeerReliability : uint8_t {
    Reliable             = 0,
    ReliableOrdered      = 1,
    Unreliable           = 2,
    UnreliableSequenced  = 3
};

// ─── Packet Header ───────────────────────────────────────────────
struct PacketHeader {
    uint32_t    packet_id       = 0;
    SubClientId sender_subclient = SubClientId::PrimaryClient;
    SubClientId target_subclient = SubClientId::PrimaryClient;

    uint32_t encode() const {
        uint32_t val = (packet_id & PACKET_ID_MASK);
        val |= (static_cast<uint32_t>(sender_subclient) << 10);
        val |= (static_cast<uint32_t>(target_subclient) << 12);
        return val;
    }

    static PacketHeader decode(uint32_t val) {
        PacketHeader h;
        h.packet_id        = val & PACKET_ID_MASK;
        h.sender_subclient = static_cast<SubClientId>((val >> 10) & 0x3);
        h.target_subclient = static_cast<SubClientId>((val >> 12) & 0x3);
        return h;
    }
};

} // namespace cphertunnel
