// CpherTunnel - DimensionDataPacket (ID: 180)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

struct DimensionDefinition {
    std::string name;
    int32_t height_max = 256;
    int32_t height_min = -64;
    int32_t generator_type = 0;
};

class DimensionDataPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(DimensionDataPacket, DimensionData)
    std::vector<DimensionDefinition> definitions;

    void write(BinaryStream& s) const override {
        s.writeUnsignedVarInt(static_cast<uint32_t>(definitions.size()));
        for (auto& d : definitions) {
            s.writeString(d.name);
            s.writeVarInt(d.height_max);
            s.writeVarInt(d.height_min);
            s.writeVarInt(d.generator_type);
        }
    }
    void read(ReadOnlyBinaryStream& s) override {
        uint32_t count = s.readUnsignedVarInt();
        definitions.resize(count);
        for (auto& d : definitions) {
            d.name = s.readString();
            d.height_max = s.readVarInt();
            d.height_min = s.readVarInt();
            d.generator_type = s.readVarInt();
        }
    }
};
} // namespace cphertunnel
