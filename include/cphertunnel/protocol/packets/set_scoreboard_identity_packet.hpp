// CpherTunnel - SetScoreboardIdentityPacket (ID: 112)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {
class SetScoreboardIdentityPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SetScoreboardIdentityPacket, SetScoreboardIdentity)

    enum class Type : uint8_t { Update = 0, Clear = 1 };

    struct Entry {
        ScoreboardId scoreboard_id;
        int64_t player_unique_id = 0;  // only for Update
    };

    Type type = Type::Update;
    std::vector<Entry> entries;

    void write(BinaryStream& s) const override {
        s.writeByte(static_cast<uint8_t>(type));
        s.writeUnsignedVarInt(static_cast<uint32_t>(entries.size()));
        for (auto& e : entries) {
            e.scoreboard_id.write(s);
            if (type == Type::Update) s.writeVarInt64(e.player_unique_id);
        }
    }
    void read(ReadOnlyBinaryStream& s) override {
        type = static_cast<Type>(s.readByte());
        uint32_t count = s.readUnsignedVarInt();
        entries.resize(count);
        for (auto& e : entries) {
            e.scoreboard_id.read(s);
            if (type == Type::Update) e.player_unique_id = s.readVarInt64();
        }
    }
};
} // namespace cphertunnel
