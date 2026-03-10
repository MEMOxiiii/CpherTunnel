// =============================================================================
// Example: Ping a Bedrock server and display its information
// =============================================================================

#include <bedrock/bedrock_protocol.hpp>
#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char* argv[]) {
    std::string server_ip = "127.0.0.1";
    uint16_t server_port = 19132;
    
    if (argc >= 2) server_ip = argv[1];
    if (argc >= 3) server_port = static_cast<uint16_t>(std::atoi(argv[2]));
    
    std::cout << "Pinging " << server_ip << ":" << server_port << "..." << std::endl;
    
    // Create client peer
    bedrock::raknet::RakNetPeer peer(bedrock::raknet::PeerMode::Client);
    if (!peer.start(0)) {
        std::cerr << "Failed to start RakNet peer" << std::endl;
        return 1;
    }
    
    // Send ping
    peer.ping(server_ip, server_port);
    
    // Wait for response
    bool got_response = false;
    auto start = std::chrono::steady_clock::now();
    
    while (!got_response) {
        auto events = peer.tick();
        for (auto& evt : events) {
            if (evt.type == bedrock::raknet::PeerEvent::Type::UnconnectedPong) {
                std::string info(evt.data.begin(), evt.data.end());
                std::cout << "\nServer Response:" << std::endl;
                std::cout << "  GUID: " << evt.guid.g << std::endl;
                std::cout << "  Info: " << info << std::endl;
                
                // Parse MCPE motd format:
                // MCPE;ServerName;ProtocolVersion;VersionName;Players;MaxPlayers;...
                // Split by semicolons
                size_t pos = 0;
                int field = 0;
                while (pos < info.size()) {
                    size_t next = info.find(';', pos);
                    if (next == std::string::npos) next = info.size();
                    std::string value = info.substr(pos, next - pos);
                    
                    switch (field) {
                        case 0: std::cout << "  Edition: " << value << std::endl; break;
                        case 1: std::cout << "  MOTD: " << value << std::endl; break;
                        case 2: std::cout << "  Protocol: " << value << std::endl; break;
                        case 3: std::cout << "  Version: " << value << std::endl; break;
                        case 4: std::cout << "  Players: " << value; break;
                        case 5: std::cout << "/" << value << std::endl; break;
                        case 7: std::cout << "  World: " << value << std::endl; break;
                        case 8: std::cout << "  Gamemode: " << value << std::endl; break;
                    }
                    
                    pos = next + 1;
                    field++;
                }
                
                got_response = true;
            }
        }
        
        auto elapsed = std::chrono::steady_clock::now() - start;
        if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() > 5) {
            std::cout << "Timeout - no response from server" << std::endl;
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    peer.shutdown();
    return got_response ? 0 : 1;
}
