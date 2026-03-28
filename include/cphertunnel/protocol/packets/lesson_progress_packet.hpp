// CpherTunnel - LessonProgressPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class LessonProgressPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(LessonProgressPacket, LessonProgress)

    int32_t action = 0;
    int32_t score = 0;
    std::string activity_id;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(action);
        stream.writeVarInt(score);
        stream.writeString(activity_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action = stream.readVarInt();
        score = stream.readVarInt();
        activity_id = stream.readString();
    }
};

} // namespace cphertunnel
