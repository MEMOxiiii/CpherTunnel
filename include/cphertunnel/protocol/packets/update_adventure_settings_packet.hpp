// CpherTunnel - UpdateAdventureSettingsPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class UpdateAdventureSettingsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(UpdateAdventureSettingsPacket, UpdateAdventureSettings)

    bool no_pvm = false;
    bool no_mvp = false;
    bool immutable_world = false;
    bool show_name_tags = true;
    bool auto_jump = true;

    void write(BinaryStream& stream) const override {
        stream.writeBool(no_pvm);
        stream.writeBool(no_mvp);
        stream.writeBool(immutable_world);
        stream.writeBool(show_name_tags);
        stream.writeBool(auto_jump);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        no_pvm = stream.readBool();
        no_mvp = stream.readBool();
        immutable_world = stream.readBool();
        show_name_tags = stream.readBool();
        auto_jump = stream.readBool();
    }
};

} // namespace cphertunnel
