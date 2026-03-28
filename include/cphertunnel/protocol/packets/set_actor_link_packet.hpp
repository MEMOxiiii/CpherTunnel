// CpherTunnel - SetActorLinkPacket
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {

class SetActorLinkPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SetActorLinkPacket, SetActorLink)

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

} // namespace cphertunnel
