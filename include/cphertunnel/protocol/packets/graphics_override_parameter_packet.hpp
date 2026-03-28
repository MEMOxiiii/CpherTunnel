// CpherTunnel - GraphicsOverrideParameterPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class GraphicsOverrideParameterPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(GraphicsOverrideParameterPacket, GraphicsParameterOverride)

    ByteBuffer parameter_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(parameter_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        parameter_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
