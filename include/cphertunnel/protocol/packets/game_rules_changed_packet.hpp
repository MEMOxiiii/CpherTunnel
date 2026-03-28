// CpherTunnel - GameRulesChangedPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {



class GameRulesChangedPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(GameRulesChangedPacket, GameRulesChanged)

    std::vector<GameRule> rules;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(static_cast<uint32_t>(rules.size()));
        for (auto& rule : rules) {
            stream.writeString(rule.name);
            stream.writeBool(rule.editable);
            stream.writeUnsignedVarInt(rule.type);
            switch (rule.type) {
                case 1: stream.writeBool(rule.bool_value); break;
                case 2: stream.writeVarInt(rule.int_value); break;
                case 3: stream.writeFloat(rule.float_value); break;
                default: break;
            }
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        uint32_t count = stream.readUnsignedVarInt();
        rules.resize(count);
        for (auto& rule : rules) {
            rule.name = stream.readString();
            rule.editable = stream.readBool();
            rule.type = stream.readUnsignedVarInt();
            switch (rule.type) {
                case 1: rule.bool_value = stream.readBool(); break;
                case 2: rule.int_value = stream.readVarInt(); break;
                case 3: rule.float_value = stream.readFloat(); break;
                default: break;
            }
        }
    }
};

} // namespace cphertunnel
