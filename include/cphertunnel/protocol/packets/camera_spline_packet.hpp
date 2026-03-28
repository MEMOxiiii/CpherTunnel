// CpherTunnel - CameraSplinePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class CameraSplinePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CameraSplinePacket, CameraSpline)

    ByteBuffer spline_data; // complex structure

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(spline_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        spline_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
