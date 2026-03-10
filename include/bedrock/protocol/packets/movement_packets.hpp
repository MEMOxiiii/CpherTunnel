// =============================================================================
// BedrockProtocol - Movement Packets
// Player and entity movement, position correction
// =============================================================================
#pragma once

#include "../packet.hpp"

namespace bedrock {

// ─── Player Action Enums ─────────────────────────────────────────
enum class PlayerActionType : int32_t {
    StartBreak            = 0,
    AbortBreak            = 1,
    StopBreak             = 2,
    GetUpdatedBlock       = 3,
    DropItem              = 4,
    StartSleeping         = 5,
    StopSleeping          = 6,
    Respawn               = 7,
    Jump                  = 8,
    StartSprint           = 9,
    StopSprint            = 10,
    StartSneak            = 11,
    StopSneak             = 12,
    CreativePlayerDestroyBlock = 13,
    DimensionChangeAck    = 14,
    StartGlide            = 15,
    StopGlide             = 16,
    BuildDenied           = 17,
    CrackBreak            = 18,
    ChangeSkin            = 19,
    SetEnchantmentSeed_DEPRECATED = 20,
    StartSwimming         = 21,
    StopSwimming          = 22,
    StartSpinAttack       = 23,
    StopSpinAttack        = 24,
    InteractBlock         = 25,
    PredictDestroyBlock   = 26,
    ContinueDestroyBlock  = 27,
    StartItemUseOn        = 28,
    StopItemUseOn         = 29,
    HandledTeleport       = 30,
    MissedSwing           = 31,
    StartCrawling         = 32,
    StopCrawling          = 33,
    StartFlying           = 34,
    StopFlying            = 35,
    ClientAckServerData   = 36,
    StartUsingItem        = 37,
};

// ─── MovePlayer Mode ────────────────────────────────────────────
enum class MovePlayerMode : uint8_t {
    Normal    = 0,
    Reset     = 1,
    Teleport  = 2,
    Rotation  = 3
};

// =============================================================================
// MovePlayerPacket (ID: 19)
// =============================================================================
class MovePlayerPacket : public Packet {
public:
    BEDROCK_PACKET(MovePlayerPacket, MovePlayer)

    ActorRuntimeID runtime_id;
    Vec3 position;
    Vec2 rotation; // pitch, yaw
    float head_yaw = 0.0f;
    MovePlayerMode mode = MovePlayerMode::Normal;
    bool on_ground = false;
    ActorRuntimeID riding_runtime_id;
    // Only for Teleport mode
    int32_t teleport_cause = 0;
    int32_t teleport_source_type = 0;
    uint64_t tick = 0;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(runtime_id);
        stream.writeVec3(position);
        stream.writeVec2(rotation);
        stream.writeFloat(head_yaw);
        stream.writeByte(static_cast<uint8_t>(mode));
        stream.writeBool(on_ground);
        stream.writeActorRuntimeID(riding_runtime_id);
        if (mode == MovePlayerMode::Teleport) {
            stream.writeSignedInt(teleport_cause);
            stream.writeSignedInt(teleport_source_type);
        }
        stream.writeUnsignedVarInt64(tick);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        runtime_id = stream.readActorRuntimeID();
        position = stream.readVec3();
        rotation = stream.readVec2();
        head_yaw = stream.readFloat();
        mode = static_cast<MovePlayerMode>(stream.readByte());
        on_ground = stream.readBool();
        riding_runtime_id = stream.readActorRuntimeID();
        if (mode == MovePlayerMode::Teleport) {
            teleport_cause = stream.readSignedInt();
            teleport_source_type = stream.readSignedInt();
        }
        tick = stream.readUnsignedVarInt64();
    }
};

// =============================================================================
// MoveActorAbsolutePacket (ID: 18)
// =============================================================================
class MoveActorAbsolutePacket : public Packet {
public:
    BEDROCK_PACKET(MoveActorAbsolutePacket, MoveAbsoluteActor)

    ActorRuntimeID runtime_id;
    uint16_t flags = 0; // bit 0=on_ground, bit 1=teleported, bit 2=force_move
    Vec3 position;
    uint8_t rotation_x = 0; // byte rotation (angle / 360 * 256)
    uint8_t rotation_y = 0;
    uint8_t rotation_z = 0;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(runtime_id);
        stream.writeUnsignedShort(flags);
        stream.writeVec3(position);
        stream.writeByte(rotation_x);
        stream.writeByte(rotation_y);
        stream.writeByte(rotation_z);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        runtime_id = stream.readActorRuntimeID();
        flags = stream.readUnsignedShort();
        position = stream.readVec3();
        rotation_x = stream.readByte();
        rotation_y = stream.readByte();
        rotation_z = stream.readByte();
    }
};

// =============================================================================
// PlayerActionPacket (ID: 36)
// =============================================================================
class PlayerActionPacket : public Packet {
public:
    BEDROCK_PACKET(PlayerActionPacket, PlayerAction)

    ActorRuntimeID runtime_id;
    PlayerActionType action = PlayerActionType::StartBreak;
    NetworkBlockPos block_position;
    NetworkBlockPos result_position;
    int32_t face = 0;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(runtime_id);
        stream.writeVarInt(static_cast<int32_t>(action));
        stream.writeNetworkBlockPos(block_position);
        stream.writeNetworkBlockPos(result_position);
        stream.writeVarInt(face);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        runtime_id = stream.readActorRuntimeID();
        action = static_cast<PlayerActionType>(stream.readVarInt());
        block_position = stream.readNetworkBlockPos();
        result_position = stream.readNetworkBlockPos();
        face = stream.readVarInt();
    }
};

// =============================================================================
// CorrectPlayerMovePredictionPacket (ID: 161)
// =============================================================================
class CorrectPlayerMovePredictionPacket : public Packet {
public:
    BEDROCK_PACKET(CorrectPlayerMovePredictionPacket, CorrectPlayerMovePrediction)

    uint8_t prediction_type = 0;
    Vec3 position;
    Vec3 delta;
    bool on_ground = false;
    uint64_t tick = 0;
    Vec2 rotation;

    void write(BinaryStream& stream) const override {
        stream.writeByte(prediction_type);
        stream.writeVec3(position);
        stream.writeVec3(delta);
        stream.writeBool(on_ground);
        stream.writeUnsignedVarInt64(tick);
        if (prediction_type == 1) { // vehicle
            stream.writeVec2(rotation);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        prediction_type = stream.readByte();
        position = stream.readVec3();
        delta = stream.readVec3();
        on_ground = stream.readBool();
        tick = stream.readUnsignedVarInt64();
        if (prediction_type == 1) {
            rotation = stream.readVec2();
        }
    }
};

// =============================================================================
// MotionPredictionHintsPacket (ID: 157)
// =============================================================================
class MotionPredictionHintsPacket : public Packet {
public:
    BEDROCK_PACKET(MotionPredictionHintsPacket, MotionPredictionHints)

    ActorRuntimeID entity_runtime_id;
    Vec3 velocity;
    bool on_ground = false;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(entity_runtime_id);
        stream.writeVec3(velocity);
        stream.writeBool(on_ground);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_runtime_id = stream.readActorRuntimeID();
        velocity = stream.readVec3();
        on_ground = stream.readBool();
    }
};

// =============================================================================
// ServerPlayerPostMovePositionPacket (ID: 16)
// =============================================================================
class ServerPlayerPostMovePositionPacket : public Packet {
public:
    BEDROCK_PACKET(ServerPlayerPostMovePositionPacket, ServerPlayerPostMovePosition)

    Vec3 position;
    ActorRuntimeID player_runtime_id;

    void write(BinaryStream& stream) const override {
        stream.writeVec3(position);
        stream.writeActorRuntimeID(player_runtime_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readVec3();
        player_runtime_id = stream.readActorRuntimeID();
    }
};

// =============================================================================
// MovementEffectPacket (ID: 318)
// =============================================================================
class MovementEffectPacket : public Packet {
public:
    BEDROCK_PACKET(MovementEffectPacket, MovementEffect)

    ActorRuntimeID entity_runtime_id;
    uint32_t effect_type = 0;
    int32_t duration = 0;
    uint64_t tick = 0;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(entity_runtime_id);
        stream.writeUnsignedVarInt(effect_type);
        stream.writeVarInt(duration);
        stream.writeUnsignedVarInt64(tick);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_runtime_id = stream.readActorRuntimeID();
        effect_type = stream.readUnsignedVarInt();
        duration = stream.readVarInt();
        tick = stream.readUnsignedVarInt64();
    }
};

} // namespace bedrock
