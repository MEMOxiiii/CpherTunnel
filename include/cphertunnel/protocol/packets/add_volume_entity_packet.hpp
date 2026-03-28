// CpherTunnel - AddVolumeEntityPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class AddVolumeEntityPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(AddVolumeEntityPacket, AddVolumeEntity)

    uint32_t entity_net_id = 0;
    ByteBuffer components_nbt;  // CompoundTag NBT data
    std::string json_identifier;
    std::string instance_name;
    BlockPos min_bounds;
    BlockPos max_bounds;
    int32_t dimension_type = 0;
    std::string engine_version;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(entity_net_id);
        stream.writeRawBytes(components_nbt);
        stream.writeString(json_identifier);
        stream.writeString(instance_name);
        stream.writeVarInt(min_bounds.x);
        stream.writeVarInt(min_bounds.y);
        stream.writeVarInt(min_bounds.z);
        stream.writeVarInt(max_bounds.x);
        stream.writeVarInt(max_bounds.y);
        stream.writeVarInt(max_bounds.z);
        stream.writeVarInt(dimension_type);
        stream.writeString(engine_version);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_net_id = stream.readUnsignedVarInt();
        // NBT compound data — complex; read remaining and parse externally
        components_nbt = stream.readRemainingBytes();
        json_identifier = stream.readString();
        instance_name = stream.readString();
        min_bounds.x = stream.readVarInt();
        min_bounds.y = stream.readVarInt();
        min_bounds.z = stream.readVarInt();
        max_bounds.x = stream.readVarInt();
        max_bounds.y = stream.readVarInt();
        max_bounds.z = stream.readVarInt();
        dimension_type = stream.readVarInt();
        engine_version = stream.readString();
    }
};

} // namespace cphertunnel
