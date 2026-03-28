// CpherTunnel - SyncWorldClocksPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SyncWorldClocksPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SyncWorldClocksPacket, SyncWorldClocks)

    ByteBuffer clock_data; // Complex oneOf variant structure

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(clock_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        clock_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
