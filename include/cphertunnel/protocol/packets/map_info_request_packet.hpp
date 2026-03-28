// CpherTunnel - MapInfoRequestPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class MapInfoRequestPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(MapInfoRequestPacket, MapInfoRequest)

    ActorUniqueID map_id;
    std::vector<uint32_t> client_pixel_request;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(map_id);
        stream.writeUnsignedInt(static_cast<uint32_t>(client_pixel_request.size()));
        for (auto p : client_pixel_request) {
            stream.writeUnsignedInt(p);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        map_id = stream.readActorUniqueID();
        uint32_t count = stream.readUnsignedInt();
        client_pixel_request.resize(count);
        for (auto& p : client_pixel_request) {
            p = stream.readUnsignedInt();
        }
    }
};

} // namespace cphertunnel
