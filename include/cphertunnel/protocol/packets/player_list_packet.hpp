// CpherTunnel - PlayerListPacket (ID: 63)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class PlayerListPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PlayerListPacket, PlayerList)

    enum class Action : uint8_t { Add = 0, Remove = 1 };

    struct AddEntry {
        UUID uuid;
        ActorUniqueID actor_id;
        std::string player_name;
        std::string xbl_xuid;
        std::string platform_chat_id;
        int32_t build_platform = 0;
        std::string serialized_skin; // raw skin data
        bool is_teacher = false;
        bool is_host = false;
        bool is_sub_client = false;
    };

    Action action = Action::Add;
    std::vector<AddEntry> add_entries;
    std::vector<UUID> remove_entries;
    // For Add action, trusted skins list (one per entry)
    std::vector<bool> trusted_skins;

    void write(BinaryStream& s) const override {
        s.writeByte(static_cast<uint8_t>(action));
        if (action == Action::Add) {
            s.writeUnsignedVarInt(static_cast<uint32_t>(add_entries.size()));
            for (auto& e : add_entries) {
                s.writeUUID(e.uuid);
                s.writeActorUniqueID(e.actor_id);
                s.writeString(e.player_name);
                s.writeString(e.xbl_xuid);
                s.writeString(e.platform_chat_id);
                s.writeSignedInt(e.build_platform);
                s.writeString(e.serialized_skin);
                s.writeBool(e.is_teacher);
                s.writeBool(e.is_host);
                s.writeBool(e.is_sub_client);
            }
            for (size_t i = 0; i < add_entries.size(); ++i) {
                s.writeBool(i < trusted_skins.size() ? trusted_skins[i] : true);
            }
        } else {
            s.writeUnsignedVarInt(static_cast<uint32_t>(remove_entries.size()));
            for (auto& uuid : remove_entries) s.writeUUID(uuid);
        }
    }
    void read(ReadOnlyBinaryStream& s) override {
        action = static_cast<Action>(s.readByte());
        if (action == Action::Add) {
            uint32_t count = s.readUnsignedVarInt();
            add_entries.resize(count);
            for (auto& e : add_entries) {
                e.uuid = s.readUUID();
                e.actor_id = s.readActorUniqueID();
                e.player_name = s.readString();
                e.xbl_xuid = s.readString();
                e.platform_chat_id = s.readString();
                e.build_platform = s.readSignedInt();
                e.serialized_skin = s.readString();
                e.is_teacher = s.readBool();
                e.is_host = s.readBool();
                e.is_sub_client = s.readBool();
            }
            trusted_skins.resize(count);
            for (auto& trusted : trusted_skins) trusted = s.readBool();
        } else {
            uint32_t count = s.readUnsignedVarInt();
            remove_entries.resize(count);
            for (auto& uuid : remove_entries) uuid = s.readUUID();
        }
    }
};
} // namespace cphertunnel
