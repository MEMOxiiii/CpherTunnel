// CpherTunnel - TextPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {


enum class TextType : uint8_t {
    Raw             = 0,
    Chat            = 1,
    Translate       = 2,
    Popup           = 3,
    JukeboxPopup    = 4,
    Tip             = 5,
    SystemMessage   = 6,
    Whisper         = 7,
    Announcement    = 8,
    TextObjectWhisper     = 9,
    TextObject            = 10,
    TextObjectAnnouncement = 11
};
enum class TitleType : int32_t {
    Clear                   = 0,
    Reset                   = 1,
    Title                   = 2,
    Subtitle                = 3,
    Actionbar               = 4,
    Times                   = 5,
    TitleTextObject          = 6,
    SubtitleTextObject       = 7,
    ActionbarTextObject      = 8
};

class TextPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(TextPacket, Text)

    TextType type = TextType::Raw;
    bool needs_translation = false;
    std::string source_name;     // player name (for Chat, Whisper, Announcement)
    std::string message;
    std::vector<std::string> parameters; // for Translate, Popup, JukeboxPopup
    std::string xuid;
    std::string platform_chat_id;
    std::string filtered_message;

    void write(BinaryStream& stream) const override {
        stream.writeByte(static_cast<uint8_t>(type));
        stream.writeBool(needs_translation);

        switch (type) {
            case TextType::Chat:
            case TextType::Whisper:
            case TextType::Announcement:
                stream.writeString(source_name);
                stream.writeString(message);
                break;
            case TextType::Raw:
            case TextType::Tip:
            case TextType::SystemMessage:
            case TextType::TextObjectWhisper:
            case TextType::TextObject:
            case TextType::TextObjectAnnouncement:
                stream.writeString(message);
                break;
            case TextType::Translate:
            case TextType::Popup:
            case TextType::JukeboxPopup:
                stream.writeString(message);
                stream.writeUnsignedVarInt(static_cast<uint32_t>(parameters.size()));
                for (auto& p : parameters) stream.writeString(p);
                break;
        }

        stream.writeString(xuid);
        stream.writeString(platform_chat_id);
        stream.writeString(filtered_message);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        type = static_cast<TextType>(stream.readByte());
        needs_translation = stream.readBool();

        switch (type) {
            case TextType::Chat:
            case TextType::Whisper:
            case TextType::Announcement:
                source_name = stream.readString();
                message = stream.readString();
                break;
            case TextType::Raw:
            case TextType::Tip:
            case TextType::SystemMessage:
            case TextType::TextObjectWhisper:
            case TextType::TextObject:
            case TextType::TextObjectAnnouncement:
                message = stream.readString();
                break;
            case TextType::Translate:
            case TextType::Popup:
            case TextType::JukeboxPopup:
                message = stream.readString();
                {
                    uint32_t count = stream.readUnsignedVarInt();
                    parameters.resize(count);
                    for (auto& p : parameters) p = stream.readString();
                }
                break;
        }

        xuid = stream.readString();
        platform_chat_id = stream.readString();
        filtered_message = stream.readString();
    }
};

} // namespace cphertunnel
