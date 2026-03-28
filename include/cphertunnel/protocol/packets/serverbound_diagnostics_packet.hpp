// CpherTunnel - ServerboundDiagnosticsPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ServerboundDiagnosticsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ServerboundDiagnosticsPacket, ServerboundDiagnostics)

    float avg_fps = 0.0f;
    float avg_server_sim_tick_time = 0.0f;
    float avg_client_sim_tick_time = 0.0f;
    float avg_begin_frame_time = 0.0f;
    float avg_input_time = 0.0f;
    float avg_render_time = 0.0f;
    float avg_end_frame_time = 0.0f;
    float avg_remaining_time_percent = 0.0f;
    float avg_unaccounted_time_percent = 0.0f;

    void write(BinaryStream& stream) const override {
        stream.writeFloat(avg_fps);
        stream.writeFloat(avg_server_sim_tick_time);
        stream.writeFloat(avg_client_sim_tick_time);
        stream.writeFloat(avg_begin_frame_time);
        stream.writeFloat(avg_input_time);
        stream.writeFloat(avg_render_time);
        stream.writeFloat(avg_end_frame_time);
        stream.writeFloat(avg_remaining_time_percent);
        stream.writeFloat(avg_unaccounted_time_percent);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        avg_fps = stream.readFloat();
        avg_server_sim_tick_time = stream.readFloat();
        avg_client_sim_tick_time = stream.readFloat();
        avg_begin_frame_time = stream.readFloat();
        avg_input_time = stream.readFloat();
        avg_render_time = stream.readFloat();
        avg_end_frame_time = stream.readFloat();
        avg_remaining_time_percent = stream.readFloat();
        avg_unaccounted_time_percent = stream.readFloat();
    }
};

} // namespace cphertunnel
