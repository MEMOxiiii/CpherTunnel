// =============================================================================
// Example: Basic Bedrock client - connects to a server 
// Demonstrates: RakNet handshake → packet receiving → response handling
// =============================================================================

#include "cphertunnel/cphertunnel.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char* argv[]) {
    std::string server_ip = "127.0.0.1";
    uint16_t server_port = 19132;
    
    if (argc >= 2) server_ip = argv[1];
    if (argc >= 3) server_port = static_cast<uint16_t>(std::atoi(argv[2]));
    
    std::cout << "Connecting to " << server_ip << ":" << server_port << "..." << std::endl;
    
    // Create connection handler (client mode)
    auto handler = cphertunnel::network::ConnectionHandler::createClient();
    if (!handler->start()) {
        std::cerr << "Failed to start network" << std::endl;
        return 1;
    }
    
    if (!handler->connect(server_ip, server_port)) {
        std::cerr << "Failed to initiate connection" << std::endl;
        return 1;
    }
    
    bool connected = false;
    auto start = std::chrono::steady_clock::now();
    cphertunnel::raknet::SystemAddress server_addr;
    
    while (true) {
        auto events = handler->tick();
        
        for (auto& evt : events) {
            switch (evt.type) {
                case cphertunnel::network::ConnectionEvent::Type::Connected: {
                    std::cout << "Connected to server!" << std::endl;
                    connected = true;
                    server_addr = evt.address;
                    
                    // Send RequestNetworkSettings
                    cphertunnel::RequestNetworkSettingsPacket req;
                    req.client_network_version = cphertunnel::PROTOCOL_VERSION;
                    handler->sendPacket(server_addr, req);
                    std::cout << "Sent RequestNetworkSettings (protocol=" 
                              << cphertunnel::PROTOCOL_VERSION << ")" << std::endl;
                    break;
                }
                
                case cphertunnel::network::ConnectionEvent::Type::Disconnected: {
                    std::cout << "Disconnected from server" << std::endl;
                    handler->shutdown();
                    return 0;
                }
                
                case cphertunnel::network::ConnectionEvent::Type::PacketReceived: {
                    std::cout << "Received packet: " 
                              << cphertunnel::packetIdToString(evt.packet_id) 
                              << " (" << static_cast<int>(evt.packet_id) << ")"
                              << " [" << evt.raw_data.size() << " bytes]" << std::endl;
                    
                    // Handle specific packets
                    switch (evt.packet_id) {
                        case cphertunnel::PacketId::NetworkSettings: {
                            cphertunnel::NetworkSettingsPacket settings;
                            settings.deserialize(evt.raw_data.data(), evt.raw_data.size());
                            
                            std::cout << "  Compression threshold: " 
                                      << settings.compression_threshold << std::endl;
                            std::cout << "  Compression algorithm: " 
                                      << static_cast<int>(settings.compression_algorithm) << std::endl;
                            
                            // Apply compression
                            auto* bc = handler->getConnection(server_addr);
                            if (bc) {
                                bc->setCompressionAlgorithm(settings.compression_algorithm);
                            }
                            
                            std::cout << "  → Would send Login packet next..." << std::endl;
                            break;
                        }
                        
                        case cphertunnel::PacketId::PlayStatus: {
                            cphertunnel::PlayStatusPacket status;
                            status.deserialize(evt.raw_data.data(), evt.raw_data.size());
                            
                            const char* status_str = "Unknown";
                            switch (static_cast<int>(status.status)) {
                                case 0: status_str = "Login Success"; break;
                                case 1: status_str = "Failed (client)"; break;
                                case 2: status_str = "Failed (server)"; break;
                                case 3: status_str = "Player Spawn"; break;
                                case 4: status_str = "Failed (edition mismatch)"; break;
                                case 5: status_str = "Failed (vanilla mismatch)"; break;
                            }
                            std::cout << "  Status: " << status_str << std::endl;
                            break;
                        }
                        
                        case cphertunnel::PacketId::Disconnect: {
                            cphertunnel::DisconnectPacket disc;
                            disc.deserialize(evt.raw_data.data(), evt.raw_data.size());
                            
                            std::cout << "  Reason: " << disc.message << std::endl;
                            break;
                        }
                        
                        default:
                            break;
                    }
                    break;
                }
                
                default:
                    break;
            }
        }
        
        // Timeout check
        auto elapsed = std::chrono::steady_clock::now() - start;
        if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() > 30) {
            std::cout << "Connection timeout" << std::endl;
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    handler->shutdown();
    return 0;
}
