// CpherTunnel - ServerSettingsRequestPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ServerSettingsRequestPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ServerSettingsRequestPacket, ServerSettingsRequest)

    void write(BinaryStream&) const override {}
    void read(ReadOnlyBinaryStream&) override {}
};

} // namespace cphertunnel
