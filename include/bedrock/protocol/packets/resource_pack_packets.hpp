// =============================================================================
// BedrockProtocol - Resource Pack Packets
// Data transfer for resource/behavior packs
// =============================================================================
#pragma once

#include "../packet.hpp"

namespace bedrock {

// =============================================================================
// ResourcePackDataInfoPacket (ID: 82)
// =============================================================================
class ResourcePackDataInfoPacket : public Packet {
public:
    BEDROCK_PACKET(ResourcePackDataInfoPacket, ResourcePackDataInfo)

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

// =============================================================================
// ResourcePackChunkDataPacket (ID: 83)
// =============================================================================
class ResourcePackChunkDataPacket : public Packet {
public:
    BEDROCK_PACKET(ResourcePackChunkDataPacket, ResourcePackChunkData)

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

// =============================================================================
// ResourcePackChunkRequestPacket (ID: 84)
// =============================================================================
class ResourcePackChunkRequestPacket : public Packet {
public:
    BEDROCK_PACKET(ResourcePackChunkRequestPacket, ResourcePackChunkRequest)

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

// =============================================================================
// ResourcePacksReadyForValidationPacket (ID: 340)
// =============================================================================
class ResourcePacksReadyForValidationPacket : public Packet {
public:
    BEDROCK_PACKET(ResourcePacksReadyForValidationPacket, ResourcePacksReadyForValidation)

    void write(BinaryStream&) const override {}
    void read(ReadOnlyBinaryStream&) override {}
};

} // namespace bedrock
