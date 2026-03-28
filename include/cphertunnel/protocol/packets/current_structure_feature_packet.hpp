// CpherTunnel - CurrentStructureFeaturePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class CurrentStructureFeaturePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CurrentStructureFeaturePacket, CurrentStructureFeature)

    std::string current_structure_feature;

    void write(BinaryStream& stream) const override {
        stream.writeString(current_structure_feature);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        current_structure_feature = stream.readString();
    }
};

} // namespace cphertunnel
