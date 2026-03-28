// CpherTunnel - CameraPresetsPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class CameraPresetsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CameraPresetsPacket, CameraPresets)

    // Camera presets are complex nested structures - stored as raw bytes
    ByteBuffer presets_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(presets_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        presets_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
