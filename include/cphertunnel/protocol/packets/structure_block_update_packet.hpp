// CpherTunnel - StructureBlockUpdatePacket (ID: 90)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class StructureBlockUpdatePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(StructureBlockUpdatePacket, StructureBlockUpdate)
    BlockPos block_position{0, 0, 0};
    // StructureEditorData - stored as raw bytes for flexibility
    std::string structure_data_raw;
    bool trigger = false;
    bool is_waterlogged = false;

    void write(BinaryStream& s) const override {
        s.writeBlockPos(block_position);
        s.writeRawBytes(structure_data_raw);
        s.writeBool(trigger);
        s.writeBool(is_waterlogged);
    }
    void read(ReadOnlyBinaryStream& s) override {
        block_position = s.readBlockPos();
        // Read remaining minus 2 bools at end
        size_t remaining = s.getUnreadLength();
        if (remaining > 2) {
            structure_data_raw.resize(remaining - 2);
            s.read(structure_data_raw.data(), remaining - 2);
        }
        trigger = s.readBool();
        is_waterlogged = s.readBool();
    }
};
} // namespace cphertunnel
