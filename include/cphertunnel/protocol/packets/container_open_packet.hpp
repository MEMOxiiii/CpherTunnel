// CpherTunnel - ContainerOpenPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ContainerOpenPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ContainerOpenPacket, ContainerOpen)

    uint8_t window_id = 0;
    uint8_t type = 0;
    NetworkBlockPos position;
    ActorUniqueID entity_unique_id;

    void write(BinaryStream& stream) const override {
        stream.writeByte(window_id);
        stream.writeByte(type);
        stream.writeNetworkBlockPos(position);
        stream.writeActorUniqueID(entity_unique_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        window_id = stream.readByte();
        type = stream.readByte();
        position = stream.readNetworkBlockPos();
        entity_unique_id = stream.readActorUniqueID();
    }
};

} // namespace cphertunnel
