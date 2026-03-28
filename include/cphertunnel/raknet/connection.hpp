// =============================================================================
// CpherTunnel - RakNet Connection
// Individual RakNet connection state machine
// =============================================================================
#pragma once

#include "types.hpp"
#include "bitstream.hpp"
#include "reliability.hpp"
#include <vector>
#include <functional>
#include <chrono>
#include <queue>
#include <mutex>

namespace cphertunnel::raknet {

// =============================================================================
// Connection State
// =============================================================================
enum class ConnectionState {
    Disconnected,
    Connecting,       // Handshake in progress
    Connected,        // Fully connected
    Disconnecting,    // Graceful disconnect in progress
};

// =============================================================================
// Offline Message Builders - Handshake packet encoding/decoding
// =============================================================================
namespace offline {

// ----- UnconnectedPing -----
inline std::vector<uint8_t> buildUnconnectedPing(int64_t time, uint64_t client_guid) {
    BitStream bs;
    bs.writeUint8(static_cast<uint8_t>(MessageId::UnconnectedPing));
    bs.writeUint64BE(static_cast<uint64_t>(time));
    bs.writeMagic();
    bs.writeUint64BE(client_guid);
    return bs.toVector();
}

// ----- UnconnectedPong -----
struct UnconnectedPongData {
    int64_t time;
    uint64_t server_guid;
    std::string server_info;
};

inline std::vector<uint8_t> buildUnconnectedPong(int64_t time, uint64_t server_guid,
                                                   const std::string& server_info) {
    BitStream bs;
    bs.writeUint8(static_cast<uint8_t>(MessageId::UnconnectedPong));
    bs.writeUint64BE(static_cast<uint64_t>(time));
    bs.writeUint64BE(server_guid);
    bs.writeMagic();
    bs.writeString(server_info);
    return bs.toVector();
}

inline UnconnectedPongData parseUnconnectedPong(const uint8_t* data, size_t length) {
    BitStream bs(data, length);
    bs.readUint8(); // Skip ID
    UnconnectedPongData pong;
    pong.time = static_cast<int64_t>(bs.readUint64BE());
    pong.server_guid = bs.readUint64BE();
    bs.readMagicAndValidate();
    pong.server_info = bs.readString();
    return pong;
}

// ----- OpenConnectionRequest1 -----
inline std::vector<uint8_t> buildOpenConnectionRequest1(uint16_t mtu_size) {
    BitStream bs;
    bs.writeUint8(static_cast<uint8_t>(MessageId::OpenConnectionRequest1));
    bs.writeMagic();
    bs.writeUint8(RAKNET_PROTOCOL_VERSION);
    // Padding to hit MTU size (total packet size should approximately equal mtu_size)
    size_t current_size = bs.getDataSize();
    size_t padding = (mtu_size > current_size + 28) ? (mtu_size - current_size - 28) : 0; // 28 for IP+UDP headers
    for (size_t i = 0; i < padding; ++i) {
        bs.writeUint8(0x00);
    }
    return bs.toVector();
}

// ----- OpenConnectionReply1 -----
struct OpenConnectionReply1Data {
    uint64_t server_guid;
    bool use_security;
    uint16_t mtu_size;
};

inline std::vector<uint8_t> buildOpenConnectionReply1(uint64_t server_guid, bool use_security,
                                                        uint16_t mtu_size) {
    BitStream bs;
    bs.writeUint8(static_cast<uint8_t>(MessageId::OpenConnectionReply1));
    bs.writeMagic();
    bs.writeUint64BE(server_guid);
    bs.writeUint8(use_security ? 1 : 0);
    bs.writeUint16BE(mtu_size);
    return bs.toVector();
}

inline OpenConnectionReply1Data parseOpenConnectionReply1(const uint8_t* data, size_t length) {
    BitStream bs(data, length);
    bs.readUint8(); // Skip ID
    bs.readMagicAndValidate();
    OpenConnectionReply1Data reply;
    reply.server_guid = bs.readUint64BE();
    reply.use_security = bs.readUint8() != 0;
    reply.mtu_size = bs.readUint16BE();
    return reply;
}

// ----- OpenConnectionRequest2 -----
inline std::vector<uint8_t> buildOpenConnectionRequest2(const SystemAddress& server_addr,
                                                          uint16_t mtu_size, uint64_t client_guid) {
    BitStream bs;
    bs.writeUint8(static_cast<uint8_t>(MessageId::OpenConnectionRequest2));
    bs.writeMagic();
    bs.writeAddress(server_addr);
    bs.writeUint16BE(mtu_size);
    bs.writeUint64BE(client_guid);
    return bs.toVector();
}

// ----- OpenConnectionReply2 -----
struct OpenConnectionReply2Data {
    uint64_t server_guid;
    SystemAddress client_address;
    uint16_t mtu_size;
    bool encryption_enabled;
};

inline std::vector<uint8_t> buildOpenConnectionReply2(uint64_t server_guid,
                                                        const SystemAddress& client_addr,
                                                        uint16_t mtu_size, bool encryption) {
    BitStream bs;
    bs.writeUint8(static_cast<uint8_t>(MessageId::OpenConnectionReply2));
    bs.writeMagic();
    bs.writeUint64BE(server_guid);
    bs.writeAddress(client_addr);
    bs.writeUint16BE(mtu_size);
    bs.writeUint8(encryption ? 1 : 0);
    return bs.toVector();
}

inline OpenConnectionReply2Data parseOpenConnectionReply2(const uint8_t* data, size_t length) {
    BitStream bs(data, length);
    bs.readUint8(); // Skip ID
    bs.readMagicAndValidate();
    OpenConnectionReply2Data reply;
    reply.server_guid = bs.readUint64BE();
    reply.client_address = bs.readAddress();
    reply.mtu_size = bs.readUint16BE();
    reply.encryption_enabled = bs.readUint8() != 0;
    return reply;
}

// ----- ConnectionRequest (connected phase) -----
inline std::vector<uint8_t> buildConnectionRequest(uint64_t client_guid, int64_t time,
                                                      bool use_security = false) {
    BitStream bs;
    bs.writeUint8(static_cast<uint8_t>(MessageId::ConnectionRequest));
    bs.writeUint64BE(client_guid);
    bs.writeUint64BE(static_cast<uint64_t>(time));
    bs.writeUint8(use_security ? 1 : 0);
    return bs.toVector();
}

// ----- ConnectionRequestAccepted (connected phase) -----
inline std::vector<uint8_t> buildConnectionRequestAccepted(const SystemAddress& client_addr,
                                                              uint16_t system_index,
                                                              int64_t request_time, int64_t reply_time) {
    BitStream bs;
    bs.writeUint8(static_cast<uint8_t>(MessageId::ConnectionRequestAccepted));
    bs.writeAddress(client_addr);
    bs.writeUint16BE(system_index);
    
    // 20 system addresses (Bedrock sends UNASSIGNED for most)
    for (int i = 0; i < 20; ++i) {
        bs.writeAddress(SystemAddress::unassigned());
    }
    
    bs.writeUint64BE(static_cast<uint64_t>(request_time));
    bs.writeUint64BE(static_cast<uint64_t>(reply_time));
    return bs.toVector();
}

// ----- NewIncomingConnection (client sends after ConnectionRequestAccepted) -----
inline std::vector<uint8_t> buildNewIncomingConnection(const SystemAddress& server_addr,
                                                         int64_t request_time, int64_t reply_time) {
    BitStream bs;
    bs.writeUint8(static_cast<uint8_t>(MessageId::NewIncomingConnection));
    bs.writeAddress(server_addr);
    
    // 20 system addresses
    for (int i = 0; i < 20; ++i) {
        bs.writeAddress(SystemAddress::unassigned());
    }
    
    bs.writeUint64BE(static_cast<uint64_t>(request_time));
    bs.writeUint64BE(static_cast<uint64_t>(reply_time));
    return bs.toVector();
}

// ----- Connected Ping/Pong -----
inline std::vector<uint8_t> buildConnectedPing(int64_t time) {
    BitStream bs;
    bs.writeUint8(static_cast<uint8_t>(MessageId::ConnectedPing));
    bs.writeUint64BE(static_cast<uint64_t>(time));
    return bs.toVector();
}

inline std::vector<uint8_t> buildConnectedPong(int64_t ping_time, int64_t pong_time) {
    BitStream bs;
    bs.writeUint8(static_cast<uint8_t>(MessageId::ConnectedPong));
    bs.writeUint64BE(static_cast<uint64_t>(ping_time));
    bs.writeUint64BE(static_cast<uint64_t>(pong_time));
    return bs.toVector();
}

// ----- Disconnect -----
inline std::vector<uint8_t> buildDisconnect() {
    BitStream bs;
    bs.writeUint8(static_cast<uint8_t>(MessageId::DisconnectionNotification));
    return bs.toVector();
}

} // namespace offline

// =============================================================================
// RakNetConnection - Manages state for a single RakNet connection
// =============================================================================
class RakNetConnection {
public:
    using GamePacketHandler = std::function<void(std::vector<uint8_t>)>;
    
    RakNetConnection() = default;
    
    RakNetConnection(const SystemAddress& remote, uint64_t guid, uint16_t mtu)
        : remote_address_(remote)
        , remote_guid_({guid, 0})
        , mtu_(mtu)
        , state_(ConnectionState::Connected) {
    }
    
    // =========================================================================
    // Properties
    // =========================================================================
    
    ConnectionState getState() const { return state_; }
    void setState(ConnectionState state) { state_ = state; }
    
    const SystemAddress& getRemoteAddress() const { return remote_address_; }
    void setRemoteAddress(const SystemAddress& addr) { remote_address_ = addr; }
    
    RakNetGUID getRemoteGUID() const { return remote_guid_; }
    void setRemoteGUID(const RakNetGUID& guid) { remote_guid_ = guid; }
    
    uint16_t getMTU() const { return mtu_; }
    void setMTU(uint16_t mtu) { mtu_ = mtu; }
    
    bool isEncryptionEnabled() const { return encryption_enabled_; }
    void setEncryptionEnabled(bool enabled) { encryption_enabled_ = enabled; }
    
    ReliabilityLayer& getReliability() { return reliability_; }
    
    void setGamePacketHandler(GamePacketHandler handler) {
        game_packet_handler_ = std::move(handler);
    }
    
    // =========================================================================
    // Sending
    // =========================================================================
    
    // Send a game packet (0xFE prefixed batch data)
    std::vector<Datagram> sendGamePacket(const std::vector<uint8_t>& data) {
        // Pre-pend 0xFE
        std::vector<uint8_t> payload;
        payload.reserve(1 + data.size());
        payload.push_back(GAME_PACKET_ID);
        payload.insert(payload.end(), data.begin(), data.end());
        
        return sendReliable(std::move(payload));
    }
    
    // Send reliable ordered data
    std::vector<Datagram> sendReliable(std::vector<uint8_t> data, uint8_t channel = 0) {
        auto frames = reliability_.splitMessage(std::move(data),
                                                Reliability::ReliableOrdered,
                                                mtu_, channel);
        return reliability_.buildDatagrams(frames, mtu_);
    }
    
    // Send unreliable data
    std::vector<Datagram> sendUnreliable(std::vector<uint8_t> data) {
        Frame frame = reliability_.createFrame(std::move(data), Reliability::Unreliable);
        return reliability_.buildDatagrams({frame}, mtu_);
    }
    
    // =========================================================================
    // Receiving
    // =========================================================================
    
    // Process a received raw datagram
    void processReceivedDatagram(const uint8_t* data, size_t length) {
        if (length < 1) return;
        
        uint8_t flags = data[0];
        
        // ACK
        if (flags & FlagACK) {
            auto ack = AckNak::decode(data, length);
            reliability_.processAck(ack);
            return;
        }
        
        // NAK
        if (flags & FlagNAK) {
            auto nak = AckNak::decode(data, length);
            auto retransmits = reliability_.processNak(nak);
            for (auto& dg : retransmits) {
                pending_send_.push(dg);
            }
            return;
        }
        
        // Data datagram
        if (flags & FlagValid) {
            Datagram dg = Datagram::decode(data, length);
            reliability_.recordReceivedSequence(dg.sequence_number);
            
            for (auto& frame : dg.frames) {
                auto messages = reliability_.processReceivedFrame(frame);
                for (auto& msg : messages) {
                    handleReceivedMessage(msg);
                }
            }
        }
    }
    
    // Get pending datagrams to send (ACKs, retransmits, etc.)
    std::vector<std::vector<uint8_t>> getPendingOutbound() {
        std::vector<std::vector<uint8_t>> result;
        
        // Build ACK for received sequences
        auto ack = reliability_.buildAck();
        if (!ack.ranges.empty()) {
            result.push_back(ack.encode());
        }
        
        // Queued retransmissions
        while (!pending_send_.empty()) {
            result.push_back(pending_send_.front().encode());
            pending_send_.pop();
        }
        
        return result;
    }
    
    // =========================================================================
    // Ping/Keepalive
    // =========================================================================
    
    std::vector<uint8_t> buildPing() {
        auto now = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        last_ping_time_ = ms;
        return offline::buildConnectedPing(ms);
    }
    
    std::vector<uint8_t> buildPong(int64_t ping_time) {
        auto now = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        return offline::buildConnectedPong(ping_time, ms);
    }
    
    int64_t getLatency() const { return latency_ms_; }
    
private:
    void handleReceivedMessage(const std::vector<uint8_t>& data) {
        if (data.empty()) return;
        
        uint8_t id = data[0];
        
        switch (static_cast<MessageId>(id)) {
            case MessageId::ConnectedPing: {
                if (data.size() >= 9) {
                    int64_t time = static_cast<int64_t>(readInt64BE(data.data() + 1));
                    auto pong = buildPong(time);
                    // Wrap in a reliable frame and queue
                    auto frames = reliability_.splitMessage(std::move(pong),
                                                           Reliability::Unreliable, mtu_);
                    auto dgs = reliability_.buildDatagrams(frames, mtu_);
                    for (auto& dg : dgs) pending_send_.push(dg);
                }
                break;
            }
            
            case MessageId::ConnectedPong: {
                if (data.size() >= 17) {
                    int64_t ping_time = static_cast<int64_t>(readInt64BE(data.data() + 1));
                    auto now = std::chrono::steady_clock::now();
                    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now.time_since_epoch()).count();
                    latency_ms_ = now_ms - ping_time;
                }
                break;
            }
            
            case MessageId::DisconnectionNotification:
                state_ = ConnectionState::Disconnected;
                break;
            
            case MessageId::GamePacket: {
                // Strip 0xFE prefix and deliver game packet
                if (data.size() > 1 && game_packet_handler_) {
                    std::vector<uint8_t> game_data(data.begin() + 1, data.end());
                    game_packet_handler_(std::move(game_data));
                }
                break;
            }
            
            default:
                // Handle ConnectionRequest, NewIncomingConnection etc in the Peer
                if (unhandled_message_handler_) {
                    unhandled_message_handler_(data);
                }
                break;
        }
    }
    
public:
    using MessageHandler = std::function<void(const std::vector<uint8_t>&)>;
    void setUnhandledMessageHandler(MessageHandler h) { unhandled_message_handler_ = std::move(h); }
    
private:
    SystemAddress remote_address_;
    RakNetGUID remote_guid_;
    uint16_t mtu_ = DEFAULT_MTU_SIZE;
    ConnectionState state_ = ConnectionState::Disconnected;
    bool encryption_enabled_ = false;
    
    ReliabilityLayer reliability_;
    std::queue<Datagram> pending_send_;
    
    GamePacketHandler game_packet_handler_;
    MessageHandler unhandled_message_handler_;
    
    int64_t last_ping_time_ = 0;
    int64_t latency_ms_ = 0;
};

} // namespace cphertunnel::raknet
