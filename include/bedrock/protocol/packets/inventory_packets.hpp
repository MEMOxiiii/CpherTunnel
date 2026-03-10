// =============================================================================
// BedrockProtocol - Inventory Packets
// Container, inventory, and item-related packets
// =============================================================================
#pragma once

#include "../packet.hpp"

namespace bedrock {

// =============================================================================
// ContainerOpenPacket (ID: 46)
// =============================================================================
class ContainerOpenPacket : public Packet {
public:
    BEDROCK_PACKET(ContainerOpenPacket, ContainerOpen)

    uint8_t window_id = 0;
    uint8_t type = 0;
    NetworkBlockPos position;
    ActorUniqueID entity_unique_id;

    void write(BinaryStream& stream) const override {
        stream.writeByte(window_id);
        stream.writeByte(type);
        stream.writeNetworkBlockPos(position);
        stream.writeActorUniqueID(entity_unique_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        window_id = stream.readByte();
        type = stream.readByte();
        position = stream.readNetworkBlockPos();
        entity_unique_id = stream.readActorUniqueID();
    }
};

// =============================================================================
// ContainerClosePacket (ID: 47)
// =============================================================================
class ContainerClosePacket : public Packet {
public:
    BEDROCK_PACKET(ContainerClosePacket, ContainerClose)

    uint8_t window_id = 0;
    uint8_t container_type = 0;
    bool server_initiated = false;

    void write(BinaryStream& stream) const override {
        stream.writeByte(window_id);
        stream.writeByte(container_type);
        stream.writeBool(server_initiated);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        window_id = stream.readByte();
        container_type = stream.readByte();
        server_initiated = stream.readBool();
    }
};

// =============================================================================
// ContainerSetDataPacket (ID: 51)
// =============================================================================
class ContainerSetDataPacket : public Packet {
public:
    BEDROCK_PACKET(ContainerSetDataPacket, ContainerSetData)

    uint8_t window_id = 0;
    int32_t property = 0;
    int32_t value = 0;

    void write(BinaryStream& stream) const override {
        stream.writeByte(window_id);
        stream.writeVarInt(property);
        stream.writeVarInt(value);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        window_id = stream.readByte();
        property = stream.readVarInt();
        value = stream.readVarInt();
    }
};

// =============================================================================
// PlayerHotbarPacket (ID: 48)
// =============================================================================
class PlayerHotbarPacket : public Packet {
public:
    BEDROCK_PACKET(PlayerHotbarPacket, PlayerHotbar)

    uint32_t selected_hotbar_slot = 0;
    uint8_t window_id = 0;
    bool select_hotbar_slot = true;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(selected_hotbar_slot);
        stream.writeByte(window_id);
        stream.writeBool(select_hotbar_slot);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        selected_hotbar_slot = stream.readUnsignedVarInt();
        window_id = stream.readByte();
        select_hotbar_slot = stream.readBool();
    }
};

// =============================================================================
// GuiDataPickItemPacket (ID: 54)
// =============================================================================
class GuiDataPickItemPacket : public Packet {
public:
    BEDROCK_PACKET(GuiDataPickItemPacket, GuiDataPickItem)

    std::string item_description;
    std::string item_effects;
    int32_t hotbar_slot = 0;

    void write(BinaryStream& stream) const override {
        stream.writeString(item_description);
        stream.writeString(item_effects);
        stream.writeSignedInt(hotbar_slot);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        item_description = stream.readString();
        item_effects = stream.readString();
        hotbar_slot = stream.readSignedInt();
    }
};

// =============================================================================
// ContainerRegistryCleanupPacket (ID: 317)
// =============================================================================
class ContainerRegistryCleanupPacket : public Packet {
public:
    BEDROCK_PACKET(ContainerRegistryCleanupPacket, ContainerRegistryCleanup)

    std::vector<uint8_t> removed_containers;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(static_cast<uint32_t>(removed_containers.size()));
        for (auto c : removed_containers) {
            stream.writeByte(c);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        uint32_t count = stream.readUnsignedVarInt();
        removed_containers.resize(count);
        for (auto& c : removed_containers) {
            c = stream.readByte();
        }
    }
};

// =============================================================================
// PlayerToggleCrafterSlotRequestPacket (ID: 306)
// =============================================================================
class PlayerToggleCrafterSlotRequestPacket : public Packet {
public:
    BEDROCK_PACKET(PlayerToggleCrafterSlotRequestPacket, PlayerToggleCrafterSlotRequest)

    int32_t x = 0;
    int32_t y = 0;
    int32_t z = 0;
    uint8_t slot = 0;
    bool disabled = false;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(x);
        stream.writeVarInt(y);
        stream.writeVarInt(z);
        stream.writeByte(slot);
        stream.writeBool(disabled);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        x = stream.readVarInt();
        y = stream.readVarInt();
        z = stream.readVarInt();
        slot = stream.readByte();
        disabled = stream.readBool();
    }
};

// =============================================================================
// SetPlayerInventoryOptionsPacket (ID: 307)
// =============================================================================
class SetPlayerInventoryOptionsPacket : public Packet {
public:
    BEDROCK_PACKET(SetPlayerInventoryOptionsPacket, SetPlayerInventoryOptions)

    int32_t left_tab = 0;
    int32_t right_tab = 0;
    bool filtering = false;
    int32_t layout = 0;
    int32_t crafting_layout = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(left_tab);
        stream.writeVarInt(right_tab);
        stream.writeBool(filtering);
        stream.writeVarInt(layout);
        stream.writeVarInt(crafting_layout);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        left_tab = stream.readVarInt();
        right_tab = stream.readVarInt();
        filtering = stream.readBool();
        layout = stream.readVarInt();
        crafting_layout = stream.readVarInt();
    }
};

// =============================================================================
// CompletedUsingItemPacket (ID: 142)
// =============================================================================
class CompletedUsingItemPacket : public Packet {
public:
    BEDROCK_PACKET(CompletedUsingItemPacket, CompletedUsingItem)

    int16_t item_id = 0;
    int32_t use_method = 0;

    void write(BinaryStream& stream) const override {
        stream.writeSignedShort(item_id);
        stream.writeSignedInt(use_method);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        item_id = stream.readSignedShort();
        use_method = stream.readSignedInt();
    }
};

// =============================================================================
// AnvilDamagePacket (ID: 141)
// =============================================================================
class AnvilDamagePacket : public Packet {
public:
    BEDROCK_PACKET(AnvilDamagePacket, AnvilDamage)

    uint8_t damage = 0;
    NetworkBlockPos position;

    void write(BinaryStream& stream) const override {
        stream.writeByte(damage);
        stream.writeNetworkBlockPos(position);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        damage = stream.readByte();
        position = stream.readNetworkBlockPos();
    }
};

// =============================================================================
// PlayerStartItemCooldownPacket (ID: 176)
// =============================================================================
class PlayerStartItemCooldownPacket : public Packet {
public:
    BEDROCK_PACKET(PlayerStartItemCooldownPacket, PlayerStartItemCooldown)

    std::string item_category;
    int32_t cooldown_ticks = 0;

    void write(BinaryStream& stream) const override {
        stream.writeString(item_category);
        stream.writeVarInt(cooldown_ticks);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        item_category = stream.readString();
        cooldown_ticks = stream.readVarInt();
    }
};

// =============================================================================
// UpdateEquipPacket (ID: 81)
// =============================================================================
class UpdateEquipPacket : public Packet {
public:
    BEDROCK_PACKET(UpdateEquipPacket, UpdateEquip)

    uint8_t container_id = 0;
    uint8_t type = 0;
    int32_t size = 0;
    ActorUniqueID entity_unique_id;
    ByteBuffer nbt_data; // CompoundTag

    void write(BinaryStream& stream) const override {
        stream.writeByte(container_id);
        stream.writeByte(type);
        stream.writeVarInt(size);
        stream.writeActorUniqueID(entity_unique_id);
        stream.writeRawBytes(nbt_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        container_id = stream.readByte();
        type = stream.readByte();
        size = stream.readVarInt();
        entity_unique_id = stream.readActorUniqueID();
        nbt_data = stream.readRemainingBytes();
    }
};

// =============================================================================
// UpdateTradePacket (ID: 80)
// =============================================================================
class UpdateTradePacket : public Packet {
public:
    BEDROCK_PACKET(UpdateTradePacket, UpdateTrade)

    uint8_t container_id = 0;
    uint8_t type = 0;
    int32_t size = 0;
    int32_t trader_tier = 0;
    ActorUniqueID entity_unique_id;
    ActorUniqueID last_trading_player;
    std::string display_name;
    bool use_new_trade_screen = false;
    bool using_economy_trade = false;
    ByteBuffer nbt_data; // CompoundTag

    void write(BinaryStream& stream) const override {
        stream.writeByte(container_id);
        stream.writeByte(type);
        stream.writeVarInt(size);
        stream.writeVarInt(trader_tier);
        stream.writeActorUniqueID(entity_unique_id);
        stream.writeActorUniqueID(last_trading_player);
        stream.writeString(display_name);
        stream.writeBool(use_new_trade_screen);
        stream.writeBool(using_economy_trade);
        stream.writeRawBytes(nbt_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        container_id = stream.readByte();
        type = stream.readByte();
        size = stream.readVarInt();
        trader_tier = stream.readVarInt();
        entity_unique_id = stream.readActorUniqueID();
        last_trading_player = stream.readActorUniqueID();
        display_name = stream.readString();
        use_new_trade_screen = stream.readBool();
        using_economy_trade = stream.readBool();
        nbt_data = stream.readRemainingBytes();
    }
};

} // namespace bedrock
