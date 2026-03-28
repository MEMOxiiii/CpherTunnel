// CpherTunnel - MobArmorEquipmentPacket (ID: 32)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {
class MobArmorEquipmentPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(MobArmorEquipmentPacket, MobArmorEquipment)
    ActorRuntimeID target_runtime_id;
    NetworkItemStackDescriptor head;
    NetworkItemStackDescriptor torso;
    NetworkItemStackDescriptor legs;
    NetworkItemStackDescriptor feet;
    NetworkItemStackDescriptor body;

    void write(BinaryStream& s) const override {
        s.writeActorRuntimeID(target_runtime_id);
        head.write(s); torso.write(s); legs.write(s); feet.write(s); body.write(s);
    }
    void read(ReadOnlyBinaryStream& s) override {
        target_runtime_id = s.readActorRuntimeID();
        head.read(s); torso.read(s); legs.read(s); feet.read(s); body.read(s);
    }
};
} // namespace cphertunnel
