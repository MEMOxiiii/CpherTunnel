// CpherTunnel - MobEquipmentPacket (ID: 31)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {
class MobEquipmentPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(MobEquipmentPacket, PlayerEquipment)
    ActorRuntimeID target_runtime_id;
    NetworkItemStackDescriptor item;
    uint8_t slot = 0;
    uint8_t selected_slot = 0;
    uint8_t container_id = 0;

    void write(BinaryStream& s) const override {
        s.writeActorRuntimeID(target_runtime_id);
        item.write(s);
        s.writeByte(slot);
        s.writeByte(selected_slot);
        s.writeByte(container_id);
    }
    void read(ReadOnlyBinaryStream& s) override {
        target_runtime_id = s.readActorRuntimeID();
        item.read(s);
        slot = s.readByte();
        selected_slot = s.readByte();
        container_id = s.readByte();
    }
};
} // namespace cphertunnel
