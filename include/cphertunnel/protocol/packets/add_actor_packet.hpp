// CpherTunnel - AddActorPacket (ID: 13)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {
class AddActorPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(AddActorPacket, AddActor)

    ActorUniqueID target_actor_id;
    ActorRuntimeID target_runtime_id;
    std::string actor_type;
    Vec3 position{0, 0, 0};
    Vec3 velocity{0, 0, 0};
    Vec2 rotation{0, 0};
    float y_head_rotation = 0.0f;
    float y_body_rotation = 0.0f;
    std::vector<AttributeData> attributes;
    std::vector<DataItem> actor_data;
    PropertySyncData synched_properties;
    std::vector<ActorLink> actor_links;

    void write(BinaryStream& s) const override {
        s.writeActorUniqueID(target_actor_id);
        s.writeActorRuntimeID(target_runtime_id);
        s.writeString(actor_type);
        s.writeVec3(position);
        s.writeVec3(velocity);
        s.writeVec2(rotation);
        s.writeFloat(y_head_rotation);
        s.writeFloat(y_body_rotation);
        s.writeUnsignedVarInt(static_cast<uint32_t>(attributes.size()));
        for (auto& a : attributes) a.writeSimple(s);
        writeDataItemList(s, actor_data);
        synched_properties.write(s);
        s.writeUnsignedVarInt(static_cast<uint32_t>(actor_links.size()));
        for (auto& link : actor_links) link.write(s);
    }
    void read(ReadOnlyBinaryStream& s) override {
        target_actor_id = s.readActorUniqueID();
        target_runtime_id = s.readActorRuntimeID();
        actor_type = s.readString();
        position = s.readVec3();
        velocity = s.readVec3();
        rotation = s.readVec2();
        y_head_rotation = s.readFloat();
        y_body_rotation = s.readFloat();
        uint32_t attr_count = s.readUnsignedVarInt();
        attributes.resize(attr_count);
        for (auto& a : attributes) a.readSimple(s);
        actor_data = readDataItemList(s);
        synched_properties.read(s);
        uint32_t link_count = s.readUnsignedVarInt();
        actor_links.resize(link_count);
        for (auto& link : actor_links) link.read(s);
    }
};
} // namespace cphertunnel
