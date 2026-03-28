// CpherTunnel - SubChunkRequestPacket (ID: 175)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {
class SubChunkRequestPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SubChunkRequestPacket, SubChunkRequest)
    int32_t dimension_type = 0;
    SubChunkPos center_pos;
    std::vector<SubChunkPosOffset> offsets;

    void write(BinaryStream& s) const override {
        s.writeVarInt(dimension_type);
        center_pos.write(s);
        s.writeUnsignedInt(static_cast<uint32_t>(offsets.size()));
        for (auto& o : offsets) o.write(s);
    }
    void read(ReadOnlyBinaryStream& s) override {
        dimension_type = s.readVarInt();
        center_pos.read(s);
        uint32_t count = s.readUnsignedInt();
        offsets.resize(count);
        for (auto& o : offsets) o.read(s);
    }
};
} // namespace cphertunnel
