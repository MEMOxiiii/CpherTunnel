// CpherTunnel - AddPlayerPacket (ID: 12)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {
class AddPlayerPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(AddPlayerPacket, AddPlayer)

    UUID uuid;
    std::string player_name;
    ActorRuntimeID target_runtime_id;
    std::string platform_chat_id;
    Vec3 position{0, 0, 0};
    Vec3 velocity{0, 0, 0};
    Vec2 rotation{0, 0};
    float y_head_rotation = 0.0f;
    NetworkItemStackDescriptor carried_item;
    int32_t player_game_type = 0;
    std::vector<DataItem> actor_data;
    PropertySyncData synched_properties;
    SerializedAbilitiesData abilities;
    std::vector<ActorLink> actor_links;
    std::string device_id;
    int32_t build_platform = 0;

    void write(BinaryStream& s) const override {
        s.writeUUID(uuid);
        s.writeString(player_name);
        s.writeActorRuntimeID(target_runtime_id);
        s.writeString(platform_chat_id);
        s.writeVec3(position);
        s.writeVec3(velocity);
        s.writeVec2(rotation);
        s.writeFloat(y_head_rotation);
        carried_item.write(s);
        s.writeVarInt(player_game_type);
        writeDataItemList(s, actor_data);
        synched_properties.write(s);
        abilities.write(s);
        s.writeUnsignedVarInt(static_cast<uint32_t>(actor_links.size()));
        for (auto& link : actor_links) link.write(s);
        s.writeString(device_id);
        s.writeSignedInt(build_platform);
    }
    void read(ReadOnlyBinaryStream& s) override {
        uuid = s.readUUID();
        player_name = s.readString();
        target_runtime_id = s.readActorRuntimeID();
        platform_chat_id = s.readString();
        position = s.readVec3();
        velocity = s.readVec3();
        rotation = s.readVec2();
        y_head_rotation = s.readFloat();
        carried_item.read(s);
        player_game_type = s.readVarInt();
        actor_data = readDataItemList(s);
        synched_properties.read(s);
        abilities.read(s);
        uint32_t link_count = s.readUnsignedVarInt();
        actor_links.resize(link_count);
        for (auto& link : actor_links) link.read(s);
        device_id = s.readString();
        build_platform = s.readSignedInt();
    }
};
} // namespace cphertunnel
