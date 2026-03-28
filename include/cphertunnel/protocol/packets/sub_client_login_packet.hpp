// CpherTunnel - SubClientLoginPacket (ID: 94)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class SubClientLoginPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SubClientLoginPacket, SubClientLogin)
    std::string connection_request;
    void write(BinaryStream& s) const override { s.writeString(connection_request); }
    void read(ReadOnlyBinaryStream& s) override { connection_request = s.readString(); }
};
} // namespace cphertunnel
