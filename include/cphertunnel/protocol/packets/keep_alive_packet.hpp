// CpherTunnel - KeepAlivePacket (ID: 0)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class KeepAlivePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(KeepAlivePacket, KeepAlive)
    void write(BinaryStream&) const override {}
    void read(ReadOnlyBinaryStream&) override {}
};
} // namespace cphertunnel
