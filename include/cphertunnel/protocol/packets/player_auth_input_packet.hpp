// CpherTunnel - PlayerAuthInputPacket (ID: 144)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
#include <bitset>
namespace cphertunnel {

class PlayerAuthInputPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PlayerAuthInputPacket, PlayerAuthInput)

    enum InputData : uint64_t {
        Ascend = 0, Descend = 1, NorthJump_Deprecated = 2, JumpDown = 3,
        SprintDown = 4, ChangeHeight = 5, Jumping = 6, AutoJumpingInWater = 7,
        Sneaking = 8, SneakDown = 9, Up = 10, Down = 11, Left = 12, Right = 13,
        UpLeft = 14, UpRight = 15, WantUp = 16, WantDown = 17,
        WantDownSlow = 18, WantUpSlow = 19, Sprinting = 20,
        AscendBlock = 21, DescendBlock = 22, SneakToggleDown = 23,
        PersistSneak = 24, StartSprinting = 25, StopSprinting = 26,
        StartSneaking = 27, StopSneaking = 28, StartSwimming = 29,
        StopSwimming = 30, StartJumping = 31, StartGliding = 32,
        StopGliding = 33, PerformItemInteraction = 34,
        PerformBlockActions = 35, PerformItemStackRequest = 36,
        HandledTeleport = 37, Emoting = 38,
        MissedSwing = 39, StartCrawling = 40, StopCrawling = 41,
        StartFlying = 42, StopFlying = 43, ClientAckServerData = 44,
        IsInClientPredictedVehicle = 45, PaddleLeft = 46, PaddleRight = 47,
        BlockBreakingDelayEnabled = 48, HorizontalCollision = 49,
        VerticalCollision = 50, DownLeft = 51, DownRight = 52,
        StartUsingItem = 53, IsCameraRelativeMovementEnabled = 54,
        IsRotControlledByMoveDirection = 55, StartSpinAttack = 56,
        StopSpinAttack = 57, IsHotbarOnly = 58, JumpReleasedRaw = 59,
        JumpPressedRaw = 60, JumpCurrentRaw = 61,
        SneakReleasedRaw = 62, SneakPressedRaw = 63, SneakCurrentRaw = 64
    };

    // PlayerBlockActionData
    struct BlockActionData {
        int32_t action_type = 0;
        BlockPos position{0, 0, 0};
        int32_t facing = 0;
        void write(BinaryStream& s) const {
            s.writeVarInt(action_type);
            if (action_type < 5) { // actions with position
                s.writeBlockPos(position);
                s.writeVarInt(facing);
            }
        }
        void read(ReadOnlyBinaryStream& s) {
            action_type = s.readVarInt();
            if (action_type < 5) {
                position = s.readBlockPos();
                facing = s.readVarInt();
            }
        }
    };

    // ItemStackRequestAction (simplified - just action type + raw data)
    struct StackRequestAction {
        uint8_t action_type = 0;
        uint8_t amount = 0;
        ItemStackRequestSlotInfo source;
        ItemStackRequestSlotInfo destination;
        void write(BinaryStream& s) const {
            s.writeByte(action_type);
            s.writeByte(amount);
            source.write(s);
            destination.write(s);
        }
        void read(ReadOnlyBinaryStream& s) {
            action_type = s.readByte();
            amount = s.readByte();
            source.read(s);
            destination.read(s);
        }
    };

    // ItemUse transaction (embedded)
    struct ItemUseTransaction {
        int32_t legacy_request_id = 0;
        struct LegacySlot { uint8_t container_id = 0; std::vector<uint8_t> slots; };
        std::vector<LegacySlot> legacy_slots;
        std::vector<InventoryAction> actions;
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

        void write(BinaryStream& s) const {
            s.writeVarInt(legacy_request_id);
            if (legacy_request_id != 0) {
                s.writeUnsignedVarInt(static_cast<uint32_t>(legacy_slots.size()));
                for (auto& ls : legacy_slots) {
                    s.writeByte(ls.container_id);
                    s.writeUnsignedVarInt(static_cast<uint32_t>(ls.slots.size()));
                    for (auto sl : ls.slots) s.writeByte(sl);
                }
            }
            s.writeUnsignedVarInt(static_cast<uint32_t>(actions.size()));
            for (auto& a : actions) a.write(s);
            s.writeUnsignedVarInt(action_type);
            s.writeUnsignedVarInt(trigger_type);
            s.writeBlockPos(block_pos);
            s.writeVarInt(face);
            s.writeVarInt(hotbar_slot);
            item.write(s);
            s.writeVec3(player_pos);
            s.writeVec3(click_pos);
            s.writeUnsignedVarInt(block_runtime_id);
            s.writeByte(client_interact_prediction);
            s.writeByte(client_cooldown_state);
        }
        void read(ReadOnlyBinaryStream& s) {
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
            uint32_t ac = s.readUnsignedVarInt();
            actions.resize(ac);
            for (auto& a : actions) a.read(s);
            action_type = s.readUnsignedVarInt();
            trigger_type = s.readUnsignedVarInt();
            block_pos = s.readBlockPos();
            face = s.readVarInt();
            hotbar_slot = s.readVarInt();
            item.read(s);
            player_pos = s.readVec3();
            click_pos = s.readVec3();
            block_runtime_id = s.readUnsignedVarInt();
            client_interact_prediction = s.readByte();
            client_cooldown_state = s.readByte();
        }
    };

    Vec2 rotation{0, 0};
    Vec3 position{0, 0, 0};
    Vec2 move_vector{0, 0};
    float head_rotation = 0.0f;
    uint64_t input_data_lo = 0; // bits 0-63
    uint8_t input_data_hi = 0;  // bit 64
    uint32_t input_mode = 0;
    uint32_t play_mode = 0;
    uint32_t interaction_model = 0;
    Vec2 interact_rotation{0, 0};
    PlayerInputTick client_tick;
    Vec3 pos_delta{0, 0, 0};

    // Conditional fields
    ItemUseTransaction item_use_transaction;          // if PerformItemInteraction
    int32_t stack_request_id = 0;                     // if PerformItemStackRequest
    std::vector<StackRequestAction> stack_actions;
    std::vector<std::string> strings_to_filter;
    int32_t strings_to_filter_origin = 0;
    std::vector<BlockActionData> block_actions;       // if PerformBlockActions
    Vec2 vehicle_rotation{0, 0};                      // if IsInClientPredictedVehicle
    ActorUniqueID vehicle_id;
    Vec2 analog_move_vector{0, 0};
    Vec3 camera_orientation{0, 0, 0};
    Vec2 raw_move_vector{0, 0};

    bool hasInputFlag(InputData flag) const {
        if (flag < 64) return (input_data_lo >> flag) & 1;
        return (input_data_hi >> (flag - 64)) & 1;
    }
    void setInputFlag(InputData flag, bool val) {
        if (flag < 64) {
            if (val) input_data_lo |= (1ULL << flag);
            else input_data_lo &= ~(1ULL << flag);
        } else {
            if (val) input_data_hi |= (1 << (flag - 64));
            else input_data_hi &= ~(1 << (flag - 64));
        }
    }

    void write(BinaryStream& s) const override {
        s.writeVec2(rotation);
        s.writeVec3(position);
        s.writeVec2(move_vector);
        s.writeFloat(head_rotation);
        // Write 65-bit bitset as UnsignedVarInt64 + 1 bit
        s.writeUnsignedVarInt64(input_data_lo);
        s.writeByte(input_data_hi);
        s.writeUnsignedVarInt(input_mode);
        s.writeUnsignedVarInt(play_mode);
        s.writeUnsignedVarInt(interaction_model);
        s.writeVec2(interact_rotation);
        client_tick.write(s);
        s.writeVec3(pos_delta);
        // Conditional: PerformItemInteraction
        if (hasInputFlag(PerformItemInteraction)) {
            item_use_transaction.write(s);
        }
        // Conditional: PerformItemStackRequest
        if (hasInputFlag(PerformItemStackRequest)) {
            s.writeVarInt(stack_request_id);
            s.writeUnsignedVarInt(static_cast<uint32_t>(stack_actions.size()));
            for (auto& a : stack_actions) a.write(s);
            s.writeUnsignedVarInt(static_cast<uint32_t>(strings_to_filter.size()));
            for (auto& str : strings_to_filter) s.writeString(str);
            s.writeSignedInt(strings_to_filter_origin);
        }
        // Conditional: PerformBlockActions
        if (hasInputFlag(PerformBlockActions)) {
            s.writeVarInt(static_cast<int32_t>(block_actions.size()));
            for (auto& a : block_actions) a.write(s);
        }
        // Conditional: IsInClientPredictedVehicle
        if (hasInputFlag(IsInClientPredictedVehicle)) {
            s.writeVec2(vehicle_rotation);
            s.writeActorUniqueID(vehicle_id);
        }
        s.writeVec2(analog_move_vector);
        s.writeVec3(camera_orientation);
        s.writeVec2(raw_move_vector);
    }
    void read(ReadOnlyBinaryStream& s) override {
        rotation = s.readVec2();
        position = s.readVec3();
        move_vector = s.readVec2();
        head_rotation = s.readFloat();
        input_data_lo = s.readUnsignedVarInt64();
        input_data_hi = s.readByte();
        input_mode = s.readUnsignedVarInt();
        play_mode = s.readUnsignedVarInt();
        interaction_model = s.readUnsignedVarInt();
        interact_rotation = s.readVec2();
        client_tick.read(s);
        pos_delta = s.readVec3();
        if (hasInputFlag(PerformItemInteraction)) {
            item_use_transaction.read(s);
        }
        if (hasInputFlag(PerformItemStackRequest)) {
            stack_request_id = s.readVarInt();
            uint32_t ac = s.readUnsignedVarInt();
            stack_actions.resize(ac);
            for (auto& a : stack_actions) a.read(s);
            uint32_t sc = s.readUnsignedVarInt();
            strings_to_filter.resize(sc);
            for (auto& str : strings_to_filter) str = s.readString();
            strings_to_filter_origin = s.readSignedInt();
        }
        if (hasInputFlag(PerformBlockActions)) {
            int32_t bc = s.readVarInt();
            block_actions.resize(static_cast<size_t>(bc));
            for (auto& a : block_actions) a.read(s);
        }
        if (hasInputFlag(IsInClientPredictedVehicle)) {
            vehicle_rotation = s.readVec2();
            vehicle_id = s.readActorUniqueID();
        }
        analog_move_vector = s.readVec2();
        camera_orientation = s.readVec3();
        raw_move_vector = s.readVec2();
    }
};

} // namespace cphertunnel
