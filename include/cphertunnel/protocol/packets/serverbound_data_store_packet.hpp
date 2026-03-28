// CpherTunnel - ServerboundDataStorePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ServerboundDataStorePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ServerboundDataStorePacket, ServerboundDataStore)

    ByteBuffer store_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(store_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        store_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
