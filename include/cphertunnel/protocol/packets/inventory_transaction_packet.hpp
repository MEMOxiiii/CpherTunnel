// CpherTunnel - InventoryTransactionPacket (ID: 30)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {

class InventoryTransactionPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(InventoryTransactionPacket, InventoryTransaction)

    enum class TransactionType : uint32_t {
        Normal = 0,
        Mismatch = 1,
        ItemUse = 2,
        ItemUseOnEntity = 3,
        ItemRelease = 4
    };

    // Legacy fields
    int32_t legacy_request_id = 0;
    struct LegacySetItemSlot {
        uint8_t container_id = 0;
        std::vector<uint8_t> slots;
    };
    std::vector<LegacySetItemSlot> legacy_slots;

    TransactionType transaction_type = TransactionType::Normal;

    // Actions (for Normal/Mismatch)
    std::vector<InventoryAction> actions;

    // ItemUse data
    struct ItemUseData {
        uint32_t action_type = 0;
        uint32_t trigger_type = 0;
        BlockPos block_pos{0, 0, 0};
        int32_t face = 0;
        int32_t hotbar_slot = 0;
        NetworkItemStackDescriptor item;
        Vec3 player_pos{0, 0, 0};
        Vec3 click_pos{0, 0, 0};
        uint32_t block_runtime_id = 0;
        uint8_t client_interact_prediction = 0;
        uint8_t client_cooldown_state = 0;
    };
    ItemUseData item_use_data;

    // ItemUseOnEntity data
    struct ItemUseOnEntityData {
        ActorRuntimeID entity_runtime_id;
        uint32_t action_type = 0;
        int32_t hotbar_slot = 0;
        NetworkItemStackDescriptor item;
        Vec3 player_pos{0, 0, 0};
        Vec3 click_pos{0, 0, 0};
    };
    ItemUseOnEntityData item_use_on_entity_data;

    // ItemRelease data
    struct ItemReleaseData {
        uint32_t action_type = 0;
        int32_t hotbar_slot = 0;
        NetworkItemStackDescriptor item;
        Vec3 head_pos{0, 0, 0};
    };
    ItemReleaseData item_release_data;

    void write(BinaryStream& s) const override {
        s.writeVarInt(legacy_request_id);
        if (legacy_request_id != 0) {
            s.writeUnsignedVarInt(static_cast<uint32_t>(legacy_slots.size()));
            for (auto& ls : legacy_slots) {
                s.writeByte(ls.container_id);
                s.writeUnsignedVarInt(static_cast<uint32_t>(ls.slots.size()));
                for (auto sl : ls.slots) s.writeByte(sl);
            }
        }
        s.writeUnsignedVarInt(static_cast<uint32_t>(transaction_type));
        // Actions
        s.writeUnsignedVarInt(static_cast<uint32_t>(actions.size()));
        for (auto& a : actions) a.write(s);
        // Type-specific data
        switch (transaction_type) {
        case TransactionType::ItemUse:
            s.writeUnsignedVarInt(item_use_data.action_type);
            s.writeUnsignedVarInt(item_use_data.trigger_type);
            s.writeBlockPos(item_use_data.block_pos);
            s.writeVarInt(item_use_data.face);
            s.writeVarInt(item_use_data.hotbar_slot);
            item_use_data.item.write(s);
            s.writeVec3(item_use_data.player_pos);
            s.writeVec3(item_use_data.click_pos);
            s.writeUnsignedVarInt(item_use_data.block_runtime_id);
            s.writeByte(item_use_data.client_interact_prediction);
            s.writeByte(item_use_data.client_cooldown_state);
            break;
        case TransactionType::ItemUseOnEntity:
            s.writeActorRuntimeID(item_use_on_entity_data.entity_runtime_id);
            s.writeUnsignedVarInt(item_use_on_entity_data.action_type);
            s.writeVarInt(item_use_on_entity_data.hotbar_slot);
            item_use_on_entity_data.item.write(s);
            s.writeVec3(item_use_on_entity_data.player_pos);
            s.writeVec3(item_use_on_entity_data.click_pos);
            break;
        case TransactionType::ItemRelease:
            s.writeUnsignedVarInt(item_release_data.action_type);
            s.writeVarInt(item_release_data.hotbar_slot);
            item_release_data.item.write(s);
            s.writeVec3(item_release_data.head_pos);
            break;
        default: break;
        }
    }
    void read(ReadOnlyBinaryStream& s) override {
        legacy_request_id = s.readVarInt();
        if (legacy_request_id != 0) {
            uint32_t lc = s.readUnsignedVarInt();
            legacy_slots.resize(lc);
            for (auto& ls : legacy_slots) {
                ls.container_id = s.readByte();
                uint32_t sc = s.readUnsignedVarInt();
                ls.slots.resize(sc);
                for (auto& sl : ls.slots) sl = s.readByte();
            }
        }
        transaction_type = static_cast<TransactionType>(s.readUnsignedVarInt());
        uint32_t ac = s.readUnsignedVarInt();
        actions.resize(ac);
        for (auto& a : actions) a.read(s);
        switch (transaction_type) {
        case TransactionType::ItemUse:
            item_use_data.action_type = s.readUnsignedVarInt();
            item_use_data.trigger_type = s.readUnsignedVarInt();
            item_use_data.block_pos = s.readBlockPos();
            item_use_data.face = s.readVarInt();
            item_use_data.hotbar_slot = s.readVarInt();
            item_use_data.item.read(s);
            item_use_data.player_pos = s.readVec3();
            item_use_data.click_pos = s.readVec3();
            item_use_data.block_runtime_id = s.readUnsignedVarInt();
            item_use_data.client_interact_prediction = s.readByte();
            item_use_data.client_cooldown_state = s.readByte();
            break;
        case TransactionType::ItemUseOnEntity:
            item_use_on_entity_data.entity_runtime_id = s.readActorRuntimeID();
            item_use_on_entity_data.action_type = s.readUnsignedVarInt();
            item_use_on_entity_data.hotbar_slot = s.readVarInt();
            item_use_on_entity_data.item.read(s);
            item_use_on_entity_data.player_pos = s.readVec3();
            item_use_on_entity_data.click_pos = s.readVec3();
            break;
        case TransactionType::ItemRelease:
            item_release_data.action_type = s.readUnsignedVarInt();
            item_release_data.hotbar_slot = s.readVarInt();
            item_release_data.item.read(s);
            item_release_data.head_pos = s.readVec3();
            break;
        default: break;
        }
    }
};

} // namespace cphertunnel
