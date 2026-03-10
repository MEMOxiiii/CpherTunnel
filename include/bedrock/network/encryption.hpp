// =============================================================================
// BedrockProtocol - Network Encryption
// AES-256-CTR/GCM encryption for Bedrock protocol
// =============================================================================
#pragma once

#include <vector>
#include <cstdint>
#include <cstring>
#include <array>
#include <memory>
#include <stdexcept>

// Optional: If OpenSSL is available
#ifdef BEDROCK_HAS_OPENSSL
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#endif

namespace bedrock::network {

// =============================================================================
// Encryption Interface
// Bedrock uses AES-256-CFB8 (after NetworkSettings, if enabled)
// The key is derived from ECDH shared secret + SHA-256
// =============================================================================
class Encryption {
public:
    virtual ~Encryption() = default;
    
    virtual bool isEnabled() const = 0;
    virtual void enable(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) = 0;
    virtual void disable() = 0;
    
    // Encrypt in-place (appends checksum)
    virtual std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data) = 0;
    
    // Decrypt (verifies and removes checksum)
    virtual std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data) = 0;
};

// =============================================================================
// No Encryption (passthrough)
// =============================================================================
class NoEncryption : public Encryption {
public:
    bool isEnabled() const override { return false; }
    void enable(const std::vector<uint8_t>&, const std::vector<uint8_t>&) override {}
    void disable() override {}
    
    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data) override {
        return data;
    }
    
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data) override {
        return data;
    }
};

// =============================================================================
// AES-256-CFB8 Encryption (Bedrock protocol)
// Requires OpenSSL. The protocol uses:
// - AES-256-CFB8 mode
// - First 12 bytes of the key as the initial IV
// - SHA-256 checksum of (counter || plaintext) appended (last 8 bytes)
// =============================================================================
#ifdef BEDROCK_HAS_OPENSSL

class AESEncryption : public Encryption {
public:
    AESEncryption() = default;
    
    ~AESEncryption() override {
        disable();
    }
    
    bool isEnabled() const override { return enabled_; }
    
    void enable(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) override {
        if (key.size() != 32) {
            throw std::runtime_error("AES key must be 32 bytes");
        }
        if (iv.size() < 16) {
            throw std::runtime_error("AES IV must be at least 16 bytes");
        }
        
        key_ = key;
        
        // Encrypt context
        encrypt_ctx_ = EVP_CIPHER_CTX_new();
        EVP_EncryptInit_ex(encrypt_ctx_, EVP_aes_256_cfb8(), nullptr,
                           key_.data(), iv.data());
        
        // Decrypt context
        decrypt_ctx_ = EVP_CIPHER_CTX_new();
        EVP_DecryptInit_ex(decrypt_ctx_, EVP_aes_256_cfb8(), nullptr,
                           key_.data(), iv.data());
        
        send_counter_ = 0;
        recv_counter_ = 0;
        enabled_ = true;
    }
    
    void disable() override {
        if (encrypt_ctx_) { EVP_CIPHER_CTX_free(encrypt_ctx_); encrypt_ctx_ = nullptr; }
        if (decrypt_ctx_) { EVP_CIPHER_CTX_free(decrypt_ctx_); decrypt_ctx_ = nullptr; }
        enabled_ = false;
    }
    
    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data) override {
        if (!enabled_) return data;
        
        // Compute checksum: SHA-256(counter_LE_8bytes || plaintext || key)
        auto checksum = computeChecksum(data, send_counter_);
        send_counter_++;
        
        // Prepare: plaintext + 8-byte checksum
        std::vector<uint8_t> plaintext;
        plaintext.reserve(data.size() + 8);
        plaintext.insert(plaintext.end(), data.begin(), data.end());
        plaintext.insert(plaintext.end(), checksum.begin(), checksum.end());
        
        // Encrypt
        std::vector<uint8_t> encrypted(plaintext.size());
        int out_len = 0;
        EVP_EncryptUpdate(encrypt_ctx_, encrypted.data(), &out_len,
                          plaintext.data(), static_cast<int>(plaintext.size()));
        encrypted.resize(out_len);
        
        return encrypted;
    }
    
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data) override {
        if (!enabled_) return data;
        
        // Decrypt
        std::vector<uint8_t> decrypted(data.size());
        int out_len = 0;
        EVP_DecryptUpdate(decrypt_ctx_, decrypted.data(), &out_len,
                          data.data(), static_cast<int>(data.size()));
        decrypted.resize(out_len);
        
        if (decrypted.size() < 8) {
            throw std::runtime_error("Decrypted data too short for checksum");
        }
        
        // Split: payload + 8-byte checksum
        std::vector<uint8_t> payload(decrypted.begin(), decrypted.end() - 8);
        std::vector<uint8_t> received_checksum(decrypted.end() - 8, decrypted.end());
        
        // Verify checksum
        auto expected_checksum = computeChecksum(payload, recv_counter_);
        recv_counter_++;
        
        if (received_checksum != expected_checksum) {
            throw std::runtime_error("Encryption checksum mismatch");
        }
        
        return payload;
    }
    
private:
    std::array<uint8_t, 8> computeChecksum(const std::vector<uint8_t>& data, uint64_t counter) {
        // SHA-256(counter_LE_8bytes || data || key)
        uint8_t counter_bytes[8];
        for (int i = 0; i < 8; ++i) {
            counter_bytes[i] = static_cast<uint8_t>((counter >> (i * 8)) & 0xFF);
        }
        
        SHA256_CTX ctx;
        SHA256_Init(&ctx);
        SHA256_Update(&ctx, counter_bytes, 8);
        SHA256_Update(&ctx, data.data(), data.size());
        SHA256_Update(&ctx, key_.data(), key_.size());
        
        uint8_t hash[SHA256_DIGEST_LENGTH];
        SHA256_Final(hash, &ctx);
        
        // Return first 8 bytes
        std::array<uint8_t, 8> result;
        std::memcpy(result.data(), hash, 8);
        return result;
    }
    
    EVP_CIPHER_CTX* encrypt_ctx_ = nullptr;
    EVP_CIPHER_CTX* decrypt_ctx_ = nullptr;
    std::vector<uint8_t> key_;
    uint64_t send_counter_ = 0;
    uint64_t recv_counter_ = 0;
    bool enabled_ = false;
};

#endif // BEDROCK_HAS_OPENSSL

// =============================================================================
// Factory
// =============================================================================
inline std::unique_ptr<Encryption> createEncryption() {
#ifdef BEDROCK_HAS_OPENSSL
    return std::make_unique<AESEncryption>();
#else
    return std::make_unique<NoEncryption>();
#endif
}

} // namespace bedrock::network
