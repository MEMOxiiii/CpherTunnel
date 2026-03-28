// CpherTunnel - AddItemActorPacket (ID: 15)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {
class AddItemActorPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(AddItemActorPacket, AddItemActor)
    ActorUniqueID target_actor_id;
    ActorRuntimeID target_runtime_id;
    NetworkItemStackDescriptor item;
    Vec3 position{0, 0, 0};
    Vec3 velocity{0, 0, 0};
    std::vector<DataItem> entity_data;
    bool from_fishing = false;

    void write(BinaryStream& s) const override {
        s.writeActorUniqueID(target_actor_id);
        s.writeActorRuntimeID(target_runtime_id);
        item.write(s);
        s.writeVec3(position);
        s.writeVec3(velocity);
        writeDataItemList(s, entity_data);
        s.writeBool(from_fishing);
    }
    void read(ReadOnlyBinaryStream& s) override {
        target_actor_id = s.readActorUniqueID();
        target_runtime_id = s.readActorRuntimeID();
        item.read(s);
        position = s.readVec3();
        velocity = s.readVec3();
        entity_data = readDataItemList(s);
        from_fishing = s.readBool();
    }
};
} // namespace cphertunnel
