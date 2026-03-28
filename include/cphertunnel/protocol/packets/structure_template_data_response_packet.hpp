// CpherTunnel - StructureTemplateDataResponsePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class StructureTemplateDataResponsePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(StructureTemplateDataResponsePacket, StructureTemplateDataExportResponse)

    enum class ResponseType : uint8_t {
        None   = 0,
        Export = 1,
        Query  = 2,
    };

    std::string structure_name;
    ByteBuffer structure_nbt; // optional CompoundTag
    ResponseType response_type = ResponseType::None;

    void write(BinaryStream& stream) const override {
        stream.writeString(structure_name);
        stream.writeRawBytes(structure_nbt);
        stream.writeByte(static_cast<uint8_t>(response_type));
    }

    void read(ReadOnlyBinaryStream& stream) override {
        structure_name = stream.readString();
        // NBT data + response_type at end - simplification: read all remaining
        structure_nbt = stream.readRemainingBytes();
        if (!structure_nbt.empty()) {
            response_type = static_cast<ResponseType>(structure_nbt.back());
            structure_nbt.pop_back();
        }
    }
};

} // namespace cphertunnel
