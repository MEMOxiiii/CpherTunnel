// CpherTunnel - UpdateTradePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class UpdateTradePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(UpdateTradePacket, UpdateTrade)

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

} // namespace cphertunnel
