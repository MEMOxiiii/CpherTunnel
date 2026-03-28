// CpherTunnel - UpdateEquipPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class UpdateEquipPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(UpdateEquipPacket, UpdateEquip)

    uint8_t container_id = 0;
    uint8_t type = 0;
    int32_t size = 0;
    ActorUniqueID entity_unique_id;
    ByteBuffer nbt_data; // CompoundTag

    void write(BinaryStream& stream) const override {
        stream.writeByte(container_id);
        stream.writeByte(type);
        stream.writeVarInt(size);
        stream.writeActorUniqueID(entity_unique_id);
        stream.writeRawBytes(nbt_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        container_id = stream.readByte();
        type = stream.readByte();
        size = stream.readVarInt();
        entity_unique_id = stream.readActorUniqueID();
        nbt_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
