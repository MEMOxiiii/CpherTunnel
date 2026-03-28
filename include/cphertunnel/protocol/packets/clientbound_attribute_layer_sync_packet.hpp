// CpherTunnel - ClientboundAttributeLayerSyncPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ClientboundAttributeLayerSyncPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ClientboundAttributeLayerSyncPacket, ClientboundAttributeLayerSync)

    ByteBuffer attribute_data; // Complex oneOf variant structure

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(attribute_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        attribute_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
