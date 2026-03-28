// CpherTunnel - PlayerEnchantOptionsPacket (ID: 146)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class PlayerEnchantOptionsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PlayerEnchantOptionsPacket, PlayerEnchantOptions)

    struct EnchantOption {
        uint32_t cost = 0;
        // ItemEnchants - simplified as raw bytes
        std::string enchants_raw;
        std::string enchant_name;
        uint32_t enchant_net_id = 0;
    };
    std::vector<EnchantOption> options;

    void write(BinaryStream& s) const override {
        s.writeUnsignedVarInt(static_cast<uint32_t>(options.size()));
        for (auto& o : options) {
            s.writeUnsignedVarInt(o.cost);
            s.writeString(o.enchants_raw);
            s.writeString(o.enchant_name);
            s.writeUnsignedVarInt(o.enchant_net_id);
        }
    }
    void read(ReadOnlyBinaryStream& s) override {
        uint32_t count = s.readUnsignedVarInt();
        options.resize(count);
        for (auto& o : options) {
            o.cost = s.readUnsignedVarInt();
            o.enchants_raw = s.readString();
            o.enchant_name = s.readString();
            o.enchant_net_id = s.readUnsignedVarInt();
        }
    }
};
} // namespace cphertunnel
