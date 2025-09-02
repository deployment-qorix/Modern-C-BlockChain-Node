#include "Utils.h"
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

#include <iostream>
#include <sstream>
#include <iomanip>

// Hash data using SHA-256
std::string Crypto::sha256(const std::string& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.c_str(), data.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

// Generate an RSA key pair
RSA* Crypto::generateKeyPair() {
    RSA *rsa = nullptr;
    BIGNUM *bne = BN_new();
    if (BN_set_word(bne, RSA_F4) != 1) {
        BN_free(bne);
        return nullptr;
    }
    rsa = RSA_new();
    if (RSA_generate_key_ex(rsa, 2048, bne, nullptr) != 1) {
        RSA_free(rsa);
        rsa = nullptr;
    }
    BN_free(bne);
    return rsa;
}

// Get the public key from an RSA key pair
std::string Crypto::getPublicKey(RSA* key) {
    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPublicKey(bio, key);
    char* pubkey_pem = nullptr;
    long len = BIO_get_mem_data(bio, &pubkey_pem);
    std::string pub_key_str(pubkey_pem, len);
    BIO_free(bio);
    return pub_key_str;
}

// Sign data using the private key
std::string Crypto::sign(const std::string& data, RSA* privateKey) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)data.c_str(), data.size(), hash);

    unsigned int sig_len = RSA_size(privateKey);
    unsigned char* sig = (unsigned char*)malloc(sig_len);

    if (RSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH, sig, &sig_len, privateKey) != 1) {
        free(sig);
        return "";
    }
    
    std::string signature(reinterpret_cast<const char*>(sig), sig_len);
    free(sig);
    return signature;
}

// Verify data using the public key and signature
bool Crypto::verify(const std::string& data, const std::string& signature, const std::string& publicKey) {
    BIO *bio = BIO_new_mem_buf(publicKey.c_str(), -1);
    RSA* rsa_pub = PEM_read_bio_RSAPublicKey(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);

    if (!rsa_pub) {
        return false;
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)data.c_str(), data.size(), hash);
    
    bool result = RSA_verify(NID_sha256, hash, SHA256_DIGEST_LENGTH, (const unsigned char*)signature.c_str(), signature.size(), rsa_pub);
    RSA_free(rsa_pub);
    return result;
}