// =============================================================================
// BedrockProtocol - Connection Handler  
// High-level Bedrock protocol connection managing the full pipeline:
// RakNet → Encryption → Compression → Packet decode/encode
// =============================================================================
#pragma once

#include "../raknet/peer.hpp"
#include "batch_packet.hpp"
#include "../protocol/packets/login_packets.hpp"
#include <functional>
#include <memory>
#include <queue>

namespace bedrock::network {

// =============================================================================
// Connection Event
// =============================================================================
struct ConnectionEvent {
    enum class Type {
        Connected,           // RakNet connection established
        Disconnected,        // Connection lost
        PacketReceived,      // Game packet received
        ServerInfo,          // Server info response (ping)
    };
    
    Type type;
    raknet::SystemAddress address;
    raknet::RakNetGUID guid;
    bedrock::PacketId packet_id = PacketId::Login;
    std::vector<uint8_t> raw_data;
};

// =============================================================================
// BedrockConnection - Full protocol stack for one connection
// =============================================================================
class BedrockConnection {
public:
    using PacketCallback = std::function<void(PacketId, const uint8_t*, size_t)>;
    
    BedrockConnection() = default;
    
    explicit BedrockConnection(const raknet::SystemAddress& remote)
        : remote_address_(remote) {
    }
    
    // =========================================================================
    // Configuration
    // =========================================================================
    
    void setRemoteAddress(const raknet::SystemAddress& addr) { remote_address_ = addr; }
    const raknet::SystemAddress& getRemoteAddress() const { return remote_address_; }
    
    BatchPacketProcessor& getProcessor() { return processor_; }
    
    void setCompressionAlgorithm(CompressionAlgorithm algo) {
        processor_.setCompression(algo);
    }
    
    // =========================================================================
    // Send packets
    // =========================================================================
    
    // Encode a packet for sending (returns the batch payload to send via RakNet)
    std::vector<uint8_t> encodePacket(const bedrock::Packet& packet) {
        return processor_.encodeSingle(packet);
    }
    
    // Encode multiple packets into one batch
    std::vector<uint8_t> encodeBatch(const std::vector<const bedrock::Packet*>& packets) {
        return processor_.encodeBatch(packets);
    }
    
    // =========================================================================
    // Receive packets
    // =========================================================================
    
    // Process received game data (after RakNet 0xFE strip)
    void processGameData(const std::vector<uint8_t>& data, PacketCallback callback) {
        processor_.decode(data, std::move(callback));
    }
    
    // Process game data and return decoded packets
    std::vector<std::pair<PacketId, std::vector<uint8_t>>> processGameData(
            const std::vector<uint8_t>& data) {
        return processor_.decode(data);
    }
    
private:
    raknet::SystemAddress remote_address_;
    BatchPacketProcessor processor_;
};

// =============================================================================
// ConnectionHandler - Manages RakNet peer + Bedrock connections
// Provides a clean high-level API for connecting/listening
// =============================================================================
class ConnectionHandler {
public:
    using EventCallback = std::function<void(const ConnectionEvent&)>;
    
    // =========================================================================
    // Client mode: connect to a server
    // =========================================================================
    
    static std::unique_ptr<ConnectionHandler> createClient() {
        auto handler = std::unique_ptr<ConnectionHandler>(new ConnectionHandler());
        handler->peer_ = std::make_unique<raknet::RakNetPeer>(raknet::PeerMode::Client);
        return handler;
    }
    
    // =========================================================================
    // Server mode: listen for connections
    // =========================================================================
    
    static std::unique_ptr<ConnectionHandler> createServer(uint16_t port,
                                                             const std::string& address = "0.0.0.0") {
        auto handler = std::unique_ptr<ConnectionHandler>(new ConnectionHandler());
        handler->peer_ = std::make_unique<raknet::RakNetPeer>(raknet::PeerMode::Server);
        if (!handler->peer_->start(port, address)) {
            return nullptr;
        }
        return handler;
    }
    
    // =========================================================================
    // Client operations
    // =========================================================================
    
    bool start(uint16_t port = 0) {
        return peer_->start(port);
    }
    
    bool connect(const std::string& ip, uint16_t port) {
        return peer_->connect(ip, port);
    }
    
    void ping(const std::string& ip, uint16_t port) {
        peer_->ping(ip, port);
    }
    
    // =========================================================================
    // Server operations
    // =========================================================================
    
    void setServerInfo(const std::string& motd) {
        peer_->setServerInfo(motd);
    }
    
    void setMaxConnections(uint16_t max) {
        peer_->setMaxConnections(max);
    }
    
    // =========================================================================
    // Sending
    // =========================================================================
    
    // Send a packet to a specific connection
    bool sendPacket(const raknet::SystemAddress& addr, const bedrock::Packet& packet) {
        auto* bc = getBedrockConnection(addr);
        if (!bc) return false;
        
        auto payload = bc->encodePacket(packet);
        return peer_->sendGamePacket(addr, payload);
    }
    
    // Send multiple packets as a batch
    bool sendBatch(const raknet::SystemAddress& addr,
                   const std::vector<const bedrock::Packet*>& packets) {
        auto* bc = getBedrockConnection(addr);
        if (!bc) return false;
        
        auto payload = bc->encodeBatch(packets);
        return peer_->sendGamePacket(addr, payload);
    }
    
    // Broadcast a packet to all connections
    void broadcastPacket(const bedrock::Packet& packet) {
        peer_->forEachConnection([this, &packet](raknet::RakNetConnection& conn) {
            sendPacket(conn.getRemoteAddress(), packet);
        });
    }
    
    // =========================================================================
    // Tick: Process network I/O
    // =========================================================================
    
    std::vector<ConnectionEvent> tick() {
        std::vector<ConnectionEvent> events;
        
        auto raknet_events = peer_->tick();
        
        for (auto& evt : raknet_events) {
            switch (evt.type) {
                case raknet::PeerEvent::Type::Connected: {
                    // Create Bedrock connection wrapper
                    auto bc = std::make_unique<BedrockConnection>(evt.address);
                    bedrock_connections_[evt.address.toString()] = std::move(bc);
                    
                    events.push_back({ConnectionEvent::Type::Connected,
                                      evt.address, evt.guid, PacketId::Login, {}});
                    break;
                }
                
                case raknet::PeerEvent::Type::Disconnected: {
                    bedrock_connections_.erase(evt.address.toString());
                    events.push_back({ConnectionEvent::Type::Disconnected,
                                      evt.address, evt.guid, PacketId::Login, {}});
                    break;
                }
                
                case raknet::PeerEvent::Type::GamePacket: {
                    auto* bc = getBedrockConnection(evt.address);
                    if (bc) {
                        auto decoded = bc->processGameData(evt.data);
                        for (auto& [id, data] : decoded) {
                            events.push_back({ConnectionEvent::Type::PacketReceived,
                                              evt.address, evt.guid, id, std::move(data)});
                        }
                    }
                    break;
                }
                
                case raknet::PeerEvent::Type::UnconnectedPong: {
                    events.push_back({ConnectionEvent::Type::ServerInfo,
                                      evt.address, evt.guid, PacketId::Login,
                                      std::move(evt.data)});
                    break;
                }
            }
        }
        
        return events;
    }
    
    // =========================================================================
    // Connection access
    // =========================================================================
    
    BedrockConnection* getBedrockConnection(const raknet::SystemAddress& addr) {
        auto it = bedrock_connections_.find(addr.toString());
        if (it != bedrock_connections_.end()) return it->second.get();
        return nullptr;
    }
    
    raknet::RakNetPeer& getPeer() { return *peer_; }
    
    void shutdown() {
        peer_->shutdown();
        bedrock_connections_.clear();
    }
    
private:
    ConnectionHandler() = default;
    
    std::unique_ptr<raknet::RakNetPeer> peer_;
    std::unordered_map<std::string, std::unique_ptr<BedrockConnection>> bedrock_connections_;
};

} // namespace bedrock::network
