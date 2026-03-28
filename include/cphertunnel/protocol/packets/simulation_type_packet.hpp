// CpherTunnel - SimulationTypePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SimulationTypePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SimulationTypePacket, SimulationType)

    uint8_t simulation_type = 0; // 0=Game, 1=Editor, 2=Test, 3=Invalid

    void write(BinaryStream& stream) const override {
        stream.writeByte(simulation_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        simulation_type = stream.readByte();
    }
};

} // namespace cphertunnel
