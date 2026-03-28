// =============================================================================
// CpherTunnel - RakNet Peer
// High-level RakNet peer managing multiple connections and offline messages
// =============================================================================
#pragma once

#include "types.hpp"
#include "bitstream.hpp"
#include "socket.hpp"
#include "reliability.hpp"
#include "connection.hpp"

#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <string>
#include <random>

namespace cphertunnel::raknet {

// =============================================================================
// Peer Mode
// =============================================================================
enum class PeerMode {
    Client,
    Server,
};

// =============================================================================
// Peer Events
// =============================================================================
struct PeerEvent {
    enum class Type {
        Connected,         // New connection established
        Disconnected,      // Connection lost or closed
        GamePacket,        // Received game packet data (after 0xFE strip)
        UnconnectedPong,   // Received server info reply
    };
    
    Type type;
    SystemAddress address;
    RakNetGUID guid;
    std::vector<uint8_t> data;
};

// =============================================================================
// RakNetPeer - Manages all RakNet connections
// =============================================================================
class RakNetPeer {
public:
    explicit RakNetPeer(PeerMode mode = PeerMode::Client)
        : mode_(mode) {
        // Generate random GUID
        std::random_device rd;
        std::mt19937_64 gen(rd());
        local_guid_.g = gen();
    }
    
    ~RakNetPeer() {
        shutdown();
    }
    
    // =========================================================================
    // Lifecycle
    // =========================================================================
    
    bool start(uint16_t port = 0, const std::string& address = "0.0.0.0") {
        if (!socket_.bind(port, address)) {
            return false;
        }
        running_ = true;
        return true;
    }
    
    void shutdown() {
        // Send disconnect to all connections
        for (auto& [addr_str, conn] : connections_) {
            if (conn->getState() == ConnectionState::Connected) {
                sendDisconnect(*conn);
            }
        }
        connections_.clear();
        socket_.close();
        running_ = false;
    }
    
    bool isRunning() const { return running_; }
    uint16_t getPort() const { return socket_.getPort(); }
    RakNetGUID getLocalGUID() const { return local_guid_; }
    
    // =========================================================================
    // Server Configuration
    // =========================================================================
    
    void setMaxConnections(uint16_t max) { max_connections_ = max; }
    void setServerInfo(const std::string& info) { server_info_ = info; }
    
    // =========================================================================
    // Client: Connect to a server
    // =========================================================================
    
    bool connect(const std::string& ip, uint16_t port) {
        if (mode_ != PeerMode::Client) return false;
        
        target_address_ = SystemAddress(ip, port);
        
        // Send OpenConnectionRequest1 with maximum MTU
        auto req1 = offline::buildOpenConnectionRequest1(MAXIMUM_MTU_SIZE);
        socket_.sendTo(req1, target_address_);
        
        connecting_ = true;
        return true;
    }
    
    // =========================================================================
    // Send: Game packet to a connection
    // =========================================================================
    
    bool sendGamePacket(const SystemAddress& addr, const std::vector<uint8_t>& data) {
        auto* conn = getConnection(addr);
        if (!conn || conn->getState() != ConnectionState::Connected) return false;
        
        auto datagrams = conn->sendGamePacket(data);
        for (auto& dg : datagrams) {
            auto encoded = dg.encode();
            conn->getReliability().recordSentDatagram(dg.sequence_number, dg);
            socket_.sendTo(encoded, addr);
        }
        return true;
    }
    
    // Send game packet to all connections
    void broadcastGamePacket(const std::vector<uint8_t>& data) {
        for (auto& [addr_str, conn] : connections_) {
            if (conn->getState() == ConnectionState::Connected) {
                sendGamePacket(conn->getRemoteAddress(), data);
            }
        }
    }
    
    // =========================================================================
    // Tick: Process network I/O, returns events
    // =========================================================================
    
    std::vector<PeerEvent> tick() {
        std::vector<PeerEvent> events;
        
        if (!running_) return events;
        
        // Receive all pending UDP packets
        while (true) {
            auto recv = socket_.receiveFrom();
            if (!recv.success) break;
            
            processIncoming(recv.data.data(), recv.data.size(), recv.from_address, events);
        }
        
        // Process connection maintenance (ACKs, pings, retransmits)
        auto now = std::chrono::steady_clock::now();
        for (auto& [addr_str, conn] : connections_) {
            if (conn->getState() != ConnectionState::Connected) continue;
            
            // Send pending ACKs and retransmissions
            auto pending = conn->getPendingOutbound();
            for (auto& pkt : pending) {
                socket_.sendTo(pkt, conn->getRemoteAddress());
            }
            
            // Periodic ping
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - last_ping_time_).count();
            if (elapsed > 2000) { // Every 2 seconds
                auto ping = conn->buildPing();
                auto frames = conn->getReliability().splitMessage(
                    std::move(ping), Reliability::Unreliable, conn->getMTU());
                auto dgs = conn->getReliability().buildDatagrams(frames, conn->getMTU());
                for (auto& dg : dgs) {
                    auto encoded = dg.encode();
                    socket_.sendTo(encoded, conn->getRemoteAddress());
                }
                last_ping_time_ = now;
            }
            
            // Check for timed-out reliable datagrams (resend after 1s)
            auto retransmits = conn->getReliability().getTimedOutDatagrams(
                std::chrono::milliseconds(1000));
            for (auto& dg : retransmits) {
                auto encoded = dg.encode();
                conn->getReliability().recordSentDatagram(dg.sequence_number, dg);
                socket_.sendTo(encoded, conn->getRemoteAddress());
            }
        }
        
        // Clean up disconnected connections
        for (auto it = connections_.begin(); it != connections_.end();) {
            if (it->second->getState() == ConnectionState::Disconnected) {
                events.push_back({PeerEvent::Type::Disconnected,
                                  it->second->getRemoteAddress(),
                                  it->second->getRemoteGUID(), {}});
                it = connections_.erase(it);
            } else {
                ++it;
            }
        }
        
        return events;
    }
    
    // =========================================================================
    // Ping a server (client mode, for server list)
    // =========================================================================
    
    void ping(const std::string& ip, uint16_t port) {
        SystemAddress addr(ip, port);
        auto now = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        auto ping_pkt = offline::buildUnconnectedPing(ms, local_guid_.g);
        socket_.sendTo(ping_pkt, addr);
    }
    
    // =========================================================================
    // Connection access
    // =========================================================================
    
    RakNetConnection* getConnection(const SystemAddress& addr) {
        auto key = addr.toString();
        auto it = connections_.find(key);
        if (it != connections_.end()) return it->second.get();
        return nullptr;
    }
    
    size_t getConnectionCount() const { return connections_.size(); }
    
    // Iterate connections
    template<typename F>
    void forEachConnection(F func) {
        for (auto& [key, conn] : connections_) {
            func(*conn);
        }
    }
    
private:
    // =========================================================================
    // Incoming packet dispatch
    // =========================================================================
    
    void processIncoming(const uint8_t* data, size_t length,
                         const SystemAddress& from, std::vector<PeerEvent>& events) {
        if (length < 1) return;
        
        uint8_t first_byte = data[0];
        
        // Check if it's an offline message (unconnected)
        if (isOfflineMessage(first_byte, data, length)) {
            processOfflineMessage(first_byte, data, length, from, events);
            return;
        }
        
        // Check if it's ACK/NAK or data datagram for an existing connection
        auto* conn = getConnection(from);
        if (conn) {
            // Set up game packet handler to push events
            conn->setGamePacketHandler([&events, &from, conn](std::vector<uint8_t> game_data) {
                events.push_back({PeerEvent::Type::GamePacket, from,
                                  conn->getRemoteGUID(), std::move(game_data)});
            });
            conn->processReceivedDatagram(data, length);
        }
    }
    
    bool isOfflineMessage(uint8_t id, const uint8_t* data, size_t length) {
        // Offline messages have the MAGIC at a predictable offset
        switch (static_cast<MessageId>(id)) {
            case MessageId::UnconnectedPing:
            case MessageId::UnconnectedPingOpenConn:
                return length >= 1 + 8 + MAGIC_SIZE && validateMagic(data + 9);
            case MessageId::UnconnectedPong:
                return length >= 1 + 8 + 8 + MAGIC_SIZE && validateMagic(data + 17);
            case MessageId::OpenConnectionRequest1:
                return length >= 1 + MAGIC_SIZE && validateMagic(data + 1);
            case MessageId::OpenConnectionReply1:
                return length >= 1 + MAGIC_SIZE && validateMagic(data + 1);
            case MessageId::OpenConnectionRequest2:
                return length >= 1 + MAGIC_SIZE && validateMagic(data + 1);
            case MessageId::OpenConnectionReply2:
                return length >= 1 + MAGIC_SIZE && validateMagic(data + 1);
            case MessageId::IncompatibleProtocolVersion:
                return true;
            default:
                return false;
        }
    }
    
    void processOfflineMessage(uint8_t id, const uint8_t* data, size_t length,
                                const SystemAddress& from, std::vector<PeerEvent>& events) {
        switch (static_cast<MessageId>(id)) {
            // ----- Server handling client requests -----
            case MessageId::UnconnectedPing:
            case MessageId::UnconnectedPingOpenConn: {
                if (mode_ == PeerMode::Server) {
                    auto now = std::chrono::steady_clock::now();
                    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now.time_since_epoch()).count();
                    auto pong = offline::buildUnconnectedPong(ms, local_guid_.g, server_info_);
                    socket_.sendTo(pong, from);
                }
                break;
            }
            
            case MessageId::OpenConnectionRequest1: {
                if (mode_ == PeerMode::Server) {
                    if (connections_.size() >= max_connections_) {
                        // Send NoFreeIncomingConnections
                        BitStream bs;
                        bs.writeUint8(static_cast<uint8_t>(MessageId::NoFreeIncomingConnections));
                        bs.writeMagic();
                        bs.writeUint64BE(local_guid_.g);
                        socket_.sendTo(bs.toVector(), from);
                        break;
                    }
                    
                    // Parse MTU from packet size
                    uint16_t mtu = static_cast<uint16_t>(
                        std::min(static_cast<size_t>(MAXIMUM_MTU_SIZE), length + 28));
                    
                    auto reply = offline::buildOpenConnectionReply1(local_guid_.g, false, mtu);
                    socket_.sendTo(reply, from);
                }
                break;
            }
            
            case MessageId::OpenConnectionRequest2: {
                if (mode_ == PeerMode::Server) {
                    BitStream bs(data, length);
                    bs.readUint8(); // Skip ID
                    bs.readMagicAndValidate();
                    SystemAddress client_bind_addr = bs.readAddress();
                    uint16_t mtu = bs.readUint16BE();
                    uint64_t client_guid = bs.readUint64BE();
                    
                    mtu = std::min(mtu, MAXIMUM_MTU_SIZE);
                    
                    // Create connection
                    auto conn = std::make_unique<RakNetConnection>(from, client_guid, mtu);
                    conn->setState(ConnectionState::Connecting);
                    
                    // Set up handler for ConnectionRequest (connected phase message)
                    conn->setUnhandledMessageHandler(
                        [this, from_addr = from, &events](const std::vector<uint8_t>& msg) {
                            if (msg.empty()) return;
                            if (static_cast<MessageId>(msg[0]) == MessageId::ConnectionRequest) {
                                handleConnectionRequest(from_addr, msg, events);
                            }
                            if (static_cast<MessageId>(msg[0]) == MessageId::NewIncomingConnection) {
                                auto* c = getConnection(from_addr);
                                if (c) {
                                    c->setState(ConnectionState::Connected);
                                    events.push_back({PeerEvent::Type::Connected, from_addr,
                                                      c->getRemoteGUID(), {}});
                                }
                            }
                        });
                    
                    connections_[from.toString()] = std::move(conn);
                    
                    auto reply = offline::buildOpenConnectionReply2(
                        local_guid_.g, from, mtu, false);
                    socket_.sendTo(reply, from);
                }
                break;
            }
            
            // ----- Client handling server replies -----
            case MessageId::OpenConnectionReply1: {
                if (mode_ == PeerMode::Client && connecting_) {
                    auto reply = offline::parseOpenConnectionReply1(data, length);
                    uint16_t mtu = std::min(reply.mtu_size, MAXIMUM_MTU_SIZE);
                    
                    auto req2 = offline::buildOpenConnectionRequest2(
                        target_address_, mtu, local_guid_.g);
                    socket_.sendTo(req2, from);
                }
                break;
            }
            
            case MessageId::OpenConnectionReply2: {
                if (mode_ == PeerMode::Client && connecting_) {
                    auto reply = offline::parseOpenConnectionReply2(data, length);
                    uint16_t mtu = std::min(reply.mtu_size, MAXIMUM_MTU_SIZE);
                    
                    // Create connection
                    auto conn = std::make_unique<RakNetConnection>(from, reply.server_guid, mtu);
                    conn->setState(ConnectionState::Connecting);
                    
                    conn->setUnhandledMessageHandler(
                        [this, from_addr = from, &events](const std::vector<uint8_t>& msg) {
                            if (msg.empty()) return;
                            if (static_cast<MessageId>(msg[0]) == MessageId::ConnectionRequestAccepted) {
                                auto* c = getConnection(from_addr);
                                if (c) {
                                    c->setState(ConnectionState::Connected);
                                    connecting_ = false;
                                    
                                    // Send NewIncomingConnection
                                    auto now = std::chrono::steady_clock::now();
                                    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                        now.time_since_epoch()).count();
                                    auto nic = offline::buildNewIncomingConnection(from_addr, ms, ms);
                                    auto dgs = c->sendReliable(std::move(nic));
                                    for (auto& dg : dgs) {
                                        auto encoded = dg.encode();
                                        c->getReliability().recordSentDatagram(dg.sequence_number, dg);
                                        socket_.sendTo(encoded, from_addr);
                                    }
                                    
                                    events.push_back({PeerEvent::Type::Connected, from_addr,
                                                      c->getRemoteGUID(), {}});
                                }
                            }
                        });
                    
                    connections_[from.toString()] = std::move(conn);
                    
                    // Send ConnectionRequest
                    auto now = std::chrono::steady_clock::now();
                    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now.time_since_epoch()).count();
                    auto cr = offline::buildConnectionRequest(local_guid_.g, ms);
                    
                    auto* c = getConnection(from);
                    if (c) {
                        auto dgs = c->sendReliable(std::move(cr));
                        for (auto& dg : dgs) {
                            auto encoded = dg.encode();
                            c->getReliability().recordSentDatagram(dg.sequence_number, dg);
                            socket_.sendTo(encoded, from);
                        }
                    }
                }
                break;
            }
            
            case MessageId::UnconnectedPong: {
                auto pong = offline::parseUnconnectedPong(data, length);
                events.push_back({PeerEvent::Type::UnconnectedPong, from,
                                  {pong.server_guid, 0},
                                  std::vector<uint8_t>(pong.server_info.begin(),
                                                        pong.server_info.end())});
                break;
            }
            
            case MessageId::IncompatibleProtocolVersion: {
                connecting_ = false;
                events.push_back({PeerEvent::Type::Disconnected, from, {}, {}});
                break;
            }
            
            default:
                break;
        }
    }
    
    void handleConnectionRequest(const SystemAddress& from, const std::vector<uint8_t>& data,
                                  std::vector<PeerEvent>& events) {
        if (data.size() < 18) return;
        
        BitStream bs(data.data(), data.size());
        bs.readUint8(); // Skip ID
        uint64_t client_guid = bs.readUint64BE();
        int64_t request_time = static_cast<int64_t>(bs.readUint64BE());
        
        auto now = std::chrono::steady_clock::now();
        auto reply_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        
        auto accepted = offline::buildConnectionRequestAccepted(
            from, 0, request_time, reply_time);
        
        auto* conn = getConnection(from);
        if (conn) {
            auto dgs = conn->sendReliable(std::move(accepted));
            for (auto& dg : dgs) {
                auto encoded = dg.encode();
                conn->getReliability().recordSentDatagram(dg.sequence_number, dg);
                socket_.sendTo(encoded, from);
            }
        }
    }
    
    void sendDisconnect(RakNetConnection& conn) {
        auto disc = offline::buildDisconnect();
        auto dgs = conn.sendReliable(std::move(disc));
        for (auto& dg : dgs) {
            auto encoded = dg.encode();
            socket_.sendTo(encoded, conn.getRemoteAddress());
        }
    }
    
    // =========================================================================
    // State
    // =========================================================================
    
    PeerMode mode_;
    UDPSocket socket_;
    RakNetGUID local_guid_;
    bool running_ = false;
    
    // Server state
    uint16_t max_connections_ = 20;
    std::string server_info_;
    
    // Client state
    bool connecting_ = false;
    SystemAddress target_address_;
    
    // Connections keyed by address string
    std::unordered_map<std::string, std::unique_ptr<RakNetConnection>> connections_;
    
    // Ping timing
    std::chrono::steady_clock::time_point last_ping_time_ = std::chrono::steady_clock::now();
};

} // namespace cphertunnel::raknet
