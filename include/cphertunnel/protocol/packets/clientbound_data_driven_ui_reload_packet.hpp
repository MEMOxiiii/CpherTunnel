// CpherTunnel - ClientboundDataDrivenUIReloadPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ClientboundDataDrivenUIReloadPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ClientboundDataDrivenUIReloadPacket, ClientboundDataDrivenUIReload)

    void write(BinaryStream&) const override {}
    void read(ReadOnlyBinaryStream&) override {}
};

} // namespace cphertunnel
