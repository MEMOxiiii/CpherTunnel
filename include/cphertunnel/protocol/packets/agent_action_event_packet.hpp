// CpherTunnel - AgentActionEventPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class AgentActionEventPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(AgentActionEventPacket, AgentAction)

    enum class AgentAction : int32_t {
        None              = 0,
        Attack            = 1,
        Collect           = 2,
        Destroy           = 3,
        DetectRedstone    = 4,
        DetectObstacle    = 5,
        Drop              = 6,
        DropAll           = 7,
        Inspect           = 8,
        InspectData       = 9,
        InspectItem       = 10,
        InspectItemDetail = 11,
        InspectItemCount  = 12,
        InspectItemSpace  = 13,
        Interact          = 14,
        Move              = 15,
        PlaceBlock        = 16,
        Till              = 17,
        TransferTo        = 18,
        Turn              = 19,
    };

    std::string request_id;
    AgentAction action = AgentAction::None;
    std::string response;

    void write(BinaryStream& stream) const override {
        stream.writeString(request_id);
        stream.writeVarInt(static_cast<int32_t>(action));
        stream.writeString(response);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        request_id = stream.readString();
        action = static_cast<AgentAction>(stream.readVarInt());
        response = stream.readString();
    }
};

} // namespace cphertunnel
