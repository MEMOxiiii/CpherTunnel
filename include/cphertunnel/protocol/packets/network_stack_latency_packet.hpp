// CpherTunnel - NetworkStackLatencyPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class NetworkStackLatencyPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(NetworkStackLatencyPacket, Ping)

    uint64_t timestamp = 0;
    bool from_server = false;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedInt64(timestamp);
        stream.writeBool(from_server);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        timestamp = stream.readUnsignedInt64();
        from_server = stream.readBool();
    }
};

} // namespace cphertunnel
