// CpherTunnel - ItemStackResponsePacket (ID: 148)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {

class ItemStackResponsePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ItemStackResponsePacket, ItemStackResponse)

    struct SlotInfo {
        uint8_t requested_slot = 0;
        uint8_t slot = 0;
        uint8_t amount = 0;
        int32_t stack_net_id = 0;
        std::string custom_name;
        std::string filtered_custom_name;
        int32_t durability_correction = 0;

        void write(BinaryStream& s) const {
            s.writeByte(requested_slot);
            s.writeByte(slot);
            s.writeByte(amount);
            s.writeVarInt(stack_net_id);
            s.writeString(custom_name);
            s.writeString(filtered_custom_name);
            s.writeVarInt(durability_correction);
        }
        void read(ReadOnlyBinaryStream& s) {
            requested_slot = s.readByte();
            slot = s.readByte();
            amount = s.readByte();
            stack_net_id = s.readVarInt();
            custom_name = s.readString();
            filtered_custom_name = s.readString();
            durability_correction = s.readVarInt();
        }
    };

    struct ContainerInfo {
        FullContainerName container_name;
        std::vector<SlotInfo> slots;

        void write(BinaryStream& s) const {
            container_name.write(s);
            s.writeUnsignedVarInt(static_cast<uint32_t>(slots.size()));
            for (auto& sl : slots) sl.write(s);
        }
        void read(ReadOnlyBinaryStream& s) {
            container_name.read(s);
            uint32_t c = s.readUnsignedVarInt();
            slots.resize(c);
            for (auto& sl : slots) sl.read(s);
        }
    };

    struct ResponseInfo {
        uint8_t result = 0;
        int32_t client_request_id = 0;
        std::vector<ContainerInfo> containers; // only if result == 0 (success)

        void write(BinaryStream& s) const {
            s.writeByte(result);
            s.writeVarInt(client_request_id);
            if (result == 0) {
                s.writeUnsignedVarInt(static_cast<uint32_t>(containers.size()));
                for (auto& c : containers) c.write(s);
            }
        }
        void read(ReadOnlyBinaryStream& s) {
            result = s.readByte();
            client_request_id = s.readVarInt();
            if (result == 0) {
                uint32_t cc = s.readUnsignedVarInt();
                containers.resize(cc);
                for (auto& c : containers) c.read(s);
            }
        }
    };

    std::vector<ResponseInfo> responses;

    void write(BinaryStream& s) const override {
        s.writeUnsignedVarInt(static_cast<uint32_t>(responses.size()));
        for (auto& r : responses) r.write(s);
    }
    void read(ReadOnlyBinaryStream& s) override {
        uint32_t rc = s.readUnsignedVarInt();
        responses.resize(rc);
        for (auto& r : responses) r.read(s);
    }
};

} // namespace cphertunnel
