// CpherTunnel - TrimDataPacket (ID: 302)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class TrimDataPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(TrimDataPacket, TrimData)

    struct TrimPattern { std::string item_name; std::string pattern_id; };
    struct TrimMaterial { std::string material_id; std::string color; std::string item_name; };

    std::vector<TrimPattern> patterns;
    std::vector<TrimMaterial> materials;

    void write(BinaryStream& s) const override {
        s.writeUnsignedVarInt(static_cast<uint32_t>(patterns.size()));
        for (auto& p : patterns) { s.writeString(p.item_name); s.writeString(p.pattern_id); }
        s.writeUnsignedVarInt(static_cast<uint32_t>(materials.size()));
        for (auto& m : materials) { s.writeString(m.material_id); s.writeString(m.color); s.writeString(m.item_name); }
    }
    void read(ReadOnlyBinaryStream& s) override {
        uint32_t pc = s.readUnsignedVarInt(); patterns.resize(pc);
        for (auto& p : patterns) { p.item_name = s.readString(); p.pattern_id = s.readString(); }
        uint32_t mc = s.readUnsignedVarInt(); materials.resize(mc);
        for (auto& m : materials) { m.material_id = s.readString(); m.color = s.readString(); m.item_name = s.readString(); }
    }
};
} // namespace cphertunnel
