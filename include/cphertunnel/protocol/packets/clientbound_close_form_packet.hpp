// CpherTunnel - ClientboundCloseFormPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ClientboundCloseFormPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ClientboundCloseFormPacket, ClientboundCloseForm)

    void write(BinaryStream&) const override {}
    void read(ReadOnlyBinaryStream&) override {}
};

} // namespace cphertunnel
