// CpherTunnel - NpcRequestPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class NpcRequestPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(NpcRequestPacket, NpcRequest)

    ActorRuntimeID entity_runtime_id;
    uint8_t request_type = 0;
    std::string command;
    uint8_t action_type = 0;
    std::string scene_name;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(entity_runtime_id);
        stream.writeByte(request_type);
        stream.writeString(command);
        stream.writeByte(action_type);
        stream.writeString(scene_name);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_runtime_id = stream.readActorRuntimeID();
        request_type = stream.readByte();
        command = stream.readString();
        action_type = stream.readByte();
        scene_name = stream.readString();
    }
};

} // namespace cphertunnel
