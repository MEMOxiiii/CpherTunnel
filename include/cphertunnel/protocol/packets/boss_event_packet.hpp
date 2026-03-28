// CpherTunnel - BossEventPacket (ID: 74)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class BossEventPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(BossEventPacket, BossEvent)

    enum class EventType : uint32_t {
        Add = 0, PlayerAdded = 1, Remove = 2, PlayerRemoved = 3,
        UpdatePercent = 4, UpdateName = 5, UpdateProperties = 6,
        UpdateStyle = 7, Query = 8
    };

    ActorUniqueID target_actor_id;
    EventType event_type = EventType::Add;
    // Add
    std::string name;
    std::string filtered_name;
    float health_percent = 1.0f;
    uint16_t darken_screen = 0;
    uint32_t color = 0;
    uint32_t overlay = 0;
    // PlayerAdded/PlayerRemoved/Query
    ActorUniqueID player_id;

    void write(BinaryStream& s) const override {
        s.writeActorUniqueID(target_actor_id);
        s.writeUnsignedVarInt(static_cast<uint32_t>(event_type));
        switch (event_type) {
            case EventType::Add:
                s.writeString(name); s.writeString(filtered_name);
                s.writeFloat(health_percent); s.writeUnsignedShort(darken_screen);
                s.writeUnsignedVarInt(color); s.writeUnsignedVarInt(overlay); break;
            case EventType::PlayerAdded:
            case EventType::PlayerRemoved:
            case EventType::Query:
                s.writeActorUniqueID(player_id); break;
            case EventType::Remove: break;
            case EventType::UpdatePercent:
                s.writeFloat(health_percent); break;
            case EventType::UpdateName:
                s.writeString(name); s.writeString(filtered_name); break;
            case EventType::UpdateProperties:
                s.writeUnsignedShort(darken_screen);
                s.writeUnsignedVarInt(color); s.writeUnsignedVarInt(overlay); break;
            case EventType::UpdateStyle:
                s.writeUnsignedVarInt(color); s.writeUnsignedVarInt(overlay); break;
        }
    }
    void read(ReadOnlyBinaryStream& s) override {
        target_actor_id = s.readActorUniqueID();
        event_type = static_cast<EventType>(s.readUnsignedVarInt());
        switch (event_type) {
            case EventType::Add:
                name = s.readString(); filtered_name = s.readString();
                health_percent = s.readFloat(); darken_screen = s.readUnsignedShort();
                color = s.readUnsignedVarInt(); overlay = s.readUnsignedVarInt(); break;
            case EventType::PlayerAdded:
            case EventType::PlayerRemoved:
            case EventType::Query:
                player_id = s.readActorUniqueID(); break;
            case EventType::Remove: break;
            case EventType::UpdatePercent:
                health_percent = s.readFloat(); break;
            case EventType::UpdateName:
                name = s.readString(); filtered_name = s.readString(); break;
            case EventType::UpdateProperties:
                darken_screen = s.readUnsignedShort();
                color = s.readUnsignedVarInt(); overlay = s.readUnsignedVarInt(); break;
            case EventType::UpdateStyle:
                color = s.readUnsignedVarInt(); overlay = s.readUnsignedVarInt(); break;
        }
    }
};
} // namespace cphertunnel
