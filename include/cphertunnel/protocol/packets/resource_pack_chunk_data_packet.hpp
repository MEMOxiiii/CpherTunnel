// CpherTunnel - ResourcePackChunkDataPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ResourcePackChunkDataPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ResourcePackChunkDataPacket, ResourcePackChunkData)

    std::string resource_name;
    uint32_t chunk_index = 0;
    uint64_t byte_offset = 0;
    std::string chunk_data;

    void write(BinaryStream& stream) const override {
        stream.writeString(resource_name);
        stream.writeUnsignedInt(chunk_index);
        stream.writeUnsignedInt64(byte_offset);
        stream.writeString(chunk_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        resource_name = stream.readString();
        chunk_index = stream.readUnsignedInt();
        byte_offset = stream.readUnsignedInt64();
        chunk_data = stream.readString();
    }

    Compressibility getCompressibility() const override {
        return Compressibility::Incompressible; // already large data
    }
};

} // namespace cphertunnel
