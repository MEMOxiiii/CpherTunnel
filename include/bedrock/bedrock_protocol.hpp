// =============================================================================
// BedrockProtocol - Minecraft Bedrock Edition Protocol Library
// Protocol Version: 944 (Minecraft Bedrock 1.26.1)
//
// A complete, header-only C++17 library for encoding/decoding
// Minecraft Bedrock Edition network protocol packets over RakNet.
//
// License: MIT
// =============================================================================
#pragma once

// ─── Core ────────────────────────────────────────────────────────
#include "bedrock/protocol/types.hpp"
#include "bedrock/protocol/binary_stream.hpp"
#include "bedrock/protocol/varint.hpp"
#include "bedrock/protocol/nbt.hpp"

// ─── Protocol ────────────────────────────────────────────────────
#include "bedrock/protocol/packet_ids.hpp"
#include "bedrock/protocol/packet.hpp"
#include "bedrock/protocol/packet_codec.hpp"

// ─── All Packets ─────────────────────────────────────────────────
#include "bedrock/protocol/packets/login_packets.hpp"
#include "bedrock/protocol/packets/play_packets.hpp"
#include "bedrock/protocol/packets/world_packets.hpp"
#include "bedrock/protocol/packets/inventory_packets.hpp"
#include "bedrock/protocol/packets/entity_packets.hpp"
#include "bedrock/protocol/packets/command_packets.hpp"
#include "bedrock/protocol/packets/resource_pack_packets.hpp"
#include "bedrock/protocol/packets/camera_packets.hpp"
#include "bedrock/protocol/packets/ui_packets.hpp"
#include "bedrock/protocol/packets/sound_packets.hpp"
#include "bedrock/protocol/packets/movement_packets.hpp"
#include "bedrock/protocol/packets/misc_packets.hpp"

// ─── RakNet ──────────────────────────────────────────────────────
#include "bedrock/raknet/types.hpp"
#include "bedrock/raknet/bitstream.hpp"
#include "bedrock/raknet/socket.hpp"
#include "bedrock/raknet/reliability.hpp"
#include "bedrock/raknet/connection.hpp"
#include "bedrock/raknet/peer.hpp"

// ─── Network Layer ───────────────────────────────────────────────
#include "bedrock/network/compression.hpp"
#include "bedrock/network/encryption.hpp"
#include "bedrock/network/batch_packet.hpp"
#include "bedrock/network/connection_handler.hpp"
