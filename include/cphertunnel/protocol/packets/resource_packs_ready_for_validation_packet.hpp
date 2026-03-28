// CpherTunnel - ResourcePacksReadyForValidationPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ResourcePacksReadyForValidationPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ResourcePacksReadyForValidationPacket, ResourcePacksReadyForValidation)

    void write(BinaryStream&) const override {}
    void read(ReadOnlyBinaryStream&) override {}
};

} // namespace cphertunnel
