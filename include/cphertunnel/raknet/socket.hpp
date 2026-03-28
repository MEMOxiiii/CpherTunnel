// =============================================================================
// CpherTunnel - RakNet UDP Socket
// Cross-platform UDP socket abstraction
// =============================================================================
#pragma once

#include "types.hpp"
#include <vector>
#include <functional>
#include <stdexcept>
#include <cstring>

#ifdef _WIN32
// Windows socket types already included in types.hpp (WinSock2)
using SocketHandle = SOCKET;
static constexpr SocketHandle INVALID_SOCK = INVALID_SOCKET;
#else
using SocketHandle = int;
static constexpr SocketHandle INVALID_SOCK = -1;
#endif

namespace cphertunnel::raknet {

// =============================================================================
// WinsockInit - RAII Winsock initializer (Windows only)
// =============================================================================
#ifdef _WIN32
class WinsockInit {
public:
    WinsockInit() {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
    }
    ~WinsockInit() { WSACleanup(); }
    WinsockInit(const WinsockInit&) = delete;
    WinsockInit& operator=(const WinsockInit&) = delete;
};
#endif

// =============================================================================
// Received datagram
// =============================================================================
struct RecvResult {
    std::vector<uint8_t> data;
    SystemAddress from_address;
    bool success = false;
};

// =============================================================================
// UDPSocket - Cross platform UDP socket
// =============================================================================
class UDPSocket {
public:
    UDPSocket() = default;
    
    ~UDPSocket() {
        close();
    }
    
    UDPSocket(const UDPSocket&) = delete;
    UDPSocket& operator=(const UDPSocket&) = delete;
    
    UDPSocket(UDPSocket&& other) noexcept 
        : socket_(other.socket_), bound_port_(other.bound_port_) {
        other.socket_ = INVALID_SOCK;
    }
    
    UDPSocket& operator=(UDPSocket&& other) noexcept {
        if (this != &other) {
            close();
            socket_ = other.socket_;
            bound_port_ = other.bound_port_;
            other.socket_ = INVALID_SOCK;
        }
        return *this;
    }
    
    // =========================================================================
    // Lifecycle
    // =========================================================================
    
    bool bind(uint16_t port, const std::string& address = "0.0.0.0") {
#ifdef _WIN32
        static WinsockInit wsa_init;
#endif
        socket_ = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (socket_ == INVALID_SOCK) return false;
        
        // Set non-blocking
        setNonBlocking(true);
        
        // Set receive buffer size
        int buf_size = 1024 * 1024; // 1MB
        setsockopt(socket_, SOL_SOCKET, SO_RCVBUF,
                   reinterpret_cast<const char*>(&buf_size), sizeof(buf_size));
        setsockopt(socket_, SOL_SOCKET, SO_SNDBUF,
                   reinterpret_cast<const char*>(&buf_size), sizeof(buf_size));
        
        // Reuse address
        int reuse = 1;
        setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR,
                   reinterpret_cast<const char*>(&reuse), sizeof(reuse));
        
        sockaddr_in bind_addr{};
        bind_addr.sin_family = AF_INET;
        bind_addr.sin_port = htons(port);
        inet_pton(AF_INET, address.c_str(), &bind_addr.sin_addr);
        
        if (::bind(socket_, reinterpret_cast<sockaddr*>(&bind_addr), sizeof(bind_addr)) != 0) {
            close();
            return false;
        }
        
        bound_port_ = port;
        
        // If port was 0, get the assigned port
        if (port == 0) {
            sockaddr_in actual_addr{};
            socklen_t len = sizeof(actual_addr);
            getsockname(socket_, reinterpret_cast<sockaddr*>(&actual_addr), &len);
            bound_port_ = ntohs(actual_addr.sin_port);
        }
        
        return true;
    }
    
    void close() {
        if (socket_ != INVALID_SOCK) {
#ifdef _WIN32
            closesocket(socket_);
#else
            ::close(socket_);
#endif
            socket_ = INVALID_SOCK;
        }
    }
    
    bool isOpen() const { return socket_ != INVALID_SOCK; }
    uint16_t getPort() const { return bound_port_; }
    
    // =========================================================================
    // Send/Receive
    // =========================================================================
    
    bool sendTo(const uint8_t* data, size_t length, const SystemAddress& address) {
        if (socket_ == INVALID_SOCK) return false;
        
        const auto* addr4 = reinterpret_cast<const sockaddr_in*>(&address.address);
        int result = ::sendto(socket_, reinterpret_cast<const char*>(data),
                              static_cast<int>(length), 0,
                              reinterpret_cast<const sockaddr*>(addr4), sizeof(sockaddr_in));
        return result >= 0;
    }
    
    bool sendTo(const std::vector<uint8_t>& data, const SystemAddress& address) {
        return sendTo(data.data(), data.size(), address);
    }
    
    RecvResult receiveFrom() {
        RecvResult result;
        if (socket_ == INVALID_SOCK) return result;
        
        uint8_t buffer[MAXIMUM_MTU_SIZE];
        sockaddr_in from_addr{};
        socklen_t from_len = sizeof(from_addr);
        
        int bytes = ::recvfrom(socket_, reinterpret_cast<char*>(buffer), sizeof(buffer), 0,
                               reinterpret_cast<sockaddr*>(&from_addr), &from_len);
        
        if (bytes > 0) {
            result.data.assign(buffer, buffer + bytes);
            std::memcpy(&result.from_address.address, &from_addr, sizeof(from_addr));
            result.from_address.debug_port = ntohs(from_addr.sin_port);
            result.success = true;
        }
        
        return result;
    }
    
private:
    void setNonBlocking(bool non_blocking) {
#ifdef _WIN32
        u_long mode = non_blocking ? 1 : 0;
        ioctlsocket(socket_, FIONBIO, &mode);
#else
        int flags = fcntl(socket_, F_GETFL, 0);
        if (non_blocking) {
            fcntl(socket_, F_SETFL, flags | O_NONBLOCK);
        } else {
            fcntl(socket_, F_SETFL, flags & ~O_NONBLOCK);
        }
#endif
    }
    
    SocketHandle socket_ = INVALID_SOCK;
    uint16_t bound_port_ = 0;

#ifdef _WIN32
    using socklen_t = int;
#endif
};

} // namespace cphertunnel::raknet
