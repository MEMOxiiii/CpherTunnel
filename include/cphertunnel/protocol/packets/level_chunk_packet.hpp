// CpherTunnel - LevelChunkPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class LevelChunkPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(LevelChunkPacket, FullChunkData)

    ChunkPos position;
    int32_t dimension_id = 0;

    // If false: sub_chunk_count is the number of sub-chunks in serialized data
    // If true: client will request sub-chunks via SubChunkRequest
    bool client_needs_to_request_subchunks = false;

    uint32_t sub_chunk_count = 0;

    // Only used when client_needs_to_request_subchunks = true
    uint16_t sub_chunk_request_limit = 0;

    bool cache_enabled = false;
    std::vector<uint64_t> cache_blob_ids;

    // The serialized chunk data (sub-chunks + biomes + border blocks + tile entities)
    std::string serialized_chunk_data;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(position.x);
        stream.writeVarInt(position.z);
        stream.writeVarInt(dimension_id);

        if (!client_needs_to_request_subchunks) {
            // Legacy mode: send sub-chunk count
            stream.writeUnsignedVarInt(sub_chunk_count);
        } else {
            // SubChunkRequest mode: special marker value
            stream.writeUnsignedVarInt(0xFFFFFFFFu);
            stream.writeUnsignedShort(sub_chunk_request_limit);
        }

        stream.writeBool(cache_enabled);
        if (cache_enabled) {
            stream.writeUnsignedVarInt(static_cast<uint32_t>(cache_blob_ids.size()));
            for (auto blob_id : cache_blob_ids) {
                stream.writeUnsignedInt64(blob_id);
            }
        }

        stream.writeString(serialized_chunk_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position.x = stream.readVarInt();
        position.z = stream.readVarInt();
        dimension_id = stream.readVarInt();

        sub_chunk_count = stream.readUnsignedVarInt();
        if (sub_chunk_count == 0xFFFFFFFFu) {
            client_needs_to_request_subchunks = true;
            sub_chunk_request_limit = stream.readUnsignedShort();
        }

        cache_enabled = stream.readBool();
        if (cache_enabled) {
            uint32_t blob_count = stream.readUnsignedVarInt();
            cache_blob_ids.resize(blob_count);
            for (auto& blob_id : cache_blob_ids) {
                blob_id = stream.readUnsignedInt64();
            }
        }

        serialized_chunk_data = stream.readString();
    }
};

} // namespace cphertunnel
