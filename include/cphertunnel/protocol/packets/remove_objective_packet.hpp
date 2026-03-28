// CpherTunnel - RemoveObjectivePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class RemoveObjectivePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(RemoveObjectivePacket, RemoveObjective)

    std::string objective_name;

    void write(BinaryStream& stream) const override {
        stream.writeString(objective_name);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        objective_name = stream.readString();
    }
};

} // namespace cphertunnel
