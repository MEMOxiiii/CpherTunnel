// CpherTunnel - InventorySlotPacket (ID: 50)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {
class InventorySlotPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(InventorySlotPacket, InventorySlot)
    uint32_t container_id = 0;
    uint32_t slot = 0;
    FullContainerName full_container_name;
    NetworkItemStackDescriptor storage_item;
    NetworkItemStackDescriptor item;

    void write(BinaryStream& s) const override {
        s.writeUnsignedVarInt(container_id);
        s.writeUnsignedVarInt(slot);
        full_container_name.write(s);
        storage_item.write(s);
        item.write(s);
    }
    void read(ReadOnlyBinaryStream& s) override {
        container_id = s.readUnsignedVarInt();
        slot = s.readUnsignedVarInt();
        full_container_name.read(s);
        storage_item.read(s);
        item.read(s);
    }
};
} // namespace cphertunnel
