// =============================================================================
// BedrockProtocol - World Packets
// Chunks, blocks, level events, sound events, etc.
// =============================================================================
#pragma once

#include "../packet.hpp"
#include "../nbt.hpp"

namespace bedrock {

// =============================================================================
// UpdateBlockPacket (ID: 21)
// =============================================================================
class UpdateBlockPacket : public Packet {
public:
    BEDROCK_PACKET(UpdateBlockPacket, UpdateBlock)

    NetworkBlockPos position;
    uint32_t block_runtime_id = 0;
    uint32_t flags = 0; // bitset: 0x1=neighbors, 0x2=network, 0x4=no_graphic, 0x8=priority
    uint32_t layer = 0; // 0=normal, 1=liquid

    void write(BinaryStream& stream) const override {
        stream.writeNetworkBlockPos(position);
        stream.writeUnsignedVarInt(block_runtime_id);
        stream.writeUnsignedVarInt(flags);
        stream.writeUnsignedVarInt(layer);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readNetworkBlockPos();
        block_runtime_id = stream.readUnsignedVarInt();
        flags = stream.readUnsignedVarInt();
        layer = stream.readUnsignedVarInt();
    }
};

// =============================================================================
// UpdateBlockSyncedPacket (ID: 110)
// =============================================================================
class UpdateBlockSyncedPacket : public Packet {
public:
    BEDROCK_PACKET(UpdateBlockSyncedPacket, UpdateBlockSynced)

    NetworkBlockPos position;
    uint32_t block_runtime_id = 0;
    uint32_t flags = 0;
    uint32_t layer = 0;
    uint64_t entity_unique_id = 0;
    uint64_t update_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeNetworkBlockPos(position);
        stream.writeUnsignedVarInt(block_runtime_id);
        stream.writeUnsignedVarInt(flags);
        stream.writeUnsignedVarInt(layer);
        stream.writeUnsignedVarInt64(entity_unique_id);
        stream.writeUnsignedVarInt64(update_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readNetworkBlockPos();
        block_runtime_id = stream.readUnsignedVarInt();
        flags = stream.readUnsignedVarInt();
        layer = stream.readUnsignedVarInt();
        entity_unique_id = stream.readUnsignedVarInt64();
        update_type = stream.readUnsignedVarInt64();
    }
};

// =============================================================================
// BlockEventPacket (ID: 26)
// =============================================================================
class BlockEventPacket : public Packet {
public:
    BEDROCK_PACKET(BlockEventPacket, BlockEvent)

    NetworkBlockPos position;
    int32_t event_type = 0;
    int32_t event_data = 0;

    void write(BinaryStream& stream) const override {
        stream.writeNetworkBlockPos(position);
        stream.writeVarInt(event_type);
        stream.writeVarInt(event_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readNetworkBlockPos();
        event_type = stream.readVarInt();
        event_data = stream.readVarInt();
    }
};

// =============================================================================
// BlockActorDataPacket (ID: 56)
// =============================================================================
class BlockActorDataPacket : public Packet {
public:
    BEDROCK_PACKET(BlockActorDataPacket, BlockActorData)

    NetworkBlockPos position;
    std::shared_ptr<NbtTag> nbt_data;

    void write(BinaryStream& stream) const override {
        stream.writeNetworkBlockPos(position);
        if (nbt_data) {
            NbtTag::writeNetworkNbt(stream, *nbt_data);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readNetworkBlockPos();
        nbt_data = NbtTag::readNetworkNbt(stream);
    }
};

// =============================================================================
// LevelEventPacket (ID: 25)
// =============================================================================
class LevelEventPacket : public Packet {
public:
    BEDROCK_PACKET(LevelEventPacket, LevelEvent)

    int32_t event_id = 0;
    Vec3 position;
    int32_t data = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(event_id);
        stream.writeVec3(position);
        stream.writeVarInt(data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        event_id = stream.readVarInt();
        position = stream.readVec3();
        data = stream.readVarInt();
    }
};

// =============================================================================
// LevelSoundEventPacket (ID: 123)
// =============================================================================
class LevelSoundEventPacket : public Packet {
public:
    BEDROCK_PACKET(LevelSoundEventPacket, LevelSoundEvent)

    uint32_t sound_id = 0;
    Vec3 position;
    int32_t extra_data = -1;
    std::string entity_type;
    bool is_baby_mob = false;
    bool is_global = false;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(sound_id);
        stream.writeVec3(position);
        stream.writeVarInt(extra_data);
        stream.writeString(entity_type);
        stream.writeBool(is_baby_mob);
        stream.writeBool(is_global);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        sound_id = stream.readUnsignedVarInt();
        position = stream.readVec3();
        extra_data = stream.readVarInt();
        entity_type = stream.readString();
        is_baby_mob = stream.readBool();
        is_global = stream.readBool();
    }
};

// =============================================================================
// LevelEventGenericPacket (ID: 124)
// =============================================================================
class LevelEventGenericPacket : public Packet {
public:
    BEDROCK_PACKET(LevelEventGenericPacket, LevelEventGeneric)

    int32_t event_id = 0;
    std::shared_ptr<NbtTag> event_data;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(event_id);
        if (event_data) {
            NbtTag::writeNetworkNbt(stream, *event_data);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        event_id = stream.readVarInt();
        event_data = NbtTag::readNetworkNbt(stream);
    }
};

// =============================================================================
// NetworkChunkPublisherUpdatePacket (ID: 121)
// =============================================================================
class NetworkChunkPublisherUpdatePacket : public Packet {
public:
    BEDROCK_PACKET(NetworkChunkPublisherUpdatePacket, NetworkChunkPublisherUpdate)

    BlockPos position;
    uint32_t radius = 0;
    std::vector<ChunkPos> saved_chunks; // Server built chunks list

    void write(BinaryStream& stream) const override {
        stream.writeBlockPos(position);
        stream.writeUnsignedVarInt(radius);
        // "No size compression" = plain uint32 (not varint)
        stream.writeUnsignedInt(static_cast<uint32_t>(saved_chunks.size()));
        for (auto& cp : saved_chunks) {
            stream.writeVarInt(cp.x);
            stream.writeVarInt(cp.z);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readBlockPos();
        radius = stream.readUnsignedVarInt();
        uint32_t count = stream.readUnsignedInt();
        saved_chunks.resize(count);
        for (auto& cp : saved_chunks) {
            cp.x = stream.readVarInt();
            cp.z = stream.readVarInt();
        }
    }
};

// =============================================================================
// SpawnExperienceOrbPacket (ID: 66)
// =============================================================================
class SpawnExperienceOrbPacket : public Packet {
public:
    BEDROCK_PACKET(SpawnExperienceOrbPacket, SpawnExperienceOrb)

    Vec3 position;
    int32_t count = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVec3(position);
        stream.writeVarInt(count);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readVec3();
        count = stream.readVarInt();
    }
};

// =============================================================================
// SpawnParticleEffectPacket (ID: 118)
// =============================================================================
class SpawnParticleEffectPacket : public Packet {
public:
    BEDROCK_PACKET(SpawnParticleEffectPacket, SpawnParticleEffect)

    uint8_t dimension_id = 0;
    ActorUniqueID entity_id;
    Vec3 position;
    std::string particle_name;
    std::optional<std::string> molang_variables;

    void write(BinaryStream& stream) const override {
        stream.writeByte(dimension_id);
        stream.writeActorUniqueID(entity_id);
        stream.writeVec3(position);
        stream.writeString(particle_name);
        stream.writeBool(molang_variables.has_value());
        if (molang_variables.has_value()) {
            stream.writeString(molang_variables.value());
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        dimension_id = stream.readByte();
        entity_id = stream.readActorUniqueID();
        position = stream.readVec3();
        particle_name = stream.readString();
        if (stream.readBool()) {
            molang_variables = stream.readString();
        }
    }
};

// =============================================================================
// UpdateSubChunkBlocksPacket (ID: 172)
// =============================================================================
struct SubChunkBlockUpdate {
    NetworkBlockPos position;
    uint32_t block_runtime_id = 0;
    uint32_t flags = 0;
    uint64_t synced_update_entity_unique_id = 0;
    uint64_t synced_update_type = 0;
};

class UpdateSubChunkBlocksPacket : public Packet {
public:
    BEDROCK_PACKET(UpdateSubChunkBlocksPacket, UpdateSubChunkBlocks)

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

// =============================================================================
// OnScreenTextureAnimationPacket (ID: 130)
// =============================================================================
class OnScreenTextureAnimationPacket : public Packet {
public:
    BEDROCK_PACKET(OnScreenTextureAnimationPacket, OnScreenTextureAnimation)

    uint32_t animation_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedInt(animation_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        animation_type = stream.readUnsignedInt();
    }
};

// =============================================================================
// LevelChunkPacket (ID: 58, a.k.a. FullChunkData)
// Server -> Client: Full chunk data with serialized sub-chunks
// =============================================================================
class LevelChunkPacket : public Packet {
public:
    BEDROCK_PACKET(LevelChunkPacket, FullChunkData)

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

// =============================================================================
// BiomeDefinitionListPacket (ID: 122)
// Server -> Client: Biome definitions (v800+ format: map + string list)
// =============================================================================
class BiomeDefinitionListPacket : public Packet {
public:
    BEDROCK_PACKET(BiomeDefinitionListPacket, BiomeDefinitionList)

    // Raw bytes for the full packet body, since biome definition data
    // is a complex structured format (not just NBT since v800)
    ByteBuffer raw_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(raw_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        raw_data = stream.readRemainingBytes();
    }

    // Helper: build an empty biome definition list (0 biomes, 0 strings)
    static BiomeDefinitionListPacket makeEmpty() {
        BiomeDefinitionListPacket pkt;
        BinaryStream bs;
        bs.writeUnsignedVarInt(0);  // map size: 0 biomes
        bs.writeUnsignedVarInt(0);  // string list size: 0 strings
        auto& buf = bs.getBuffer();
        pkt.raw_data.assign(buf.begin(), buf.end());
        return pkt;
    }
};

// =============================================================================
// CreativeContentPacket (ID: 145)
// Server -> Client: Creative inventory items
// =============================================================================
class CreativeContentPacket : public Packet {
public:
    BEDROCK_PACKET(CreativeContentPacket, CreativeContent)

    ByteBuffer items_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(items_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        items_data = stream.readRemainingBytes();
    }
};

} // namespace bedrock
