// CpherTunnel - DebugDrawerPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class DebugDrawerPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(DebugDrawerPacket, ServerScriptDebugDrawer)

    ByteBuffer shape_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(shape_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        shape_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
