// CpherTunnel - AutomationClientConnectPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class AutomationClientConnectPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(AutomationClientConnectPacket, AutomationClientConnect)

    std::string server_uri;

    void write(BinaryStream& stream) const override {
        stream.writeString(server_uri);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        server_uri = stream.readString();
    }
};

} // namespace cphertunnel
