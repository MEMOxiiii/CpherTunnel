// CpherTunnel - ServerToClientHandshakePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ServerToClientHandshakePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ServerToClientHandshakePacket, ServerToClientHandshake)

    std::string handshake_jwt; // Base64 JWT containing salt + public key

    void write(BinaryStream& stream) const override {
        stream.writeString(handshake_jwt);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        handshake_jwt = stream.readString();
    }
};

} // namespace cphertunnel
