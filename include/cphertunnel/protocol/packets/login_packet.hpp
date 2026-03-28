// CpherTunnel - LoginPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class LoginPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(LoginPacket, Login)

    int32_t client_network_version = PROTOCOL_VERSION;
    std::string connection_request; // JWT chain data

    void write(BinaryStream& stream) const override {
        stream.writeSignedBigEndianInt(client_network_version);
        // Connection request is written as a string with VarInt length
        stream.writeString(connection_request);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        client_network_version = stream.readSignedBigEndianInt();
        connection_request = stream.readString();
    }

    Compressibility getCompressibility() const override {
        return Compressibility::Incompressible;
    }
};

} // namespace cphertunnel
