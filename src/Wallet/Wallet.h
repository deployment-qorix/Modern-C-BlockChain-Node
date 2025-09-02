#pragma once
#include <string>
#include <openssl/rsa.h>

class Mempool;
class Transaction;

class Wallet {
public:
    Wallet();
    ~Wallet();

    void generateKeys();
    std::string getPublicKey() const;
    RSA* getPrivateKey() const;

    Transaction createTransaction(const std::string& to, double amount);
    void sendFunds(const std::string& to, double amount, Mempool& mempool);

private:
    RSA* _privateKey{nullptr};
    std::string _publicKey;
};
