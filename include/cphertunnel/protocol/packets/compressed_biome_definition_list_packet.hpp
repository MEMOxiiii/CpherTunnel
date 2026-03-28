// CpherTunnel - CompressedBiomeDefinitionListPacket (ID: 301)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class CompressedBiomeDefinitionListPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CompressedBiomeDefinitionListPacket, CompressedBiomeDefinitionList)
    ByteBuffer raw_payload; // compressed NBT data
    void write(BinaryStream& s) const override { s.writeRawBytes(raw_payload); }
    void read(ReadOnlyBinaryStream& s) override { raw_payload = s.readRemainingBytes(); }
};
} // namespace cphertunnel
