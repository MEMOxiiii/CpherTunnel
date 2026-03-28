// CpherTunnel - SyncActorPropertyPacket
#pragma once
#include "../packet.hpp"
#include "../nbt.hpp"
namespace cphertunnel {

class SyncActorPropertyPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SyncActorPropertyPacket, SyncActorProperty)

    std::shared_ptr<NbtTag> property_data;

    void write(BinaryStream& stream) const override {
        if (property_data) {
            NbtTag::writeNetworkNbt(stream, *property_data);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        property_data = NbtTag::readNetworkNbt(stream);
    }
};

} // namespace cphertunnel
