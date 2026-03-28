// CpherTunnel - TickingAreasLoadStatusPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class TickingAreasLoadStatusPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(TickingAreasLoadStatusPacket, TickingAreasLoadStatus)

    bool waiting_for_preload = false;

    void write(BinaryStream& stream) const override {
        stream.writeBool(waiting_for_preload);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        waiting_for_preload = stream.readBool();
    }
};

} // namespace cphertunnel
