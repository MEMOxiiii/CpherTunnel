// CpherTunnel - MovePlayerPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {


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
enum class MovePlayerMode : uint8_t {
    Normal    = 0,
    Reset     = 1,
    Teleport  = 2,
    Rotation  = 3
};

class MovePlayerPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(MovePlayerPacket, MovePlayer)

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

} // namespace cphertunnel
