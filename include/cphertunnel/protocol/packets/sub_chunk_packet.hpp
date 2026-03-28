// CpherTunnel - SubChunkPacket (ID: 174)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {
class SubChunkPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SubChunkPacket, SubChunk)

    struct SubChunkEntry {
        SubChunkPosOffset offset;
        uint8_t result = 0;       // SubChunkRequestResult
        std::string serialized_data;
        uint8_t heightmap_type = 0;
        std::vector<int16_t> heightmap_data; // 256 entries if has_heightmap
        uint64_t blob_hash = 0;   // only if cache_enabled
    };

    bool cache_enabled = false;
    int32_t dimension_type = 0;
    SubChunkPos center_pos;
    std::vector<SubChunkEntry> entries;

    void write(BinaryStream& s) const override {
        s.writeBool(cache_enabled);
        s.writeVarInt(dimension_type);
        center_pos.write(s);
        s.writeUnsignedInt(static_cast<uint32_t>(entries.size()));
        for (auto& e : entries) {
            e.offset.write(s);
            s.writeByte(e.result);
            if (!cache_enabled || e.result != 0) {
                s.writeString(e.serialized_data);
            }
            s.writeByte(e.heightmap_type);
            if (e.heightmap_type == 1) { // HasData
                for (auto h : e.heightmap_data) s.writeSignedShort(h);
            }
            if (cache_enabled) s.writeUnsignedInt64(e.blob_hash);
        }
    }
    void read(ReadOnlyBinaryStream& s) override {
        cache_enabled = s.readBool();
        dimension_type = s.readVarInt();
        center_pos.read(s);
        uint32_t count = s.readUnsignedInt();
        entries.resize(count);
        for (auto& e : entries) {
            e.offset.read(s);
            e.result = s.readByte();
            if (!cache_enabled || e.result != 0) {
                e.serialized_data = s.readString();
            }
            e.heightmap_type = s.readByte();
            if (e.heightmap_type == 1) {
                e.heightmap_data.resize(256);
                for (auto& h : e.heightmap_data) h = s.readSignedShort();
            }
            if (cache_enabled) e.blob_hash = s.readUnsignedInt64();
        }
    }
};
} // namespace cphertunnel
