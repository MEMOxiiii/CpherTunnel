// CpherTunnel - ResourcePackChunkRequestPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ResourcePackChunkRequestPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ResourcePackChunkRequestPacket, ResourcePackChunkRequest)

    std::string resource_name;
    uint32_t chunk_index = 0;

    void write(BinaryStream& stream) const override {
        stream.writeString(resource_name);
        stream.writeUnsignedInt(chunk_index);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        resource_name = stream.readString();
        chunk_index = stream.readUnsignedInt();
    }
};

} // namespace cphertunnel
