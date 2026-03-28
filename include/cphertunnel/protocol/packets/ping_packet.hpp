// CpherTunnel - PingPacket (ID: 115)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class PingPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PingPacket, Ping)
    int64_t send_time = 0;
    void write(BinaryStream& s) const override { s.writeSignedInt64(send_time); }
    void read(ReadOnlyBinaryStream& s) override { send_time = s.readSignedInt64(); }
};
} // namespace cphertunnel
