// CpherTunnel - ClientboundTextureShiftPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ClientboundTextureShiftPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ClientboundTextureShiftPacket, ClientboundTextureShift)

    ByteBuffer shift_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(shift_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        shift_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
