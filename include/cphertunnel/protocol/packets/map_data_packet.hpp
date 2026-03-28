// CpherTunnel - MapDataPacket (ID: 67)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {
class MapDataPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(MapDataPacket, MapData)

    enum TypeFlags : uint32_t {
        TextureUpdate = 0x02,
        DecorationUpdate = 0x04,
        Creation = 0x08
    };

    ActorUniqueID map_id;
    uint32_t type_flags = 0;
    uint8_t dimension = 0;
    bool is_locked = false;
    BlockPos map_origin{0, 0, 0};
    // Creation
    std::vector<ActorUniqueID> map_id_list;
    ActorUniqueID extra_map_id;
    // Scale (if decoration/texture/creation)
    uint8_t scale = 0;
    // DecorationUpdate
    std::vector<ActorUniqueID> tracked_actor_ids;
    std::vector<MapDecoration> decorations;
    // TextureUpdate
    int32_t texture_width = 0, texture_height = 0;
    int32_t x_tex_coord = 0, y_tex_coord = 0;
    std::vector<uint32_t> pixels;

    void write(BinaryStream& s) const override {
        s.writeActorUniqueID(map_id);
        s.writeUnsignedVarInt(type_flags);
        s.writeByte(dimension);
        s.writeBool(is_locked);
        s.writeBlockPos(map_origin);
        if (type_flags & Creation) {
            s.writeUnsignedVarInt(static_cast<uint32_t>(map_id_list.size()));
            for (auto& id : map_id_list) s.writeActorUniqueID(id);
            s.writeActorUniqueID(extra_map_id);
        }
        if (type_flags & (DecorationUpdate | TextureUpdate | Creation)) {
            s.writeByte(scale);
        }
        if (type_flags & DecorationUpdate) {
            s.writeUnsignedVarInt(static_cast<uint32_t>(tracked_actor_ids.size()));
            for (auto& id : tracked_actor_ids) s.writeActorUniqueID(id);
            s.writeUnsignedVarInt(static_cast<uint32_t>(decorations.size()));
            for (auto& d : decorations) d.write(s);
        }
        if (type_flags & TextureUpdate) {
            s.writeVarInt(texture_width); s.writeVarInt(texture_height);
            s.writeVarInt(x_tex_coord); s.writeVarInt(y_tex_coord);
            s.writeUnsignedVarInt(static_cast<uint32_t>(pixels.size()));
            for (auto p : pixels) s.writeUnsignedVarInt(p);
        }
    }
    void read(ReadOnlyBinaryStream& s) override {
        map_id = s.readActorUniqueID();
        type_flags = s.readUnsignedVarInt();
        dimension = s.readByte();
        is_locked = s.readBool();
        map_origin = s.readBlockPos();
        if (type_flags & Creation) {
            uint32_t c = s.readUnsignedVarInt();
            map_id_list.resize(c);
            for (auto& id : map_id_list) id = s.readActorUniqueID();
            extra_map_id = s.readActorUniqueID();
        }
        if (type_flags & (DecorationUpdate | TextureUpdate | Creation)) {
            scale = s.readByte();
        }
        if (type_flags & DecorationUpdate) {
            uint32_t tc = s.readUnsignedVarInt();
            tracked_actor_ids.resize(tc);
            for (auto& id : tracked_actor_ids) id = s.readActorUniqueID();
            uint32_t dc = s.readUnsignedVarInt();
            decorations.resize(dc);
            for (auto& d : decorations) d.read(s);
        }
        if (type_flags & TextureUpdate) {
            texture_width = s.readVarInt(); texture_height = s.readVarInt();
            x_tex_coord = s.readVarInt(); y_tex_coord = s.readVarInt();
            uint32_t pc = s.readUnsignedVarInt();
            pixels.resize(pc);
            for (auto& p : pixels) p = s.readUnsignedVarInt();
        }
    }
};
} // namespace cphertunnel
