// CpherTunnel - RefreshEntitlementsPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class RefreshEntitlementsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(RefreshEntitlementsPacket, RefreshEntitlements)

    void write(BinaryStream&) const override {}
    void read(ReadOnlyBinaryStream&) override {}
};

} // namespace cphertunnel
