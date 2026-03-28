// CpherTunnel - CameraInstructionPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class CameraInstructionPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CameraInstructionPacket, CameraInstruction)

    // Camera instructions have many optional sub-instructions (Set, Clear, Fade,
    // Target, FieldOfView, Spline, AttachToEntity, DetachFromEntity).
    // The full structure is complex; we store as raw bytes for forward compat.
    ByteBuffer instruction_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(instruction_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        instruction_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
