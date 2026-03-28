// CpherTunnel - EducationSettingsPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class EducationSettingsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(EducationSettingsPacket, EducationSettings)

    std::string code_builder_uri;
    std::string code_builder_title;
    bool can_resize_code_builder = false;
    bool has_override_uri = false;
    std::string override_uri;
    bool has_quiz = false;
    std::string quiz_url;

    void write(BinaryStream& stream) const override {
        stream.writeString(code_builder_uri);
        stream.writeString(code_builder_title);
        stream.writeBool(can_resize_code_builder);
        stream.writeBool(has_override_uri);
        if (has_override_uri) {
            stream.writeString(override_uri);
        }
        stream.writeBool(has_quiz);
        if (has_quiz) {
            stream.writeString(quiz_url);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        code_builder_uri = stream.readString();
        code_builder_title = stream.readString();
        can_resize_code_builder = stream.readBool();
        has_override_uri = stream.readBool();
        if (has_override_uri) {
            override_uri = stream.readString();
        }
        has_quiz = stream.readBool();
        if (has_quiz) {
            quiz_url = stream.readString();
        }
    }
};

} // namespace cphertunnel
