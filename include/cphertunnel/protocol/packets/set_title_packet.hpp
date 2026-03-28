// CpherTunnel - SetTitlePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SetTitlePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SetTitlePacket, SetTitle)

    TitleType title_type = TitleType::Clear;
    std::string title_text;
    int32_t fade_in_time = 0;
    int32_t stay_time = 0;
    int32_t fade_out_time = 0;
    std::string xuid;
    std::string platform_online_id;
    std::string filtered_title_text;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(static_cast<int32_t>(title_type));
        stream.writeString(title_text);
        stream.writeVarInt(fade_in_time);
        stream.writeVarInt(stay_time);
        stream.writeVarInt(fade_out_time);
        stream.writeString(xuid);
        stream.writeString(platform_online_id);
        stream.writeString(filtered_title_text);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        title_type = static_cast<TitleType>(stream.readVarInt());
        title_text = stream.readString();
        fade_in_time = stream.readVarInt();
        stay_time = stream.readVarInt();
        fade_out_time = stream.readVarInt();
        xuid = stream.readString();
        platform_online_id = stream.readString();
        filtered_title_text = stream.readString();
    }
};

} // namespace cphertunnel
