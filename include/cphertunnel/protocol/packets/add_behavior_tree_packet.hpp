// CpherTunnel - AddBehaviorTreePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class AddBehaviorTreePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(AddBehaviorTreePacket, AddBehaviorTree)

    std::string behavior_tree_json;

    void write(BinaryStream& stream) const override {
        stream.writeString(behavior_tree_json);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        behavior_tree_json = stream.readString();
    }
};

} // namespace cphertunnel
