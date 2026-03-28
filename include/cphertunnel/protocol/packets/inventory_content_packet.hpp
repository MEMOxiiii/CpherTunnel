// CpherTunnel - InventoryContentPacket (ID: 49)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {
class InventoryContentPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(InventoryContentPacket, InventoryContent)
    uint32_t inventory_id = 0;
    std::vector<NetworkItemStackDescriptor> slots;
    FullContainerName full_container_name;
    NetworkItemStackDescriptor storage_item;

    void write(BinaryStream& s) const override {
        s.writeUnsignedVarInt(inventory_id);
        s.writeUnsignedVarInt(static_cast<uint32_t>(slots.size()));
        for (auto& slot : slots) slot.write(s);
        full_container_name.write(s);
        storage_item.write(s);
    }
    void read(ReadOnlyBinaryStream& s) override {
        inventory_id = s.readUnsignedVarInt();
        uint32_t count = s.readUnsignedVarInt();
        slots.resize(count);
        for (auto& slot : slots) slot.read(s);
        full_container_name.read(s);
        storage_item.read(s);
    }
};
} // namespace cphertunnel
