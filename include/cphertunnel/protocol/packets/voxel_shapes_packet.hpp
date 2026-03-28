// CpherTunnel - VoxelShapesPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class VoxelShapesPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(VoxelShapesPacket, VoxelShapes)

    ByteBuffer shapes_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(shapes_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        shapes_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
