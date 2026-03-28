// CpherTunnel - LocatorBarPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class LocatorBarPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(LocatorBarPacket, LocatorBar)

    ByteBuffer waypoints_data; // array of LocatorBarWaypointPayload

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(waypoints_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        waypoints_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
