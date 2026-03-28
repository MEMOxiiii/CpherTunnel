// =============================================================================
// CpherTunnel - RakNet Reliability Layer
// Frame encoding/decoding, ACK/NAK, split packets, ordering
// =============================================================================
#pragma once

#include "types.hpp"
#include "bitstream.hpp"
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>
#include <set>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <optional>

namespace cphertunnel::raknet {

// =============================================================================
// Frame (Encapsulated Packet) - A single message within a datagram
// =============================================================================
struct Frame {
    Reliability reliability = Reliability::ReliableOrdered;
    bool is_split = false;
    
    // Reliable message index (for reliable types)
    uint32_t reliable_message_index = 0;
    
    // Sequencing (for sequenced types)
    uint32_t sequencing_index = 0;
    
    // Ordering (for ordered types)
    uint32_t ordering_index = 0;
    uint8_t ordering_channel = 0;
    
    // Split packet info
    uint32_t split_count = 0;
    uint16_t split_id = 0;
    uint32_t split_index = 0;
    
    // Payload
    std::vector<uint8_t> body;
    
    // =========================================================================
    // Encode frame into bitstream
    // =========================================================================
    void encode(BitStream& stream) const {
        // Reliability header byte:
        // bits 7-5: reliability type (3 bits)
        // bit 4: is_split flag
        // bits 3-0: reserved (0)
        uint8_t header = (static_cast<uint8_t>(reliability) << 5) | (is_split ? 0x10 : 0x00);
        stream.writeUint8(header);
        
        // Body length in BITS (16-bit big-endian)
        stream.writeUint16BE(static_cast<uint16_t>(body.size() * 8));
        
        // Reliable message index (24-bit LE) for reliable types
        if (isReliable(reliability)) {
            stream.writeUint24LE(reliable_message_index);
        }
        
        // Sequencing index (24-bit LE) for sequenced types
        if (isSequenced(reliability)) {
            stream.writeUint24LE(sequencing_index);
        }
        
        // Ordering index + channel for ordered types
        if (isOrdered(reliability)) {
            stream.writeUint24LE(ordering_index);
            stream.writeUint8(ordering_channel);
        }
        
        // Split packet header
        if (is_split) {
            stream.writeUint32BE(split_count);
            stream.writeUint16BE(split_id);
            stream.writeUint32BE(split_index);
        }
        
        // Body data
        stream.writeBytes(body);
    }
    
    // =========================================================================
    // Decode frame from bitstream
    // =========================================================================
    static Frame decode(BitStream& stream) {
        Frame frame;
        
        uint8_t header = stream.readUint8();
        frame.reliability = static_cast<Reliability>((header >> 5) & 0x07);
        frame.is_split = (header & 0x10) != 0;
        
        // Body length in bits
        uint16_t body_bits = stream.readUint16BE();
        size_t body_bytes = (body_bits + 7) / 8;
        
        if (isReliable(frame.reliability)) {
            frame.reliable_message_index = stream.readUint24LE();
        }
        
        if (isSequenced(frame.reliability)) {
            frame.sequencing_index = stream.readUint24LE();
        }
        
        if (isOrdered(frame.reliability)) {
            frame.ordering_index = stream.readUint24LE();
            frame.ordering_channel = stream.readUint8();
        }
        
        if (frame.is_split) {
            frame.split_count = stream.readUint32BE();
            frame.split_id = stream.readUint16BE();
            frame.split_index = stream.readUint32BE();
        }
        
        frame.body = stream.readBytes(body_bytes);
        return frame;
    }
    
    // Calculate encoded size in bytes
    size_t encodedSize() const {
        size_t size = 1 + 2; // header + body_bits
        if (isReliable(reliability)) size += 3;
        if (isSequenced(reliability)) size += 3;
        if (isOrdered(reliability)) size += 4; // 3 + 1 channel
        if (is_split) size += 10; // 4 + 2 + 4
        size += body.size();
        return size;
    }
};

// =============================================================================
// Datagram - A UDP packet containing one or more frames
// =============================================================================
struct Datagram {
    uint8_t flags = FlagValid;  // 0x80 for data
    uint32_t sequence_number = 0;
    std::vector<Frame> frames;
    
    // Encode datagram into raw bytes
    std::vector<uint8_t> encode() const {
        BitStream stream;
        stream.writeUint8(flags);
        stream.writeUint24LE(sequence_number);
        
        for (auto& frame : frames) {
            frame.encode(stream);
        }
        
        return stream.toVector();
    }
    
    // Decode datagram from raw bytes
    static Datagram decode(const uint8_t* data, size_t length) {
        BitStream stream(data, length);
        Datagram dg;
        dg.flags = stream.readUint8();
        dg.sequence_number = stream.readUint24LE();
        
        while (stream.hasDataToRead() && stream.getBytesAvailable() > 0) {
            dg.frames.push_back(Frame::decode(stream));
        }
        
        return dg;
    }
    
    // Total size with all frames
    size_t totalSize() const {
        size_t size = 4; // flags + sequence_number
        for (auto& f : frames) {
            size += f.encodedSize();
        }
        return size;
    }
};

// =============================================================================
// ACK/NAK - Range-based acknowledgement packets
// =============================================================================
struct AckNak {
    bool is_ack = true; // true = ACK, false = NAK
    
    // Ranges of sequence numbers: each pair is [start, end] inclusive
    std::vector<std::pair<uint32_t, uint32_t>> ranges;
    
    std::vector<uint8_t> encode() const {
        BitStream stream;
        stream.writeUint8(is_ack ? 0xC0 : 0xA0);
        stream.writeUint16BE(static_cast<uint16_t>(ranges.size()));
        
        for (auto& [start, end] : ranges) {
            if (start == end) {
                stream.writeUint8(1); // single
                stream.writeUint24LE(start);
            } else {
                stream.writeUint8(0); // range
                stream.writeUint24LE(start);
                stream.writeUint24LE(end);
            }
        }
        
        return stream.toVector();
    }
    
    static AckNak decode(const uint8_t* data, size_t length) {
        BitStream stream(data, length);
        AckNak result;
        
        uint8_t flags = stream.readUint8();
        result.is_ack = (flags & FlagACK) != 0;
        
        uint16_t range_count = stream.readUint16BE();
        for (uint16_t i = 0; i < range_count; ++i) {
            uint8_t single = stream.readUint8();
            if (single) {
                uint32_t seq = stream.readUint24LE();
                result.ranges.push_back({seq, seq});
            } else {
                uint32_t start = stream.readUint24LE();
                uint32_t end = stream.readUint24LE();
                result.ranges.push_back({start, end});
            }
        }
        
        return result;
    }
    
    // Build ACK from a set of sequence numbers (compresses into ranges)
    static AckNak buildAck(const std::set<uint32_t>& sequences) {
        AckNak ack;
        ack.is_ack = true;
        
        if (sequences.empty()) return ack;
        
        auto it = sequences.begin();
        uint32_t range_start = *it;
        uint32_t range_end = *it;
        ++it;
        
        while (it != sequences.end()) {
            if (*it == range_end + 1) {
                range_end = *it;
            } else {
                ack.ranges.push_back({range_start, range_end});
                range_start = *it;
                range_end = *it;
            }
            ++it;
        }
        ack.ranges.push_back({range_start, range_end});
        
        return ack;
    }
    
    // Get all sequence numbers from ranges
    std::set<uint32_t> getSequenceNumbers() const {
        std::set<uint32_t> result;
        for (auto& [start, end] : ranges) {
            for (uint32_t i = start; i <= end; ++i) {
                result.insert(i);
            }
        }
        return result;
    }
};

// =============================================================================
// Split Packet Assembler - Collects split packet fragments
// =============================================================================
class SplitPacketAssembler {
public:
    // Add a fragment. Returns complete body if all fragments received.
    std::optional<std::vector<uint8_t>> addFragment(const Frame& frame) {
        if (!frame.is_split) {
            return frame.body; // Not split, return immediately
        }
        
        auto& entry = pending_[frame.split_id];
        entry.total_count = frame.split_count;
        entry.fragments[frame.split_index] = frame.body;
        
        if (entry.fragments.size() == entry.total_count) {
            // All fragments received - assemble
            std::vector<uint8_t> assembled;
            for (uint32_t i = 0; i < entry.total_count; ++i) {
                auto it = entry.fragments.find(i);
                if (it != entry.fragments.end()) {
                    assembled.insert(assembled.end(), it->second.begin(), it->second.end());
                }
            }
            pending_.erase(frame.split_id);
            return assembled;
        }
        
        return std::nullopt;
    }
    
    void clear() { pending_.clear(); }
    
private:
    struct PendingSplit {
        uint32_t total_count = 0;
        std::map<uint32_t, std::vector<uint8_t>> fragments;
    };
    
    std::unordered_map<uint16_t, PendingSplit> pending_;
};

// =============================================================================
// Ordering System - Handles ordered/sequenced channel delivery
// =============================================================================
class OrderingSystem {
public:
    static constexpr uint8_t NUM_ORDERING_CHANNELS = 32;
    
    // Process an ordered frame. Returns frames ready for delivery.
    std::vector<std::vector<uint8_t>> processOrdered(uint8_t channel, uint32_t index,
                                                      std::vector<uint8_t> data) {
        std::vector<std::vector<uint8_t>> ready;
        
        if (channel >= NUM_ORDERING_CHANNELS) return ready;
        
        auto& state = channels_[channel];
        
        if (index == state.expected_index) {
            // In order - deliver immediately
            ready.push_back(std::move(data));
            state.expected_index++;
            
            // Check if buffered packets are now in order
            while (true) {
                auto it = state.buffered.find(state.expected_index);
                if (it == state.buffered.end()) break;
                ready.push_back(std::move(it->second));
                state.buffered.erase(it);
                state.expected_index++;
            }
        } else if (index > state.expected_index) {
            // Future packet - buffer it
            state.buffered[index] = std::move(data);
        }
        // else: old packet - discard
        
        return ready;
    }
    
    // Process a sequenced frame. Returns data only if it's newer.
    std::optional<std::vector<uint8_t>> processSequenced(uint8_t channel, uint32_t index,
                                                          std::vector<uint8_t> data) {
        if (channel >= NUM_ORDERING_CHANNELS) return std::nullopt;
        
        auto& highest = highest_sequenced_[channel];
        if (index >= highest || highest == 0) {
            highest = index + 1;
            return data;
        }
        return std::nullopt; // Old sequenced packet - discard
    }
    
    void reset() {
        for (auto& ch : channels_) {
            ch = ChannelState{};
        }
        std::fill(std::begin(highest_sequenced_), std::end(highest_sequenced_), 0);
    }
    
private:
    struct ChannelState {
        uint32_t expected_index = 0;
        std::map<uint32_t, std::vector<uint8_t>> buffered;
    };
    
    ChannelState channels_[NUM_ORDERING_CHANNELS] = {};
    uint32_t highest_sequenced_[NUM_ORDERING_CHANNELS] = {};
};

// =============================================================================
// ReliabilityLayer - Manages send/receive reliability for one connection
// =============================================================================
class ReliabilityLayer {
public:
    using TimePoint = std::chrono::steady_clock::time_point;
    
    // =========================================================================
    // Sending
    // =========================================================================
    
    // Create a frame for sending with proper indices assigned
    Frame createFrame(std::vector<uint8_t> data, Reliability reliability,
                      uint8_t ordering_channel = 0) {
        Frame frame;
        frame.reliability = reliability;
        frame.body = std::move(data);
        
        if (isReliable(reliability)) {
            frame.reliable_message_index = send_reliable_index_++;
        }
        
        if (isSequenced(reliability)) {
            frame.sequencing_index = send_sequencing_index_[ordering_channel]++;
        }
        
        if (isOrdered(reliability)) {
            frame.ordering_index = send_ordering_index_[ordering_channel]++;
            frame.ordering_channel = ordering_channel;
        }
        
        return frame;
    }
    
    // Split a large message into multiple frames
    std::vector<Frame> splitMessage(std::vector<uint8_t> data, Reliability reliability,
                                     uint16_t mtu, uint8_t ordering_channel = 0) {
        // Estimate max frame body per datagram
        // Datagram header = 4 bytes, frame overhead ~= 28 bytes max
        size_t max_body_size = mtu - 4 - 28 - 20; // -20 for IP/UDP overhead margin
        if (max_body_size > data.size()) max_body_size = data.size();
        
        if (data.size() <= max_body_size) {
            return { createFrame(std::move(data), reliability, ordering_channel) };
        }
        
        // Need to split
        uint32_t split_count = static_cast<uint32_t>((data.size() + max_body_size - 1) / max_body_size);
        uint16_t split_id = send_split_id_++;
        
        // For split packets, assign one reliable and ordering index for the group
        uint32_t reliable_idx = isReliable(reliability) ? send_reliable_index_++ : 0;
        uint32_t ordering_idx = isOrdered(reliability) ? send_ordering_index_[ordering_channel]++ : 0;
        uint32_t sequencing_idx = isSequenced(reliability) ? send_sequencing_index_[ordering_channel]++ : 0;
        
        std::vector<Frame> frames;
        for (uint32_t i = 0; i < split_count; ++i) {
            Frame frame;
            frame.reliability = reliability;
            frame.is_split = true;
            frame.split_count = split_count;
            frame.split_id = split_id;
            frame.split_index = i;
            frame.reliable_message_index = reliable_idx;
            frame.ordering_index = ordering_idx;
            frame.ordering_channel = ordering_channel;
            frame.sequencing_index = sequencing_idx;
            
            size_t offset = i * max_body_size;
            size_t len = std::min(max_body_size, data.size() - offset);
            frame.body.assign(data.begin() + offset, data.begin() + offset + len);
            frames.push_back(std::move(frame));
        }
        
        return frames;
    }
    
    // Build datagram(s) from queued frames
    std::vector<Datagram> buildDatagrams(const std::vector<Frame>& frames, uint16_t mtu) {
        std::vector<Datagram> result;
        Datagram current;
        current.flags = FlagValid;
        current.sequence_number = send_sequence_number_++;
        
        for (auto& frame : frames) {
            size_t frame_size = frame.encodedSize();
            if (current.totalSize() + frame_size > mtu - 20 && !current.frames.empty()) {
                // Flush current datagram
                result.push_back(std::move(current));
                current = Datagram{};
                current.flags = FlagValid;
                current.sequence_number = send_sequence_number_++;
            }
            current.frames.push_back(frame);
        }
        
        if (!current.frames.empty()) {
            result.push_back(std::move(current));
        }
        
        return result;
    }
    
    // Record a sent datagram for potential retransmission
    void recordSentDatagram(uint32_t sequence, const Datagram& dg) {
        sent_datagrams_[sequence] = { dg, std::chrono::steady_clock::now() };
    }
    
    // =========================================================================
    // Receiving
    // =========================================================================
    
    // Process received ACK - removes acknowledged datagrams
    void processAck(const AckNak& ack) {
        auto sequences = ack.getSequenceNumbers();
        for (auto seq : sequences) {
            sent_datagrams_.erase(seq);
        }
    }
    
    // Process received NAK - queue retransmissions
    std::vector<Datagram> processNak(const AckNak& nak) {
        std::vector<Datagram> retransmits;
        auto sequences = nak.getSequenceNumbers();
        for (auto seq : sequences) {
            auto it = sent_datagrams_.find(seq);
            if (it != sent_datagrams_.end()) {
                // Re-assign new sequence number for retransmission
                Datagram dg = it->second.datagram;
                dg.sequence_number = send_sequence_number_++;
                retransmits.push_back(dg);
                sent_datagrams_.erase(it);
            }
        }
        return retransmits;
    }
    
    // Record received datagram sequence for ACK generation
    void recordReceivedSequence(uint32_t sequence) {
        received_sequences_.insert(sequence);
    }
    
    // Build ACK for all received sequences, then clear
    AckNak buildAck() {
        auto ack = AckNak::buildAck(received_sequences_);
        received_sequences_.clear();
        return ack;
    }
    
    // Check for datagrams that need retransmission (timeout-based)
    std::vector<Datagram> getTimedOutDatagrams(std::chrono::milliseconds timeout) {
        auto now = std::chrono::steady_clock::now();
        std::vector<Datagram> retransmits;
        std::vector<uint32_t> to_remove;
        
        for (auto& [seq, entry] : sent_datagrams_) {
            if (now - entry.sent_time > timeout) {
                Datagram dg = entry.datagram;
                dg.sequence_number = send_sequence_number_++;
                retransmits.push_back(dg);
                to_remove.push_back(seq);
            }
        }
        
        for (auto seq : to_remove) {
            sent_datagrams_.erase(seq);
        }
        
        return retransmits;
    }
    
    // Process received frame through split assembly and ordering
    std::vector<std::vector<uint8_t>> processReceivedFrame(const Frame& frame) {
        std::vector<std::vector<uint8_t>> results;
        
        // Handle split packets
        auto assembled = split_assembler_.addFragment(frame);
        if (!assembled.has_value()) {
            return results; // Waiting for more fragments
        }
        
        auto& data = assembled.value();
        
        // Handle ordering
        if (isOrdered(frame.reliability) && !isSequenced(frame.reliability)) {
            return ordering_.processOrdered(frame.ordering_channel, frame.ordering_index, std::move(data));
        }
        
        if (isSequenced(frame.reliability)) {
            auto result = ordering_.processSequenced(frame.ordering_channel, frame.sequencing_index, std::move(data));
            if (result.has_value()) {
                results.push_back(std::move(result.value()));
            }
            return results;
        }
        
        // Unreliable/reliable without ordering - deliver immediately
        results.push_back(std::move(data));
        return results;
    }
    
    // =========================================================================
    // State
    // =========================================================================
    
    uint32_t getSendSequenceNumber() const { return send_sequence_number_; }
    
    void reset() {
        send_sequence_number_ = 0;
        send_reliable_index_ = 0;
        send_split_id_ = 0;
        std::fill(std::begin(send_ordering_index_), std::end(send_ordering_index_), 0);
        std::fill(std::begin(send_sequencing_index_), std::end(send_sequencing_index_), 0);
        sent_datagrams_.clear();
        received_sequences_.clear();
        split_assembler_.clear();
        ordering_.reset();
    }
    
private:
    struct SentEntry {
        Datagram datagram;
        TimePoint sent_time;
    };
    
    // Send state
    uint32_t send_sequence_number_ = 0;
    uint32_t send_reliable_index_ = 0;
    uint16_t send_split_id_ = 0;
    uint32_t send_ordering_index_[32] = {};
    uint32_t send_sequencing_index_[32] = {};
    
    // Sent datagrams awaiting ACK
    std::map<uint32_t, SentEntry> sent_datagrams_;
    
    // Received sequences for ACK generation
    std::set<uint32_t> received_sequences_;
    
    // Fragment assembly
    SplitPacketAssembler split_assembler_;
    
    // Ordering
    OrderingSystem ordering_;
};

} // namespace cphertunnel::raknet
