// CpherTunnel - LevelEventGenericPacket
#pragma once
#include "../packet.hpp"
#include "../nbt.hpp"
namespace cphertunnel {

class LevelEventGenericPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(LevelEventGenericPacket, LevelEventGeneric)

    int32_t event_id = 0;
    std::shared_ptr<NbtTag> event_data;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(event_id);
        if (event_data) {
            NbtTag::writeNetworkNbt(stream, *event_data);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        event_id = stream.readVarInt();
        event_data = NbtTag::readNetworkNbt(stream);
    }
};

} // namespace cphertunnel
