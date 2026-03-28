// CpherTunnel - UpdateClientOptionsPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class UpdateClientOptionsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(UpdateClientOptionsPacket, UpdateClientOptions)

    ByteBuffer options_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(options_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        options_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
