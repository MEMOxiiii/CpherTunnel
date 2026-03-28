// CpherTunnel - CreativeContentPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class CreativeContentPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CreativeContentPacket, CreativeContent)

    ByteBuffer items_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(items_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        items_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
