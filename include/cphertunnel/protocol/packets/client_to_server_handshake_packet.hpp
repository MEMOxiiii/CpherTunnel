// CpherTunnel - ClientToServerHandshakePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ClientToServerHandshakePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ClientToServerHandshakePacket, ClientToServerHandshake)

    void write(BinaryStream&) const override {}
    void read(ReadOnlyBinaryStream&) override {}
};

} // namespace cphertunnel
