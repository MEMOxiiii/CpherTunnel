// CpherTunnel - AgentAnimationPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class AgentAnimationPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(AgentAnimationPacket, AgentAnimation)

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

} // namespace cphertunnel
