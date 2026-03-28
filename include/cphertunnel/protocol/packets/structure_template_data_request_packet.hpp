// CpherTunnel - StructureTemplateDataRequestPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class StructureTemplateDataRequestPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(StructureTemplateDataRequestPacket, StructureTemplateDataExportRequest)

    std::string structure_name;
    BlockPos position;
    // Structure Settings
    std::string palette_name;
    bool ignore_entities = false;
    bool ignore_blocks = false;
    bool allow_non_ticking_chunks = false;
    BlockPos structure_size;
    BlockPos structure_offset;
    ActorUniqueID last_edit_player;
    uint8_t rotation = 0;
    uint8_t mirror = 0;
    uint8_t animation_mode = 0;
    float animation_seconds = 0.0f;
    float integrity_value = 100.0f;
    uint32_t integrity_seed = 0;
    Vec3 rotation_pivot;
    uint8_t requested_operation = 0;

    void write(BinaryStream& stream) const override {
        stream.writeString(structure_name);
        stream.writeBlockPos(position);
        // Settings
        stream.writeString(palette_name);
        stream.writeBool(ignore_entities);
        stream.writeBool(ignore_blocks);
        stream.writeBool(allow_non_ticking_chunks);
        stream.writeBlockPos(structure_size);
        stream.writeBlockPos(structure_offset);
        stream.writeActorUniqueID(last_edit_player);
        stream.writeByte(rotation);
        stream.writeByte(mirror);
        stream.writeByte(animation_mode);
        stream.writeFloat(animation_seconds);
        stream.writeFloat(integrity_value);
        stream.writeUnsignedInt(integrity_seed);
        stream.writeVec3(rotation_pivot);
        stream.writeByte(requested_operation);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        structure_name = stream.readString();
        position = stream.readBlockPos();
        palette_name = stream.readString();
        ignore_entities = stream.readBool();
        ignore_blocks = stream.readBool();
        allow_non_ticking_chunks = stream.readBool();
        structure_size = stream.readBlockPos();
        structure_offset = stream.readBlockPos();
        last_edit_player = stream.readActorUniqueID();
        rotation = stream.readByte();
        mirror = stream.readByte();
        animation_mode = stream.readByte();
        animation_seconds = stream.readFloat();
        integrity_value = stream.readFloat();
        integrity_seed = stream.readUnsignedInt();
        rotation_pivot = stream.readVec3();
        requested_operation = stream.readByte();
    }
};

} // namespace cphertunnel
