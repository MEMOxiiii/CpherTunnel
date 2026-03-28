// CpherTunnel - ScriptCustomEventPacket (ID: 117)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class ScriptCustomEventPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ScriptCustomEventPacket, ScriptCustomEvent)
    std::string event_name;
    std::string event_data;
    void write(BinaryStream& s) const override {
        s.writeString(event_name);
        s.writeString(event_data);
    }
    void read(ReadOnlyBinaryStream& s) override {
        event_name = s.readString();
        event_data = s.readString();
    }
};
} // namespace cphertunnel
