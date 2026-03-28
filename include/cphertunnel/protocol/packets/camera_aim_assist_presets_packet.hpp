// CpherTunnel - CameraAimAssistPresetsPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class CameraAimAssistPresetsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CameraAimAssistPresetsPacket, CameraAimAssistPresets)

    // Presets data - complex structure, stored as raw bytes for forward compatibility
    ByteBuffer presets_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(presets_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        presets_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
