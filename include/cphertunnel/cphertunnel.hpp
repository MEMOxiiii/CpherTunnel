// =============================================================================
// CpherTunnel - Minecraft Bedrock Edition Protocol Library
// Protocol Version: 944 (Minecraft Bedrock 1.26.1)
//
// A complete, header-only C++17 library for encoding/decoding
// Minecraft Bedrock Edition network protocol packets over RakNet.
//
// License: MIT
// =============================================================================
#pragma once

// ─── Core ────────────────────────────────────────────────────────
#include "cphertunnel/protocol/types.hpp"
#include "cphertunnel/protocol/binary_stream.hpp"
#include "cphertunnel/protocol/varint.hpp"
#include "cphertunnel/protocol/nbt.hpp"

// ─── Protocol ────────────────────────────────────────────────────
#include "cphertunnel/protocol/packet_ids.hpp"
#include "cphertunnel/protocol/packet.hpp"
#include "cphertunnel/protocol/packet_codec.hpp"

// ─── Shared Packet Types ─────────────────────────────────────────
#include "cphertunnel/protocol/packet_types.hpp"

// ─── All Packets (category headers - existing 190 packets) ──────
#include "cphertunnel/protocol/packets/login_packets.hpp"
#include "cphertunnel/protocol/packets/play_packets.hpp"
#include "cphertunnel/protocol/packets/world_packets.hpp"
#include "cphertunnel/protocol/packets/inventory_packets.hpp"
#include "cphertunnel/protocol/packets/entity_packets.hpp"
#include "cphertunnel/protocol/packets/command_packets.hpp"
#include "cphertunnel/protocol/packets/resource_pack_packets.hpp"
#include "cphertunnel/protocol/packets/camera_packets.hpp"
#include "cphertunnel/protocol/packets/ui_packets.hpp"
#include "cphertunnel/protocol/packets/sound_packets.hpp"
#include "cphertunnel/protocol/packets/movement_packets.hpp"
#include "cphertunnel/protocol/packets/misc_packets.hpp"

// ─── Individual Packets (new implementations) ────────────────────
#include "cphertunnel/protocol/packets/keep_alive_packet.hpp"
#include "cphertunnel/protocol/packets/add_player_packet.hpp"
#include "cphertunnel/protocol/packets/add_actor_packet.hpp"
#include "cphertunnel/protocol/packets/add_item_actor_packet.hpp"
#include "cphertunnel/protocol/packets/update_attributes_packet.hpp"
#include "cphertunnel/protocol/packets/inventory_transaction_packet.hpp"
#include "cphertunnel/protocol/packets/mob_equipment_packet.hpp"
#include "cphertunnel/protocol/packets/mob_armor_equipment_packet.hpp"
#include "cphertunnel/protocol/packets/set_actor_data_packet.hpp"
#include "cphertunnel/protocol/packets/inventory_content_packet.hpp"
#include "cphertunnel/protocol/packets/inventory_slot_packet.hpp"
#include "cphertunnel/protocol/packets/crafting_data_packet.hpp"
#include "cphertunnel/protocol/packets/player_list_packet.hpp"
#include "cphertunnel/protocol/packets/map_data_packet.hpp"
#include "cphertunnel/protocol/packets/boss_event_packet.hpp"
#include "cphertunnel/protocol/packets/command_block_update_packet.hpp"
#include "cphertunnel/protocol/packets/structure_block_update_packet.hpp"
#include "cphertunnel/protocol/packets/sub_client_login_packet.hpp"
#include "cphertunnel/protocol/packets/set_score_packet.hpp"
#include "cphertunnel/protocol/packets/move_actor_delta_packet.hpp"
#include "cphertunnel/protocol/packets/set_scoreboard_identity_packet.hpp"
#include "cphertunnel/protocol/packets/ping_packet.hpp"
#include "cphertunnel/protocol/packets/script_custom_event_packet.hpp"
#include "cphertunnel/protocol/packets/client_cache_blob_status_packet.hpp"
#include "cphertunnel/protocol/packets/client_cache_miss_response_packet.hpp"
#include "cphertunnel/protocol/packets/player_auth_input_packet.hpp"
#include "cphertunnel/protocol/packets/player_enchant_options_packet.hpp"
#include "cphertunnel/protocol/packets/item_stack_request_packet.hpp"
#include "cphertunnel/protocol/packets/item_stack_response_packet.hpp"
#include "cphertunnel/protocol/packets/sub_chunk_packet.hpp"
#include "cphertunnel/protocol/packets/sub_chunk_request_packet.hpp"
#include "cphertunnel/protocol/packets/dimension_data_packet.hpp"
#include "cphertunnel/protocol/packets/update_abilities_packet.hpp"
#include "cphertunnel/protocol/packets/player_client_input_permissions_packet.hpp"
#include "cphertunnel/protocol/packets/compressed_biome_definition_list_packet.hpp"
#include "cphertunnel/protocol/packets/trim_data_packet.hpp"
#include "cphertunnel/protocol/packets/client_movement_prediction_sync_packet.hpp"
#include "cphertunnel/protocol/packets/player_update_entity_overrides_packet.hpp"
#include "cphertunnel/protocol/packets/player_location_packet.hpp"
#include "cphertunnel/protocol/packets/server_script_debug_drawer_packet.hpp"

// ─── RakNet ──────────────────────────────────────────────────────
#include "cphertunnel/raknet/types.hpp"
#include "cphertunnel/raknet/bitstream.hpp"
#include "cphertunnel/raknet/socket.hpp"
#include "cphertunnel/raknet/reliability.hpp"
#include "cphertunnel/raknet/connection.hpp"
#include "cphertunnel/raknet/peer.hpp"

// ─── Network Layer ───────────────────────────────────────────────
#include "cphertunnel/network/compression.hpp"
#include "cphertunnel/network/encryption.hpp"
#include "cphertunnel/network/batch_packet.hpp"
#include "cphertunnel/network/connection_handler.hpp"
