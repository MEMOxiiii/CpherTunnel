// CpherTunnel - AvailableActorIdentifiersPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class AvailableActorIdentifiersPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(AvailableActorIdentifiersPacket, AvailableActorIdentifiers)

    // Contains a CompoundTag list of all actor identifiers
    ByteBuffer identifier_list_nbt;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(identifier_list_nbt);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        identifier_list_nbt = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
