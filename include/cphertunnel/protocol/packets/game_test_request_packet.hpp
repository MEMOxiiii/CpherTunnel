// CpherTunnel - GameTestRequestPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class GameTestRequestPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(GameTestRequestPacket, GameTestRequest)

    int32_t max_tests_per_batch = 0;
    int32_t repeat_count = 0;
    uint8_t rotation = 0;
    bool stop_on_failure = false;
    BlockPos test_pos;
    int32_t tests_per_row = 0;
    std::string test_name;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(max_tests_per_batch);
        stream.writeVarInt(repeat_count);
        stream.writeByte(rotation);
        stream.writeBool(stop_on_failure);
        stream.writeBlockPos(test_pos);
        stream.writeVarInt(tests_per_row);
        stream.writeString(test_name);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        max_tests_per_batch = stream.readVarInt();
        repeat_count = stream.readVarInt();
        rotation = stream.readByte();
        stop_on_failure = stream.readBool();
        test_pos = stream.readBlockPos();
        tests_per_row = stream.readVarInt();
        test_name = stream.readString();
    }
};

} // namespace cphertunnel
