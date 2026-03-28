// CpherTunnel - TransferPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class TransferPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(TransferPacket, Transfer)

    std::string server_address;
    uint16_t server_port = 19132;
    bool reload_world = false;

    void write(BinaryStream& stream) const override {
        stream.writeString(server_address);
        stream.writeUnsignedShort(server_port);
        stream.writeBool(reload_world);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        server_address = stream.readString();
        server_port = stream.readUnsignedShort();
        reload_world = stream.readBool();
    }
};

} // namespace cphertunnel
