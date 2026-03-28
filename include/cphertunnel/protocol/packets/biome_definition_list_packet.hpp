// CpherTunnel - BiomeDefinitionListPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class BiomeDefinitionListPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(BiomeDefinitionListPacket, BiomeDefinitionList)

    // Raw bytes for the full packet body, since biome definition data
    // is a complex structured format (not just NBT since v800)
    ByteBuffer raw_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(raw_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        raw_data = stream.readRemainingBytes();
    }

    // Helper: build an empty biome definition list (0 biomes, 0 strings)
    static BiomeDefinitionListPacket makeEmpty() {
        BiomeDefinitionListPacket pkt;
        BinaryStream bs;
        bs.writeUnsignedVarInt(0);  // map size: 0 biomes
        bs.writeUnsignedVarInt(0);  // string list size: 0 strings
        auto& buf = bs.getBuffer();
        pkt.raw_data.assign(buf.begin(), buf.end());
        return pkt;
    }
};

} // namespace cphertunnel
