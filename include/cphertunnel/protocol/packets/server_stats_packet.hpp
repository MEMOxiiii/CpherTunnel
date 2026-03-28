// CpherTunnel - ServerStatsPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ServerStatsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ServerStatsPacket, ServerStats)

    float server_time = 0.0f;
    float network_time = 0.0f;

    void write(BinaryStream& stream) const override {
        stream.writeFloat(server_time);
        stream.writeFloat(network_time);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        server_time = stream.readFloat();
        network_time = stream.readFloat();
    }
};

} // namespace cphertunnel
