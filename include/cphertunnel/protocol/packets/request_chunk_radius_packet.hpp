// CpherTunnel - RequestChunkRadiusPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class RequestChunkRadiusPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(RequestChunkRadiusPacket, RequestChunkRadius)

    int32_t chunk_radius = 8;
    uint8_t max_chunk_radius = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(chunk_radius);
        stream.writeByte(max_chunk_radius);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        chunk_radius = stream.readVarInt();
        max_chunk_radius = stream.readByte();
    }
};

} // namespace cphertunnel
