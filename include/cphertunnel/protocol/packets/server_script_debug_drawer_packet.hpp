// CpherTunnel - ServerScriptDebugDrawerPacket (ID: 328)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class ServerScriptDebugDrawerPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ServerScriptDebugDrawerPacket, ServerScriptDebugDrawer)
    // Debug draw commands - opaque data blob
    std::string draw_data;
    void write(BinaryStream& s) const override { s.writeString(draw_data); }
    void read(ReadOnlyBinaryStream& s) override { draw_data = s.readString(); }
};
} // namespace cphertunnel
