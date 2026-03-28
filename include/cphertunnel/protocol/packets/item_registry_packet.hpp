// CpherTunnel - ItemRegistryPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ItemRegistryPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ItemRegistryPacket, ItemRegistry)

    // Item registry is a complex structure - stored as raw bytes
    ByteBuffer items_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(items_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        items_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
