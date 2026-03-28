// CpherTunnel - ResourcePackDataInfoPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ResourcePackDataInfoPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ResourcePackDataInfoPacket, ResourcePackDataInfo)

    std::string resource_name;
    uint32_t chunk_size = 0;
    uint32_t chunk_count = 0;
    uint64_t file_size = 0;
    std::string file_hash;
    bool is_premium = false;
    uint8_t pack_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeString(resource_name);
        stream.writeUnsignedInt(chunk_size);
        stream.writeUnsignedInt(chunk_count);
        stream.writeUnsignedInt64(file_size);
        stream.writeString(file_hash);
        stream.writeBool(is_premium);
        stream.writeByte(pack_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        resource_name = stream.readString();
        chunk_size = stream.readUnsignedInt();
        chunk_count = stream.readUnsignedInt();
        file_size = stream.readUnsignedInt64();
        file_hash = stream.readString();
        is_premium = stream.readBool();
        pack_type = stream.readByte();
    }
};

} // namespace cphertunnel
