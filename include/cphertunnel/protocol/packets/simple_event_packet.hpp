// CpherTunnel - SimpleEventPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SimpleEventPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SimpleEventPacket, SimpleEvent)

    uint16_t event_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedShort(event_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        event_type = stream.readUnsignedShort();
    }
};

} // namespace cphertunnel
