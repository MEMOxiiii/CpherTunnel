// =============================================================================
// BedrockProtocol - Entity/Actor Packets
// Adding, removing, updating entities
// =============================================================================
#pragma once

#include "../packet.hpp"
#include "../nbt.hpp"

namespace bedrock {

// ─── Enums ───────────────────────────────────────────────────────
enum class ActorEventType : uint8_t {
    Jump                   = 1,
    HurtAnimation          = 2,
    DeathAnimation         = 3,
    ArmSwing               = 4,
    StopAttack             = 5,
    TameFail               = 6,
    TameSuccess            = 7,
    ShakeWet               = 8,
    UseItem                = 9,
    EatGrass               = 10,
    FishhookBubble         = 11,
    FishhookPosition       = 12,
    FishhookHook           = 13,
    FishhookTease          = 14,
    SquidInkCloud          = 15,
    ZombieVillagerCure     = 16,
    Respawn                = 18,
    IronGolemOfferFlower   = 19,
    IronGolemWithdrawFlower = 20,
    LoveParticles          = 21,
    VillagerAngry          = 22,
    VillagerHappy          = 23,
    WitchSpell             = 24,
    FireworkParticles      = 25,
    InLoveParticles        = 26,
    SilverfishSpawnAnimation = 27,
    GuardianAttack         = 28,
    WitchDrinkPotion       = 29,
    WitchThrowPotion       = 30,
    MinecartTntPrimeFuse   = 31,
    PlayerAddXpLevels      = 34,
    ElderGuardianCurse     = 35,
    AgentArmSwing          = 36,
    EnderDragonDeath       = 37,
    DustParticles          = 38,
    ArrowShake             = 39,
};

enum class InteractAction : uint8_t {
    Invalid          = 0,
    StopRiding       = 3,
    InteractUpdate   = 4,
    NpcOpen          = 5,
    OpenInventory    = 6
};

// =============================================================================
// RemoveActorPacket (ID: 14)
// =============================================================================
class RemoveActorPacket : public Packet {
public:
    BEDROCK_PACKET(RemoveActorPacket, RemoveActor)

    ActorUniqueID entity_id;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(entity_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_id = stream.readActorUniqueID();
    }
};

// =============================================================================
// ActorEventPacket (ID: 27)
// =============================================================================
class ActorEventPacket : public Packet {
public:
    BEDROCK_PACKET(ActorEventPacket, ActorEvent)

    ActorRuntimeID runtime_id;
    uint8_t event_id = 0;
    int32_t data = 0;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(runtime_id);
        stream.writeByte(event_id);
        stream.writeVarInt(data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        runtime_id = stream.readActorRuntimeID();
        event_id = stream.readByte();
        data = stream.readVarInt();
    }
};

// =============================================================================
// MobEffectPacket (ID: 28)
// =============================================================================
class MobEffectPacket : public Packet {
public:
    BEDROCK_PACKET(MobEffectPacket, MobEffect)

    ActorRuntimeID runtime_id;
    uint8_t event_id = 0; // 1=add, 2=modify, 3=remove
    int32_t effect_id = 0;
    int32_t amplifier = 0;
    bool particles = true;
    int32_t duration = 0;
    uint64_t tick = 0;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(runtime_id);
        stream.writeByte(event_id);
        stream.writeVarInt(effect_id);
        stream.writeVarInt(amplifier);
        stream.writeBool(particles);
        stream.writeVarInt(duration);
        stream.writeUnsignedVarInt64(tick);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        runtime_id = stream.readActorRuntimeID();
        event_id = stream.readByte();
        effect_id = stream.readVarInt();
        amplifier = stream.readVarInt();
        particles = stream.readBool();
        duration = stream.readVarInt();
        tick = stream.readUnsignedVarInt64();
    }
};

// =============================================================================
// SetActorMotionPacket (ID: 40)
// =============================================================================
class SetActorMotionPacket : public Packet {
public:
    BEDROCK_PACKET(SetActorMotionPacket, SetActorMotion)

    ActorRuntimeID runtime_id;
    Vec3 motion;
    uint64_t tick = 0;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(runtime_id);
        stream.writeVec3(motion);
        stream.writeUnsignedVarInt64(tick);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        runtime_id = stream.readActorRuntimeID();
        motion = stream.readVec3();
        tick = stream.readUnsignedVarInt64();
    }
};

// =============================================================================
// SetActorLinkPacket (ID: 41)
// =============================================================================
class SetActorLinkPacket : public Packet {
public:
    BEDROCK_PACKET(SetActorLinkPacket, SetActorLink)

    ActorUniqueID ridden_id;
    ActorUniqueID rider_id;
    uint8_t type = 0; // 0=remove, 1=ride, 2=passenger
    bool immediate = false;
    bool rider_initiated = false;
    float vehicle_angular_velocity = 0.0f;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(ridden_id);
        stream.writeActorUniqueID(rider_id);
        stream.writeByte(type);
        stream.writeBool(immediate);
        stream.writeBool(rider_initiated);
        stream.writeFloat(vehicle_angular_velocity);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        ridden_id = stream.readActorUniqueID();
        rider_id = stream.readActorUniqueID();
        type = stream.readByte();
        immediate = stream.readBool();
        rider_initiated = stream.readBool();
        vehicle_angular_velocity = stream.readFloat();
    }
};

// =============================================================================
// InteractPacket (ID: 33)
// =============================================================================
class InteractPacket : public Packet {
public:
    BEDROCK_PACKET(InteractPacket, Interact)

    uint8_t action = 0;
    ActorRuntimeID target_runtime_id;
    Vec3 position; // only for InteractUpdate

    void write(BinaryStream& stream) const override {
        stream.writeByte(action);
        stream.writeActorRuntimeID(target_runtime_id);
        if (action == static_cast<uint8_t>(InteractAction::InteractUpdate)) {
            stream.writeVec3(position);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action = stream.readByte();
        target_runtime_id = stream.readActorRuntimeID();
        if (action == static_cast<uint8_t>(InteractAction::InteractUpdate)) {
            position = stream.readVec3();
        }
    }
};

// =============================================================================
// TakeItemActorPacket (ID: 17)
// =============================================================================
class TakeItemActorPacket : public Packet {
public:
    BEDROCK_PACKET(TakeItemActorPacket, TakeItemActor)

    ActorRuntimeID item_runtime_id;
    ActorRuntimeID player_runtime_id;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(item_runtime_id);
        stream.writeActorRuntimeID(player_runtime_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        item_runtime_id = stream.readActorRuntimeID();
        player_runtime_id = stream.readActorRuntimeID();
    }
};

// =============================================================================
// AnimatePacket (ID: 44)
// =============================================================================
class AnimatePacket : public Packet {
public:
    BEDROCK_PACKET(AnimatePacket, Animate)

    int32_t action = 0;
    ActorRuntimeID runtime_id;
    float row_boat = 0.0f; // only for RowRight/RowLeft

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(action);
        stream.writeActorRuntimeID(runtime_id);
        if (action == 128 || action == 129) { // RowRight, RowLeft
            stream.writeFloat(row_boat);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action = stream.readVarInt();
        runtime_id = stream.readActorRuntimeID();
        if (action == 128 || action == 129) {
            row_boat = stream.readFloat();
        }
    }
};

// =============================================================================
// HurtArmorPacket (ID: 38)
// =============================================================================
class HurtArmorPacket : public Packet {
public:
    BEDROCK_PACKET(HurtArmorPacket, HurtArmor)

    int32_t cause = 0;
    int32_t damage = 0;
    uint64_t armor_slots = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(cause);
        stream.writeVarInt(damage);
        stream.writeUnsignedVarInt64(armor_slots);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        cause = stream.readVarInt();
        damage = stream.readVarInt();
        armor_slots = stream.readUnsignedVarInt64();
    }
};

// =============================================================================
// PlayerArmorDamagePacket (ID: 149)
// =============================================================================
class PlayerArmorDamagePacket : public Packet {
public:
    BEDROCK_PACKET(PlayerArmorDamagePacket, PlayerArmorDamage)

    uint8_t damage_bits = 0;
    int32_t helmet_damage = 0;
    int32_t chestplate_damage = 0;
    int32_t leggings_damage = 0;
    int32_t boots_damage = 0;
    int32_t body_damage = 0;

    void write(BinaryStream& stream) const override {
        stream.writeByte(damage_bits);
        if (damage_bits & 0x01) stream.writeVarInt(helmet_damage);
        if (damage_bits & 0x02) stream.writeVarInt(chestplate_damage);
        if (damage_bits & 0x04) stream.writeVarInt(leggings_damage);
        if (damage_bits & 0x08) stream.writeVarInt(boots_damage);
        if (damage_bits & 0x10) stream.writeVarInt(body_damage);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        damage_bits = stream.readByte();
        if (damage_bits & 0x01) helmet_damage = stream.readVarInt();
        if (damage_bits & 0x02) chestplate_damage = stream.readVarInt();
        if (damage_bits & 0x04) leggings_damage = stream.readVarInt();
        if (damage_bits & 0x08) boots_damage = stream.readVarInt();
        if (damage_bits & 0x10) body_damage = stream.readVarInt();
    }
};

// =============================================================================
// AnimateEntityPacket (ID: 158)
// =============================================================================
class AnimateEntityPacket : public Packet {
public:
    BEDROCK_PACKET(AnimateEntityPacket, TriggerAnimation)

    std::string animation;
    std::string next_state;
    std::string stop_expression;
    int32_t stop_expression_version = 0;
    std::string controller;
    float blend_out_time = 0.0f;
    std::vector<ActorRuntimeID> runtime_ids;

    void write(BinaryStream& stream) const override {
        stream.writeString(animation);
        stream.writeString(next_state);
        stream.writeString(stop_expression);
        stream.writeVarInt(stop_expression_version);
        stream.writeString(controller);
        stream.writeFloat(blend_out_time);
        stream.writeUnsignedVarInt(static_cast<uint32_t>(runtime_ids.size()));
        for (auto& id : runtime_ids) {
            stream.writeActorRuntimeID(id);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        animation = stream.readString();
        next_state = stream.readString();
        stop_expression = stream.readString();
        stop_expression_version = stream.readVarInt();
        controller = stream.readString();
        blend_out_time = stream.readFloat();
        uint32_t count = stream.readUnsignedVarInt();
        runtime_ids.resize(count);
        for (auto& id : runtime_ids) {
            id = stream.readActorRuntimeID();
        }
    }
};

// =============================================================================
// SyncActorPropertyPacket (ID: 165)
// =============================================================================
class SyncActorPropertyPacket : public Packet {
public:
    BEDROCK_PACKET(SyncActorPropertyPacket, SyncActorProperty)

    std::shared_ptr<NbtTag> property_data;

    void write(BinaryStream& stream) const override {
        if (property_data) {
            NbtTag::writeNetworkNbt(stream, *property_data);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        property_data = NbtTag::readNetworkNbt(stream);
    }
};

// =============================================================================
// SetLastHurtByPacket (ID: 96)
// =============================================================================
class SetLastHurtByPacket : public Packet {
public:
    BEDROCK_PACKET(SetLastHurtByPacket, SetLastHurtBy)

    int32_t entity_type_id = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(entity_type_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_type_id = stream.readVarInt();
    }
};

// =============================================================================
// AddPaintingPacket (ID: 22)
// =============================================================================
class AddPaintingPacket : public Packet {
public:
    BEDROCK_PACKET(AddPaintingPacket, AddPainting)

    ActorUniqueID entity_id;
    ActorRuntimeID runtime_id;
    Vec3 position;
    int32_t direction = 0;
    std::string motive;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(entity_id);
        stream.writeActorRuntimeID(runtime_id);
        stream.writeVec3(position);
        stream.writeVarInt(direction);
        stream.writeString(motive);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_id = stream.readActorUniqueID();
        runtime_id = stream.readActorRuntimeID();
        position = stream.readVec3();
        direction = stream.readVarInt();
        motive = stream.readString();
    }
};

// =============================================================================
// ChangeMobPropertyPacket (ID: 182)
// =============================================================================
class ChangeMobPropertyPacket : public Packet {
public:
    BEDROCK_PACKET(ChangeMobPropertyPacket, ChangeMobProperty)

    ActorUniqueID entity_unique_id;
    std::string property;
    bool bool_value = false;
    std::string string_value;
    int32_t int_value = 0;
    float float_value = 0.0f;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(entity_unique_id);
        stream.writeString(property);
        stream.writeBool(bool_value);
        stream.writeString(string_value);
        stream.writeVarInt(int_value);
        stream.writeFloat(float_value);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_unique_id = stream.readActorUniqueID();
        property = stream.readString();
        bool_value = stream.readBool();
        string_value = stream.readString();
        int_value = stream.readVarInt();
        float_value = stream.readFloat();
    }
};

// =============================================================================
// AddVolumeEntityPacket (ID: 166)
// =============================================================================
class AddVolumeEntityPacket : public Packet {
public:
    BEDROCK_PACKET(AddVolumeEntityPacket, AddVolumeEntity)

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

// =============================================================================
// RemoveVolumeEntityPacket (ID: 167)
// =============================================================================
class RemoveVolumeEntityPacket : public Packet {
public:
    BEDROCK_PACKET(RemoveVolumeEntityPacket, RemoveVolumeEntity)

    uint32_t entity_net_id = 0;
    int32_t dimension_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(entity_net_id);
        stream.writeVarInt(dimension_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_net_id = stream.readUnsignedVarInt();
        dimension_type = stream.readVarInt();
    }
};

// =============================================================================
// AvailableActorIdentifiersPacket (ID: 119)
// =============================================================================
class AvailableActorIdentifiersPacket : public Packet {
public:
    BEDROCK_PACKET(AvailableActorIdentifiersPacket, AvailableActorIdentifiers)

    // Contains a CompoundTag list of all actor identifiers
    ByteBuffer identifier_list_nbt;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(identifier_list_nbt);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        identifier_list_nbt = stream.readRemainingBytes();
    }
};

// =============================================================================
// AgentActionEventPacket (ID: 181)
// =============================================================================
class AgentActionEventPacket : public Packet {
public:
    BEDROCK_PACKET(AgentActionEventPacket, AgentAction)

    enum class AgentAction : int32_t {
        None              = 0,
        Attack            = 1,
        Collect           = 2,
        Destroy           = 3,
        DetectRedstone    = 4,
        DetectObstacle    = 5,
        Drop              = 6,
        DropAll           = 7,
        Inspect           = 8,
        InspectData       = 9,
        InspectItem       = 10,
        InspectItemDetail = 11,
        InspectItemCount  = 12,
        InspectItemSpace  = 13,
        Interact          = 14,
        Move              = 15,
        PlaceBlock        = 16,
        Till              = 17,
        TransferTo        = 18,
        Turn              = 19,
    };

    std::string request_id;
    AgentAction action = AgentAction::None;
    std::string response;

    void write(BinaryStream& stream) const override {
        stream.writeString(request_id);
        stream.writeVarInt(static_cast<int32_t>(action));
        stream.writeString(response);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        request_id = stream.readString();
        action = static_cast<AgentAction>(stream.readVarInt());
        response = stream.readString();
    }
};

// =============================================================================
// AgentAnimationPacket (ID: 304)
// =============================================================================
class AgentAnimationPacket : public Packet {
public:
    BEDROCK_PACKET(AgentAnimationPacket, AgentAnimation)

    enum class AnimationType : uint8_t {
        ArmSwing    = 0,
        Shrug       = 1,
    };

    AnimationType animation = AnimationType::ArmSwing;
    ActorRuntimeID runtime_id;

    void write(BinaryStream& stream) const override {
        stream.writeByte(static_cast<uint8_t>(animation));
        stream.writeActorRuntimeID(runtime_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        animation = static_cast<AnimationType>(stream.readByte());
        runtime_id = stream.readActorRuntimeID();
    }
};

} // namespace bedrock
