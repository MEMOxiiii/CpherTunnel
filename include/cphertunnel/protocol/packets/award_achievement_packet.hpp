// CpherTunnel - AwardAchievementPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class AwardAchievementPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(AwardAchievementPacket, AwardAchievement)

    int32_t achievement_id = 0;

    void write(BinaryStream& stream) const override {
        stream.writeSignedInt(achievement_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        achievement_id = stream.readSignedInt();
    }
};

} // namespace cphertunnel
