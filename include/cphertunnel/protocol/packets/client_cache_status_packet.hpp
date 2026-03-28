// CpherTunnel - ClientCacheStatusPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ClientCacheStatusPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ClientCacheStatusPacket, ClientCacheStatus)

    bool cache_supported = false;

    void write(BinaryStream& stream) const override {
        stream.writeBool(cache_supported);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        cache_supported = stream.readBool();
    }
};

} // namespace cphertunnel
