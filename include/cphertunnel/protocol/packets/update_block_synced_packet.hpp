// CpherTunnel - UpdateBlockSyncedPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class UpdateBlockSyncedPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(UpdateBlockSyncedPacket, UpdateBlockSynced)

    NetworkBlockPos position;
    uint32_t block_runtime_id = 0;
    uint32_t flags = 0;
    uint32_t layer = 0;
    uint64_t entity_unique_id = 0;
    uint64_t update_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeNetworkBlockPos(position);
        stream.writeUnsignedVarInt(block_runtime_id);
        stream.writeUnsignedVarInt(flags);
        stream.writeUnsignedVarInt(layer);
        stream.writeUnsignedVarInt64(entity_unique_id);
        stream.writeUnsignedVarInt64(update_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readNetworkBlockPos();
        block_runtime_id = stream.readUnsignedVarInt();
        flags = stream.readUnsignedVarInt();
        layer = stream.readUnsignedVarInt();
        entity_unique_id = stream.readUnsignedVarInt64();
        update_type = stream.readUnsignedVarInt64();
    }
};

} // namespace cphertunnel
