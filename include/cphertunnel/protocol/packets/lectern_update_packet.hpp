// CpherTunnel - LecternUpdatePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class LecternUpdatePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(LecternUpdatePacket, LecternUpdate)

    uint8_t page = 0;
    uint8_t total_pages = 0;
    NetworkBlockPos position;

    void write(BinaryStream& stream) const override {
        stream.writeByte(page);
        stream.writeByte(total_pages);
        stream.writeNetworkBlockPos(position);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        page = stream.readByte();
        total_pages = stream.readByte();
        position = stream.readNetworkBlockPos();
    }
};

} // namespace cphertunnel
