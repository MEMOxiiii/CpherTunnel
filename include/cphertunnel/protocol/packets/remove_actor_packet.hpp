// CpherTunnel - RemoveActorPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {


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

class RemoveActorPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(RemoveActorPacket, RemoveActor)

    ActorUniqueID entity_id;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(entity_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_id = stream.readActorUniqueID();
    }
};

} // namespace cphertunnel
