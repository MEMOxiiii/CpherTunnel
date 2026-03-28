// CpherTunnel - BlockActorDataPacket
#pragma once
#include "../packet.hpp"
#include "../nbt.hpp"
namespace cphertunnel {

class BlockActorDataPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(BlockActorDataPacket, BlockActorData)

    NetworkBlockPos position;
    std::shared_ptr<NbtTag> nbt_data;

    void write(BinaryStream& stream) const override {
        stream.writeNetworkBlockPos(position);
        if (nbt_data) {
            NbtTag::writeNetworkNbt(stream, *nbt_data);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readNetworkBlockPos();
        nbt_data = NbtTag::readNetworkNbt(stream);
    }
};

} // namespace cphertunnel
