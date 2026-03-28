// CpherTunnel - FeatureRegistryPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class FeatureRegistryPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(FeatureRegistryPacket, FeatureRegistry)

    struct FeatureEntry {
        std::string feature_name;
        std::string binary_json_output;
    };

    std::vector<FeatureEntry> features;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(static_cast<uint32_t>(features.size()));
        for (const auto& f : features) {
            stream.writeString(f.feature_name);
            stream.writeString(f.binary_json_output);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        uint32_t count = stream.readUnsignedVarInt();
        features.resize(count);
        for (auto& f : features) {
            f.feature_name = stream.readString();
            f.binary_json_output = stream.readString();
        }
    }
};

} // namespace cphertunnel
