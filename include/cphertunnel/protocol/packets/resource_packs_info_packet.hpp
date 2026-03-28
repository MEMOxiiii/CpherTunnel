// CpherTunnel - ResourcePacksInfoPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {


struct ResourcePackInfoEntry {
    UUID pack_id;       // mce::UUID (16 bytes), changed from string in v766
    std::string version;
    uint64_t size = 0;
    std::string content_key;
    std::string sub_pack_name;
    std::string content_id;
    bool has_scripts = false;
    bool is_addon_pack = false;
    bool is_ray_tracing_capable = false;
    std::string cdn_url;        // inline per-entry, moved from separate list in v748
};

class ResourcePacksInfoPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ResourcePacksInfoPacket, ResourcePacksInfo)

    bool must_accept = false;
    bool has_addon_packs = false;
    bool has_scripts = false;
    bool force_disable_vibrant_visuals = false;  // added in v818
    UUID world_template_uuid = {};                // added in v766
    std::string world_template_version;           // added in v766
    std::vector<ResourcePackInfoEntry> resource_packs;

    void write(BinaryStream& stream) const override {
        stream.writeBool(must_accept);
        stream.writeBool(has_addon_packs);
        stream.writeBool(has_scripts);
        stream.writeBool(force_disable_vibrant_visuals);
        stream.writeUUID(world_template_uuid);
        stream.writeString(world_template_version);

        // Resource pack entries
        stream.writeUnsignedShort(static_cast<uint16_t>(resource_packs.size()));
        for (auto& pack : resource_packs) {
            stream.writeUUID(pack.pack_id);
            stream.writeString(pack.version);
            stream.writeUnsignedInt64(pack.size);
            stream.writeString(pack.content_key);
            stream.writeString(pack.sub_pack_name);
            stream.writeString(pack.content_id);
            stream.writeBool(pack.has_scripts);
            stream.writeBool(pack.is_addon_pack);
            stream.writeBool(pack.is_ray_tracing_capable);
            stream.writeString(pack.cdn_url);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        must_accept = stream.readBool();
        has_addon_packs = stream.readBool();
        has_scripts = stream.readBool();
        force_disable_vibrant_visuals = stream.readBool();
        world_template_uuid = stream.readUUID();
        world_template_version = stream.readString();

        uint16_t count = stream.readUnsignedShort();
        resource_packs.resize(count);
        for (auto& pack : resource_packs) {
            pack.pack_id = stream.readUUID();
            pack.version = stream.readString();
            pack.size = stream.readUnsignedInt64();
            pack.content_key = stream.readString();
            pack.sub_pack_name = stream.readString();
            pack.content_id = stream.readString();
            pack.has_scripts = stream.readBool();
            pack.is_addon_pack = stream.readBool();
            pack.is_ray_tracing_capable = stream.readBool();
            pack.cdn_url = stream.readString();
        }
    }
};

} // namespace cphertunnel
