// CpherTunnel - ResourcePackStackPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {


struct ResourcePackStackEntry {
    std::string pack_id;
    std::string version;
    std::string sub_pack_name;
};

class ResourcePackStackPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ResourcePackStackPacket, ResourcePackStack)

    bool must_accept = false;
    std::vector<ResourcePackStackEntry> texture_packs;
    std::string base_game_version;
    // Experiments
    std::vector<std::pair<std::string, bool>> experiments;
    bool has_previously_used_experiments = false;
    bool include_editor_packs = false; // added in v671

    void write(BinaryStream& stream) const override {
        stream.writeBool(must_accept);

        // Texture/resource pack stack (behavior packs removed in v893)
        stream.writeUnsignedVarInt(static_cast<uint32_t>(texture_packs.size()));
        for (auto& tp : texture_packs) {
            stream.writeString(tp.pack_id);
            stream.writeString(tp.version);
            stream.writeString(tp.sub_pack_name);
        }

        stream.writeString(base_game_version);

        stream.writeUnsignedInt(static_cast<uint32_t>(experiments.size()));
        for (auto& [name, enabled] : experiments) {
            stream.writeString(name);
            stream.writeBool(enabled);
        }
        stream.writeBool(has_previously_used_experiments);
        stream.writeBool(include_editor_packs);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        must_accept = stream.readBool();

        uint32_t tpCount = stream.readUnsignedVarInt();
        texture_packs.resize(tpCount);
        for (auto& tp : texture_packs) {
            tp.pack_id = stream.readString();
            tp.version = stream.readString();
            tp.sub_pack_name = stream.readString();
        }

        base_game_version = stream.readString();

        uint32_t expCount = stream.readUnsignedInt();
        experiments.resize(expCount);
        for (auto& [name, enabled] : experiments) {
            name = stream.readString();
            enabled = stream.readBool();
        }
        has_previously_used_experiments = stream.readBool();
        include_editor_packs = stream.readBool();
    }
};

} // namespace cphertunnel
