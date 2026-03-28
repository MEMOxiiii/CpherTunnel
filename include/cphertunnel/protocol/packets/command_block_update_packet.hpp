// CpherTunnel - CommandBlockUpdatePacket (ID: 78)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class CommandBlockUpdatePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CommandBlockUpdatePacket, CommandBlockUpdate)
    bool is_block = true;
    // If !is_block
    ActorRuntimeID target_runtime_id;
    // If is_block
    BlockPos block_position{0, 0, 0};
    uint32_t command_block_mode = 0;
    bool redstone_mode = false;
    bool is_conditional = false;
    // Common
    std::string command;
    std::string last_output;
    std::string name;
    std::string filtered_name;
    bool track_output = false;
    uint32_t tick_delay = 0;
    bool execute_on_first_tick = false;

    void write(BinaryStream& s) const override {
        s.writeBool(is_block);
        if (is_block) {
            s.writeBlockPos(block_position);
            s.writeUnsignedVarInt(command_block_mode);
            s.writeBool(redstone_mode);
            s.writeBool(is_conditional);
        } else {
            s.writeActorRuntimeID(target_runtime_id);
        }
        s.writeString(command);
        s.writeString(last_output);
        s.writeString(name);
        s.writeString(filtered_name);
        s.writeBool(track_output);
        s.writeUnsignedInt(tick_delay);
        s.writeBool(execute_on_first_tick);
    }
    void read(ReadOnlyBinaryStream& s) override {
        is_block = s.readBool();
        if (is_block) {
            block_position = s.readBlockPos();
            command_block_mode = s.readUnsignedVarInt();
            redstone_mode = s.readBool();
            is_conditional = s.readBool();
        } else {
            target_runtime_id = s.readActorRuntimeID();
        }
        command = s.readString();
        last_output = s.readString();
        name = s.readString();
        filtered_name = s.readString();
        track_output = s.readBool();
        tick_delay = s.readUnsignedInt();
        execute_on_first_tick = s.readBool();
    }
};
} // namespace cphertunnel
