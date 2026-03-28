// CpherTunnel - UnlockedRecipesPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class UnlockedRecipesPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(UnlockedRecipesPacket, UnlockedRecipes)

    uint32_t unlock_type = 0;
    std::vector<std::string> recipes;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedInt(unlock_type);
        stream.writeUnsignedVarInt(static_cast<uint32_t>(recipes.size()));
        for (auto& r : recipes) stream.writeString(r);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        unlock_type = stream.readUnsignedInt();
        uint32_t count = stream.readUnsignedVarInt();
        recipes.resize(count);
        for (auto& r : recipes) r = stream.readString();
    }
};

} // namespace cphertunnel
