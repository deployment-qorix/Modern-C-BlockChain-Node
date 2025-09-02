#pragma once
#include <string>
#include <openssl/rsa.h> // Include this for RSA*

namespace Crypto {
    std::string sha256(const std::string& data);
    RSA* generateKeyPair();
    std::string getPublicKey(RSA* key);
    std::string sign(const std::string& data, RSA* privateKey);
    bool verify(const std::string& data, const std::string& signature, const std::string& publicKey);
}