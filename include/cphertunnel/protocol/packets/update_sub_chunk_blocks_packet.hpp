// CpherTunnel - UpdateSubChunkBlocksPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {


struct SubChunkBlockUpdate {
    NetworkBlockPos position;
    uint32_t block_runtime_id = 0;
    uint32_t flags = 0;
    uint64_t synced_update_entity_unique_id = 0;
    uint64_t synced_update_type = 0;
};

class UpdateSubChunkBlocksPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(UpdateSubChunkBlocksPacket, UpdateSubChunkBlocks)

    NetworkBlockPos sub_chunk_pos;
    std::vector<SubChunkBlockUpdate> standard_blocks;
    std::vector<SubChunkBlockUpdate> extra_blocks;

    void write(BinaryStream& stream) const override {
        stream.writeNetworkBlockPos(sub_chunk_pos);

        stream.writeUnsignedVarInt(static_cast<uint32_t>(standard_blocks.size()));
        for (auto& b : standard_blocks) {
            stream.writeNetworkBlockPos(b.position);
            stream.writeUnsignedVarInt(b.block_runtime_id);
            stream.writeUnsignedVarInt(b.flags);
            stream.writeUnsignedVarInt64(b.synced_update_entity_unique_id);
            stream.writeUnsignedVarInt64(b.synced_update_type);
        }

        stream.writeUnsignedVarInt(static_cast<uint32_t>(extra_blocks.size()));
        for (auto& b : extra_blocks) {
            stream.writeNetworkBlockPos(b.position);
            stream.writeUnsignedVarInt(b.block_runtime_id);
            stream.writeUnsignedVarInt(b.flags);
            stream.writeUnsignedVarInt64(b.synced_update_entity_unique_id);
            stream.writeUnsignedVarInt64(b.synced_update_type);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        sub_chunk_pos = stream.readNetworkBlockPos();

        uint32_t stdCount = stream.readUnsignedVarInt();
        standard_blocks.resize(stdCount);
        for (auto& b : standard_blocks) {
            b.position = stream.readNetworkBlockPos();
            b.block_runtime_id = stream.readUnsignedVarInt();
            b.flags = stream.readUnsignedVarInt();
            b.synced_update_entity_unique_id = stream.readUnsignedVarInt64();
            b.synced_update_type = stream.readUnsignedVarInt64();
        }

        uint32_t extCount = stream.readUnsignedVarInt();
        extra_blocks.resize(extCount);
        for (auto& b : extra_blocks) {
            b.position = stream.readNetworkBlockPos();
            b.block_runtime_id = stream.readUnsignedVarInt();
            b.flags = stream.readUnsignedVarInt();
            b.synced_update_entity_unique_id = stream.readUnsignedVarInt64();
            b.synced_update_type = stream.readUnsignedVarInt64();
        }
    }
};

} // namespace cphertunnel
