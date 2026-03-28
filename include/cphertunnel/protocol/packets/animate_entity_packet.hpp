// CpherTunnel - AnimateEntityPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class AnimateEntityPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(AnimateEntityPacket, TriggerAnimation)

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

} // namespace cphertunnel
