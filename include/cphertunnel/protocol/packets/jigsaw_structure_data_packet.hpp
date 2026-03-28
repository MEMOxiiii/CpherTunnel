// CpherTunnel - JigsawStructureDataPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class JigsawStructureDataPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(JigsawStructureDataPacket, JigsawStructureData)

    ByteBuffer structure_data_nbt; // CompoundTag

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(structure_data_nbt);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        structure_data_nbt = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
