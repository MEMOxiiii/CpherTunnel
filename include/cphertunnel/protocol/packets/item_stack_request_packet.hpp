// CpherTunnel - ItemStackRequestPacket (ID: 147)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {

class ItemStackRequestPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ItemStackRequestPacket, ItemStackRequest)

    struct StackRequestAction {
        uint8_t action_type = 0;
        uint8_t amount = 0;
        ItemStackRequestSlotInfo source;
        ItemStackRequestSlotInfo destination;
        void write(BinaryStream& s) const {
            s.writeByte(action_type);
            s.writeByte(amount);
            source.write(s);
            destination.write(s);
        }
        void read(ReadOnlyBinaryStream& s) {
            action_type = s.readByte();
            amount = s.readByte();
            source.read(s);
            destination.read(s);
        }
    };

    struct Request {
        int32_t client_request_id = 0;
        std::vector<StackRequestAction> actions;
        std::vector<std::string> strings_to_filter;
        int32_t strings_to_filter_origin = 0;

        void write(BinaryStream& s) const {
            s.writeVarInt(client_request_id);
            s.writeUnsignedVarInt(static_cast<uint32_t>(actions.size()));
            for (auto& a : actions) a.write(s);
            s.writeUnsignedVarInt(static_cast<uint32_t>(strings_to_filter.size()));
            for (auto& str : strings_to_filter) s.writeString(str);
            s.writeSignedInt(strings_to_filter_origin);
        }
        void read(ReadOnlyBinaryStream& s) {
            client_request_id = s.readVarInt();
            uint32_t ac = s.readUnsignedVarInt();
            actions.resize(ac);
            for (auto& a : actions) a.read(s);
            uint32_t sc = s.readUnsignedVarInt();
            strings_to_filter.resize(sc);
            for (auto& str : strings_to_filter) str = s.readString();
            strings_to_filter_origin = s.readSignedInt();
        }
    };

    std::vector<Request> requests;

    void write(BinaryStream& s) const override {
        s.writeUnsignedVarInt(static_cast<uint32_t>(requests.size()));
        for (auto& r : requests) r.write(s);
    }
    void read(ReadOnlyBinaryStream& s) override {
        uint32_t rc = s.readUnsignedVarInt();
        requests.resize(rc);
        for (auto& r : requests) r.read(s);
    }
};

} // namespace cphertunnel
