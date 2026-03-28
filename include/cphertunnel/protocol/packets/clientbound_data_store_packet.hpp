// CpherTunnel - ClientboundDataStorePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ClientboundDataStorePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ClientboundDataStorePacket, ClientboundDataStore)

    ByteBuffer store_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(store_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        store_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
