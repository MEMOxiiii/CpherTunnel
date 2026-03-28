// CpherTunnel - SetScorePacket (ID: 108)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {
class SetScorePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SetScorePacket, SetScore)

    enum class Type : uint8_t { Change = 0, Remove = 1 };

    struct ScoreInfo {
        ScoreboardId scoreboard_id;
        std::string objective_name;
        int32_t score_value = 0;
        // Only for Change type:
        enum class IdentityType : uint8_t { Player = 1, Entity = 2, FakePlayer = 3 };
        IdentityType identity_type = IdentityType::FakePlayer;
        int64_t player_unique_id = 0;    // Player
        ActorUniqueID actor_id;           // Entity
        std::string fake_player_name;     // FakePlayer
    };

    Type type = Type::Change;
    std::vector<ScoreInfo> entries;

    void write(BinaryStream& s) const override {
        s.writeByte(static_cast<uint8_t>(type));
        s.writeUnsignedVarInt(static_cast<uint32_t>(entries.size()));
        for (auto& e : entries) {
            e.scoreboard_id.write(s);
            s.writeString(e.objective_name);
            s.writeSignedInt(e.score_value);
            if (type == Type::Change) {
                s.writeByte(static_cast<uint8_t>(e.identity_type));
                switch (e.identity_type) {
                    case ScoreInfo::IdentityType::Player:
                        s.writeVarInt64(e.player_unique_id); break;
                    case ScoreInfo::IdentityType::Entity:
                        s.writeActorUniqueID(e.actor_id); break;
                    case ScoreInfo::IdentityType::FakePlayer:
                        s.writeString(e.fake_player_name); break;
                }
            }
        }
    }
    void read(ReadOnlyBinaryStream& s) override {
        type = static_cast<Type>(s.readByte());
        uint32_t count = s.readUnsignedVarInt();
        entries.resize(count);
        for (auto& e : entries) {
            e.scoreboard_id.read(s);
            e.objective_name = s.readString();
            e.score_value = s.readSignedInt();
            if (type == Type::Change) {
                e.identity_type = static_cast<ScoreInfo::IdentityType>(s.readByte());
                switch (e.identity_type) {
                    case ScoreInfo::IdentityType::Player:
                        e.player_unique_id = s.readVarInt64(); break;
                    case ScoreInfo::IdentityType::Entity:
                        e.actor_id = s.readActorUniqueID(); break;
                    case ScoreInfo::IdentityType::FakePlayer:
                        e.fake_player_name = s.readString(); break;
                }
            }
        }
    }
};
} // namespace cphertunnel
