// =============================================================================
// CpherTunnel - RakNet Core Types
// Protocol-level constants, message IDs, reliability types
// =============================================================================
#pragma once

#include <cstdint>
#include <cstring>
#include <array>
#include <string>
#include <sstream>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

namespace cphertunnel::raknet {

// =============================================================================
// Constants
// =============================================================================
static constexpr uint8_t RAKNET_PROTOCOL_VERSION = 11;
static constexpr uint16_t MAXIMUM_MTU_SIZE = 1600;
static constexpr uint16_t MINIMUM_MTU_SIZE = 400;
static constexpr uint16_t DEFAULT_MTU_SIZE = 1400;
static constexpr uint32_t BITSTREAM_STACK_ALLOCATION_SIZE = 256;

static constexpr uint8_t OFFLINE_MESSAGE_MAGIC[] = {
    0x00, 0xFF, 0xFF, 0x00,
    0xFE, 0xFE, 0xFE, 0xFE,
    0xFD, 0xFD, 0xFD, 0xFD,
    0x12, 0x34, 0x56, 0x78
};
static constexpr size_t MAGIC_SIZE = 16;

// Bedrock game packet prefix
static constexpr uint8_t GAME_PACKET_ID = 0xFE;

// =============================================================================
// RakNet Message IDs
// =============================================================================
enum class MessageId : uint8_t {
    ConnectedPing               = 0x00,
    UnconnectedPing             = 0x01,
    UnconnectedPingOpenConn     = 0x02,
    ConnectedPong               = 0x03,
    OpenConnectionRequest1      = 0x05,
    OpenConnectionReply1        = 0x06,
    OpenConnectionRequest2      = 0x07,
    OpenConnectionReply2        = 0x08,
    ConnectionRequest           = 0x09,
    ConnectionRequestAccepted   = 0x10,
    ConnectionAttemptFailed     = 0x11,
    AlreadyConnected            = 0x12,
    NewIncomingConnection       = 0x13,
    NoFreeIncomingConnections   = 0x14,
    DisconnectionNotification   = 0x15,
    ConnectionLost              = 0x16,
    ConnectionBanned            = 0x17,
    InvalidPassword             = 0x18,
    IncompatibleProtocolVersion = 0x19,
    UnconnectedPong             = 0x1C,
    GamePacket                  = 0xFE,
};

// =============================================================================
// Datagram Header Flags
// =============================================================================
enum DatagramFlag : uint8_t {
    // High nibble flags
    FlagValid           = 0x80, // Bit 7: valid datagram (always set for data)
    FlagACK             = 0x40, // Bit 6: acknowledgement
    FlagNAK             = 0x20, // Bit 5: negative acknowledgement
    FlagHasBAndAS       = 0x10, // Bit 4: has B and AS fields (continuous send)
    
    // A datagram header is:
    // - 0x80 | flags for data frames
    // - 0xC0 for ACK
    // - 0xA0 for NAK
};

// =============================================================================
// Reliability Types (3-bit values, 0-7)
// =============================================================================
enum class Reliability : uint8_t {
    Unreliable             = 0,
    UnreliableSequenced    = 1,
    Reliable               = 2,
    ReliableOrdered        = 3,
    ReliableSequenced      = 4,
    UnreliableWithAck      = 5,
    ReliableWithAck        = 6,
    ReliableOrderedWithAck = 7,
};

inline bool isReliable(Reliability r) {
    return r == Reliability::Reliable ||
           r == Reliability::ReliableOrdered ||
           r == Reliability::ReliableSequenced ||
           r == Reliability::ReliableWithAck ||
           r == Reliability::ReliableOrderedWithAck;
}

inline bool isOrdered(Reliability r) {
    return r == Reliability::ReliableOrdered ||
           r == Reliability::ReliableOrderedWithAck ||
           r == Reliability::UnreliableSequenced ||
           r == Reliability::ReliableSequenced;
}

inline bool isSequenced(Reliability r) {
    return r == Reliability::UnreliableSequenced ||
           r == Reliability::ReliableSequenced;
}

inline bool hasAckReceipt(Reliability r) {
    return r == Reliability::UnreliableWithAck ||
           r == Reliability::ReliableWithAck ||
           r == Reliability::ReliableOrderedWithAck;
}

// =============================================================================
// Packet Priority
// =============================================================================
enum class PacketPriority : uint8_t {
    Immediate = 0,
    High      = 1,
    Medium    = 2,
    Low       = 3,
};

// =============================================================================
// RakNet GUID
// =============================================================================
struct RakNetGUID {
    uint64_t g = 0;
    uint16_t system_index = 0xFFFF;
    
    bool operator==(const RakNetGUID& other) const { return g == other.g; }
    bool operator!=(const RakNetGUID& other) const { return g != other.g; }
    
    static RakNetGUID unassigned() {
        RakNetGUID guid;
        guid.g = 0;
        return guid;
    }
};

// =============================================================================
// System Address (IPv4/IPv6)
// =============================================================================
struct SystemAddress {
    sockaddr_storage address{};
    uint16_t debug_port = 0;
    uint16_t system_index = 0xFFFF;
    
    SystemAddress() {
        std::memset(&address, 0, sizeof(address));
    }
    
    SystemAddress(const std::string& ip, uint16_t port) {
        std::memset(&address, 0, sizeof(address));
        setIPv4(ip, port);
    }
    
    void setIPv4(const std::string& ip, uint16_t port) {
        auto* addr4 = reinterpret_cast<sockaddr_in*>(&address);
        addr4->sin_family = AF_INET;
        addr4->sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &addr4->sin_addr);
        debug_port = port;
    }
    
    uint16_t getPort() const {
        auto* addr4 = reinterpret_cast<const sockaddr_in*>(&address);
        if (addr4->sin_family == AF_INET) {
            return ntohs(addr4->sin_port);
        }
        auto* addr6 = reinterpret_cast<const sockaddr_in6*>(&address);
        return ntohs(addr6->sin6_port);
    }
    
    std::string getIP() const {
        char buf[INET6_ADDRSTRLEN] = {};
        auto* addr4 = reinterpret_cast<const sockaddr_in*>(&address);
        if (addr4->sin_family == AF_INET) {
            inet_ntop(AF_INET, &addr4->sin_addr, buf, sizeof(buf));
        } else {
            auto* addr6 = reinterpret_cast<const sockaddr_in6*>(&address);
            inet_ntop(AF_INET6, &addr6->sin6_addr, buf, sizeof(buf));
        }
        return std::string(buf);
    }
    
    std::string toString() const {
        return getIP() + ":" + std::to_string(getPort());
    }
    
    bool operator==(const SystemAddress& other) const {
        // Compare family and address
        auto* a = reinterpret_cast<const sockaddr_in*>(&address);
        auto* b = reinterpret_cast<const sockaddr_in*>(&other.address);
        if (a->sin_family != b->sin_family) return false;
        if (a->sin_family == AF_INET) {
            return a->sin_addr.s_addr == b->sin_addr.s_addr &&
                   a->sin_port == b->sin_port;
        }
        return false; // Simple comparison for now
    }
    
    bool operator!=(const SystemAddress& other) const {
        return !(*this == other);
    }
    
    static SystemAddress unassigned() {
        SystemAddress addr;
        auto* a4 = reinterpret_cast<sockaddr_in*>(&addr.address);
        a4->sin_family = AF_INET;
        a4->sin_addr.s_addr = INADDR_ANY;
        a4->sin_port = 0;
        return addr;
    }
    
    // Write system address in RakNet format (big-endian IP, big-endian port)
    void writeTo(std::vector<uint8_t>& buffer) const {
        auto* addr4 = reinterpret_cast<const sockaddr_in*>(&address);
        if (addr4->sin_family == AF_INET) {
            buffer.push_back(4); // IPv4 indicator
            // IP bytes inverted (RakNet convention: ~byte for each)
            uint32_t ip = addr4->sin_addr.s_addr;
            auto* bytes = reinterpret_cast<const uint8_t*>(&ip);
            buffer.push_back(~bytes[0]);
            buffer.push_back(~bytes[1]);
            buffer.push_back(~bytes[2]);
            buffer.push_back(~bytes[3]);
            // Port big-endian
            uint16_t port = ntohs(addr4->sin_port);
            buffer.push_back(static_cast<uint8_t>(port >> 8));
            buffer.push_back(static_cast<uint8_t>(port & 0xFF));
        }
    }
    
    static SystemAddress readFrom(const uint8_t* data, size_t& offset) {
        SystemAddress addr;
        uint8_t version = data[offset++];
        if (version == 4) {
            auto* addr4 = reinterpret_cast<sockaddr_in*>(&addr.address);
            addr4->sin_family = AF_INET;
            auto* bytes = reinterpret_cast<uint8_t*>(&addr4->sin_addr.s_addr);
            bytes[0] = ~data[offset++];
            bytes[1] = ~data[offset++];
            bytes[2] = ~data[offset++];
            bytes[3] = ~data[offset++];
            uint16_t port = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
            offset += 2;
            addr4->sin_port = htons(port);
            addr.debug_port = port;
        }
        return addr;
    }
};

// =============================================================================
// Utility: read/write 3-byte little-endian (uint24_t for sequence numbers)
// =============================================================================
inline uint32_t readUint24LE(const uint8_t* data) {
    return static_cast<uint32_t>(data[0]) |
           (static_cast<uint32_t>(data[1]) << 8) |
           (static_cast<uint32_t>(data[2]) << 16);
}

inline void writeUint24LE(uint8_t* data, uint32_t value) {
    data[0] = static_cast<uint8_t>(value & 0xFF);
    data[1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    data[2] = static_cast<uint8_t>((value >> 16) & 0xFF);
}

inline uint16_t readUint16BE(const uint8_t* data) {
    return (static_cast<uint16_t>(data[0]) << 8) | data[1];
}

inline void writeUint16BE(uint8_t* data, uint16_t value) {
    data[0] = static_cast<uint8_t>((value >> 8) & 0xFF);
    data[1] = static_cast<uint8_t>(value & 0xFF);
}

inline uint64_t readInt64BE(const uint8_t* data) {
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i) {
        v = (v << 8) | data[i];
    }
    return v;
}

inline void writeInt64BE(uint8_t* data, uint64_t value) {
    for (int i = 7; i >= 0; --i) {
        data[i] = static_cast<uint8_t>(value & 0xFF);
        value >>= 8;
    }
}

// Magic validation
inline bool validateMagic(const uint8_t* data) {
    return std::memcmp(data, OFFLINE_MESSAGE_MAGIC, MAGIC_SIZE) == 0;
}

} // namespace cphertunnel::raknet
