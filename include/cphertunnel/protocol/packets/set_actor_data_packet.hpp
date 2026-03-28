// CpherTunnel - SetActorDataPacket (ID: 39)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {
class SetActorDataPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SetActorDataPacket, SetActorData)
    ActorRuntimeID target_runtime_id;
    std::vector<DataItem> actor_data;
    PropertySyncData synched_properties;
    PlayerInputTick tick;

    void write(BinaryStream& s) const override {
        s.writeActorRuntimeID(target_runtime_id);
        writeDataItemList(s, actor_data);
        synched_properties.write(s);
        tick.write(s);
    }
    void read(ReadOnlyBinaryStream& s) override {
        target_runtime_id = s.readActorRuntimeID();
        actor_data = readDataItemList(s);
        synched_properties.read(s);
        tick.read(s);
    }
};
} // namespace cphertunnel
