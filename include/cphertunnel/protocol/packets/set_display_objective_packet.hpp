// CpherTunnel - SetDisplayObjectivePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SetDisplayObjectivePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SetDisplayObjectivePacket, SetDisplayObjective)

    std::string display_slot;
    std::string objective_name;
    std::string display_name;
    std::string criteria_name;
    int32_t sort_order = 0;

    void write(BinaryStream& stream) const override {
        stream.writeString(display_slot);
        stream.writeString(objective_name);
        stream.writeString(display_name);
        stream.writeString(criteria_name);
        stream.writeVarInt(sort_order);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        display_slot = stream.readString();
        objective_name = stream.readString();
        display_name = stream.readString();
        criteria_name = stream.readString();
        sort_order = stream.readVarInt();
    }
};

} // namespace cphertunnel
