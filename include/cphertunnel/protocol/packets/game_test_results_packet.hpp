// CpherTunnel - GameTestResultsPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class GameTestResultsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(GameTestResultsPacket, GameTestResults)

    bool succeeded = false;
    std::string error;
    std::string test_name;

    void write(BinaryStream& stream) const override {
        stream.writeBool(succeeded);
        stream.writeString(error);
        stream.writeString(test_name);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        succeeded = stream.readBool();
        error = stream.readString();
        test_name = stream.readString();
    }
};

} // namespace cphertunnel
