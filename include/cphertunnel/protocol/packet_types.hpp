// =============================================================================
// CpherTunnel - Shared Packet Sub-Types
// Complex types used across multiple packets
// =============================================================================
#pragma once

#include "types.hpp"
#include "binary_stream.hpp"
#include "nbt.hpp"
#include <vector>
#include <string>
#include <bitset>
#include <cstdint>

namespace cphertunnel {

// ─── ActorLink ──────────────────────────────────────────────────
struct ActorLink {
    ActorUniqueID rider_id;
    ActorUniqueID ridden_id;
    uint8_t type = 0;
    bool immediate = false;
    bool rider_initiated = false;

    void write(BinaryStream& s) const {
        s.writeActorUniqueID(rider_id);
        s.writeActorUniqueID(ridden_id);
        s.writeByte(type);
        s.writeBool(immediate);
        s.writeBool(rider_initiated);
    }
    void read(ReadOnlyBinaryStream& s) {
        rider_id = s.readActorUniqueID();
        ridden_id = s.readActorUniqueID();
        type = s.readByte();
        immediate = s.readBool();
        rider_initiated = s.readBool();
    }
};

// ─── DataItem (Entity Metadata) ─────────────────────────────────
struct DataItem {
    enum class Type : uint32_t {
        Byte = 0, Short = 1, Int = 2, Float = 3,
        String = 4, CompoundTag = 5, BlockPos = 6,
        Int64 = 7, Vec3 = 8
    };

    uint32_t id = 0;
    Type type = Type::Byte;

    // Union-like storage
    uint8_t byte_val = 0;
    int16_t short_val = 0;
    int32_t int_val = 0;
    float float_val = 0.0f;
    std::string string_val;
    std::shared_ptr<NbtTag> nbt_val;
    BlockPos blockpos_val{0, 0, 0};
    int64_t int64_val = 0;
    Vec3 vec3_val{0, 0, 0};

    void write(BinaryStream& s) const {
        s.writeUnsignedVarInt(id);
        s.writeUnsignedVarInt(static_cast<uint32_t>(type));
        switch (type) {
            case Type::Byte:        s.writeByte(byte_val); break;
            case Type::Short:       s.writeSignedShort(short_val); break;
            case Type::Int:         s.writeVarInt(int_val); break;
            case Type::Float:       s.writeFloat(float_val); break;
            case Type::String:      s.writeString(string_val); break;
            case Type::CompoundTag: if (nbt_val) NbtTag::writeRootCompound(s, *nbt_val); break;
            case Type::BlockPos:    s.writeBlockPos(blockpos_val); break;
            case Type::Int64:       s.writeVarInt64(int64_val); break;
            case Type::Vec3:        s.writeVec3(vec3_val); break;
        }
    }
    void read(ReadOnlyBinaryStream& s) {
        id = s.readUnsignedVarInt();
        type = static_cast<Type>(s.readUnsignedVarInt());
        switch (type) {
            case Type::Byte:        byte_val = s.readByte(); break;
            case Type::Short:       short_val = s.readSignedShort(); break;
            case Type::Int:         int_val = s.readVarInt(); break;
            case Type::Float:       float_val = s.readFloat(); break;
            case Type::String:      string_val = s.readString(); break;
            case Type::CompoundTag: nbt_val = NbtTag::readRootCompound(s); break;
            case Type::BlockPos:    blockpos_val = s.readBlockPos(); break;
            case Type::Int64:       int64_val = s.readVarInt64(); break;
            case Type::Vec3:        vec3_val = s.readVec3(); break;
        }
    }
};

inline void writeDataItemList(BinaryStream& s, const std::vector<DataItem>& items) {
    s.writeUnsignedVarInt(static_cast<uint32_t>(items.size()));
    for (auto& item : items) item.write(s);
}

inline std::vector<DataItem> readDataItemList(ReadOnlyBinaryStream& s) {
    uint32_t count = s.readUnsignedVarInt();
    std::vector<DataItem> items(count);
    for (auto& item : items) item.read(s);
    return items;
}

// ─── PropertySyncData ───────────────────────────────────────────
struct PropertySyncData {
    struct IntEntry { uint32_t index = 0; int32_t data = 0; };
    struct FloatEntry { uint32_t index = 0; float data = 0.0f; };

    std::vector<IntEntry> int_entries;
    std::vector<FloatEntry> float_entries;

    void write(BinaryStream& s) const {
        s.writeUnsignedVarInt(static_cast<uint32_t>(float_entries.size()));
        for (auto& e : float_entries) {
            s.writeUnsignedVarInt(e.index);
            s.writeFloat(e.data);
        }
        s.writeUnsignedVarInt(static_cast<uint32_t>(int_entries.size()));
        for (auto& e : int_entries) {
            s.writeUnsignedVarInt(e.index);
            s.writeVarInt(e.data);
        }
    }
    void read(ReadOnlyBinaryStream& s) {
        uint32_t fc = s.readUnsignedVarInt();
        float_entries.resize(fc);
        for (auto& e : float_entries) {
            e.index = s.readUnsignedVarInt();
            e.data = s.readFloat();
        }
        uint32_t ic = s.readUnsignedVarInt();
        int_entries.resize(ic);
        for (auto& e : int_entries) {
            e.index = s.readUnsignedVarInt();
            e.data = s.readVarInt();
        }
    }
};

// ─── PlayerInputTick ────────────────────────────────────────────
struct PlayerInputTick {
    uint64_t tick = 0;

    void write(BinaryStream& s) const { s.writeUnsignedVarInt64(tick); }
    void read(ReadOnlyBinaryStream& s) { tick = s.readUnsignedVarInt64(); }
};

// ─── NetworkItemStackDescriptor ─────────────────────────────────
struct ItemStackNetIdVariant {
    enum class Type : uint8_t {
        Invalid = 0,
        ServerInitiated = 1,
        ClientRequest = 2
    };
    Type type = Type::Invalid;
    int32_t net_id = 0;

    void write(BinaryStream& s) const {
        s.writeByte(static_cast<uint8_t>(type));
        if (type != Type::Invalid)
            s.writeVarInt(net_id);
    }
    void read(ReadOnlyBinaryStream& s) {
        type = static_cast<Type>(s.readByte());
        if (type != Type::Invalid)
            net_id = s.readVarInt();
    }
};

struct NetworkItemStackDescriptor {
    int32_t id = 0;  // 0 = empty/air
    uint16_t stack_size = 0;
    uint32_t aux_value = 0;
    bool include_net_id = false;
    ItemStackNetIdVariant net_id_variant;
    int32_t block_runtime_id = 0;
    std::string user_data;  // raw extra data (NBT etc.)

    bool isValid() const { return id != 0; }

    void write(BinaryStream& s) const {
        s.writeVarInt(id);
        if (id == 0) return;
        s.writeUnsignedShort(stack_size);
        s.writeUnsignedVarInt(aux_value);
        s.writeBool(include_net_id);
        if (include_net_id) net_id_variant.write(s);
        s.writeVarInt(block_runtime_id);
        // User data as length-prefixed string
        s.writeString(user_data);
    }
    void read(ReadOnlyBinaryStream& s) {
        id = s.readVarInt();
        if (id == 0) return;
        stack_size = s.readUnsignedShort();
        aux_value = s.readUnsignedVarInt();
        include_net_id = s.readBool();
        if (include_net_id) net_id_variant.read(s);
        block_runtime_id = s.readVarInt();
        user_data = s.readString();
    }
};

// ─── FullContainerName ──────────────────────────────────────────
struct FullContainerName {
    uint8_t container_enum = 0;
    uint32_t dynamic_id = 0;

    void write(BinaryStream& s) const {
        s.writeByte(container_enum);
        s.writeUnsignedInt(dynamic_id);
    }
    void read(ReadOnlyBinaryStream& s) {
        container_enum = s.readByte();
        dynamic_id = s.readUnsignedInt();
    }
};

// ─── SerializedAbilitiesData ────────────────────────────────────
struct SerializedAbilitiesData {
    struct Layer {
        uint16_t layer_type = 0;
        uint32_t abilities_set = 0;
        uint32_t ability_values = 0;
        float fly_speed = 0.05f;
        float vertical_fly_speed = 0.0f;
        float walk_speed = 0.1f;

        void write(BinaryStream& s) const {
            s.writeUnsignedShort(layer_type);
            s.writeUnsignedInt(abilities_set);
            s.writeUnsignedInt(ability_values);
            s.writeFloat(fly_speed);
            s.writeFloat(vertical_fly_speed);
            s.writeFloat(walk_speed);
        }
        void read(ReadOnlyBinaryStream& s) {
            layer_type = s.readUnsignedShort();
            abilities_set = s.readUnsignedInt();
            ability_values = s.readUnsignedInt();
            fly_speed = s.readFloat();
            vertical_fly_speed = s.readFloat();
            walk_speed = s.readFloat();
        }
    };

    int64_t target_player_raw_id = 0;
    uint8_t player_permissions = 0;
    uint8_t command_permissions = 0;
    std::vector<Layer> layers;

    void write(BinaryStream& s) const {
        s.writeSignedInt64(target_player_raw_id);
        s.writeByte(player_permissions);
        s.writeByte(command_permissions);
        s.writeByte(static_cast<uint8_t>(layers.size()));
        for (auto& layer : layers) layer.write(s);
    }
    void read(ReadOnlyBinaryStream& s) {
        target_player_raw_id = s.readSignedInt64();
        player_permissions = s.readByte();
        command_permissions = s.readByte();
        uint8_t count = s.readByte();
        layers.resize(count);
        for (auto& layer : layers) layer.read(s);
    }
};

// ─── Attribute (for AddActor/UpdateAttributes) ──────────────────
struct AttributeData {
    float min_value = 0.0f;
    float max_value = 0.0f;
    float current_value = 0.0f;
    float default_min_value = 0.0f;
    float default_max_value = 0.0f;
    float default_value = 0.0f;
    std::string name;

    struct Modifier {
        std::string id;
        std::string name;
        float amount = 0.0f;
        int32_t operation = 0;
        int32_t operand = 0;
        bool serializable = false;
    };
    std::vector<Modifier> modifiers;

    // Simplified write (for AddActor - only 4 fields)
    void writeSimple(BinaryStream& s) const {
        s.writeString(name);
        s.writeFloat(min_value);
        s.writeFloat(current_value);
        s.writeFloat(max_value);
    }
    void readSimple(ReadOnlyBinaryStream& s) {
        name = s.readString();
        min_value = s.readFloat();
        current_value = s.readFloat();
        max_value = s.readFloat();
    }

    // Full write (for UpdateAttributes - 6 fields + modifiers)
    void writeFull(BinaryStream& s) const {
        s.writeFloat(min_value);
        s.writeFloat(max_value);
        s.writeFloat(current_value);
        s.writeFloat(default_min_value);
        s.writeFloat(default_max_value);
        s.writeFloat(default_value);
        s.writeString(name);
        s.writeUnsignedVarInt(static_cast<uint32_t>(modifiers.size()));
        for (auto& m : modifiers) {
            s.writeString(m.id);
            s.writeString(m.name);
            s.writeFloat(m.amount);
            s.writeSignedInt(m.operation);
            s.writeSignedInt(m.operand);
            s.writeBool(m.serializable);
        }
    }
    void readFull(ReadOnlyBinaryStream& s) {
        min_value = s.readFloat();
        max_value = s.readFloat();
        current_value = s.readFloat();
        default_min_value = s.readFloat();
        default_max_value = s.readFloat();
        default_value = s.readFloat();
        name = s.readString();
        uint32_t mc = s.readUnsignedVarInt();
        modifiers.resize(mc);
        for (auto& m : modifiers) {
            m.id = s.readString();
            m.name = s.readString();
            m.amount = s.readFloat();
            m.operation = s.readSignedInt();
            m.operand = s.readSignedInt();
            m.serializable = s.readBool();
        }
    }
};

// ─── InventorySource ────────────────────────────────────────────
struct InventorySource {
    enum class Type : uint32_t {
        Invalid = 0xFFFFFFFF,
        Container = 0,
        Global = 1,
        WorldInteraction = 2,
        Creative = 3,
        CraftingContainer = 100,
        NonImplemented = 99999
    };

    Type source_type = Type::Invalid;
    int32_t container_id = 0;     // for Container type
    uint32_t bit_flags = 0;       // for WorldInteraction type

    void write(BinaryStream& s) const {
        s.writeUnsignedVarInt(static_cast<uint32_t>(source_type));
        switch (source_type) {
            case Type::Container:
            case Type::CraftingContainer:
                s.writeVarInt(container_id);
                break;
            case Type::WorldInteraction:
                s.writeUnsignedVarInt(bit_flags);
                break;
            default: break;
        }
    }
    void read(ReadOnlyBinaryStream& s) {
        source_type = static_cast<Type>(s.readUnsignedVarInt());
        switch (source_type) {
            case Type::Container:
            case Type::CraftingContainer:
                container_id = s.readVarInt();
                break;
            case Type::WorldInteraction:
                bit_flags = s.readUnsignedVarInt();
                break;
            default: break;
        }
    }
};

// ─── InventoryAction ────────────────────────────────────────────
struct InventoryAction {
    InventorySource source;
    uint32_t slot = 0;
    NetworkItemStackDescriptor from_item;
    NetworkItemStackDescriptor to_item;

    void write(BinaryStream& s) const {
        source.write(s);
        s.writeUnsignedVarInt(slot);
        from_item.write(s);
        to_item.write(s);
    }
    void read(ReadOnlyBinaryStream& s) {
        source.read(s);
        slot = s.readUnsignedVarInt();
        from_item.read(s);
        to_item.read(s);
    }
};

// ─── ScoreboardId ───────────────────────────────────────────────
struct ScoreboardId {
    int64_t raw_id = 0;

    void write(BinaryStream& s) const { s.writeVarInt64(raw_id); }
    void read(ReadOnlyBinaryStream& s) { raw_id = s.readVarInt64(); }
};

// ─── SubChunkPos ────────────────────────────────────────────────
struct SubChunkPos {
    int32_t x = 0, y = 0, z = 0;

    void write(BinaryStream& s) const {
        s.writeVarInt(x);
        s.writeVarInt(y);
        s.writeVarInt(z);
    }
    void read(ReadOnlyBinaryStream& s) {
        x = s.readVarInt();
        y = s.readVarInt();
        z = s.readVarInt();
    }
};

// ─── SubChunkPosOffset ──────────────────────────────────────────
struct SubChunkPosOffset {
    int8_t x = 0, y = 0, z = 0;

    void write(BinaryStream& s) const {
        s.writeSignedByte(x);
        s.writeSignedByte(y);
        s.writeSignedByte(z);
    }
    void read(ReadOnlyBinaryStream& s) {
        x = s.readSignedByte();
        y = s.readSignedByte();
        z = s.readSignedByte();
    }
};

// ─── ItemStackRequestSlotInfo ───────────────────────────────────
struct ItemStackRequestSlotInfo {
    FullContainerName container_name;
    uint8_t slot = 0;
    ItemStackNetIdVariant net_id_variant;

    void write(BinaryStream& s) const {
        container_name.write(s);
        s.writeByte(slot);
        net_id_variant.write(s);
    }
    void read(ReadOnlyBinaryStream& s) {
        container_name.read(s);
        slot = s.readByte();
        net_id_variant.read(s);
    }
};

// ─── MapDecoration ──────────────────────────────────────────────
struct MapDecoration {
    uint8_t type = 0;
    uint8_t rotation = 0;
    int32_t x = 0;
    int32_t y = 0;
    std::string label;
    uint32_t color = 0xFFFFFFFF;

    void write(BinaryStream& s) const {
        s.writeByte(type);
        s.writeByte(rotation);
        s.writeByte(static_cast<uint8_t>(x));
        s.writeByte(static_cast<uint8_t>(y));
        s.writeString(label);
        s.writeUnsignedVarInt(color);
    }
    void read(ReadOnlyBinaryStream& s) {
        type = s.readByte();
        rotation = s.readByte();
        x = s.readByte();
        y = s.readByte();
        label = s.readString();
        color = s.readUnsignedVarInt();
    }
};

// ─── MovementAttributesComponent ────────────────────────────────
struct MovementAttributesComponent {
    float movement_speed = 0.1f;
    float underwater_speed = 0.02f;
    float lava_speed = 0.02f;
    float jump_strength = 0.42f;
    float health = 20.0f;
    float hunger = 20.0f;

    void write(BinaryStream& s) const {
        s.writeFloat(movement_speed);
        s.writeFloat(underwater_speed);
        s.writeFloat(lava_speed);
        s.writeFloat(jump_strength);
        s.writeFloat(health);
        s.writeFloat(hunger);
    }
    void read(ReadOnlyBinaryStream& s) {
        movement_speed = s.readFloat();
        underwater_speed = s.readFloat();
        lava_speed = s.readFloat();
        jump_strength = s.readFloat();
        health = s.readFloat();
        hunger = s.readFloat();
    }
};

struct GameRule {
    std::string name;
    bool editable = true;
    uint32_t type = 0; // 1 = bool, 2 = int, 3 = float
    bool bool_value = false;
    int32_t int_value = 0;
    float float_value = 0.0f;
};

} // namespace cphertunnel
