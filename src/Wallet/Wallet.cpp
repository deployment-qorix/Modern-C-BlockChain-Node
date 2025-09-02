#include "Wallet.h"
#include "../crypto/Utils.h"
#include "../core/Transaction.h"
#include "../Mempool/Mempool.h"
#include <iostream>

Wallet::Wallet() {
    generateKeys();
}

Wallet::~Wallet() {
    if (_privateKey) {
        RSA_free(_privateKey);
        _privateKey = nullptr;
    }
}

void Wallet::generateKeys() {
    // Free old key if regenerating
    if (_privateKey) {
        RSA_free(_privateKey);
        _privateKey = nullptr;
    }

    _privateKey = Crypto::generateKeyPair();   // ✅ standardized call
    _publicKey  = Crypto::getPublicKey(_privateKey);
}

std::string Wallet::getPublicKey() const {
    return _publicKey;
}

RSA* Wallet::getPrivateKey() const {
    return _privateKey;
}

Transaction Wallet::createTransaction(const std::string& to, double amount) {
    Transaction tx(_publicKey, to, amount);
    tx.sign_transaction(_privateKey);
    return tx;
}

void Wallet::sendFunds(const std::string& to, double amount, Mempool& mempool) {
    Transaction tx = createTransaction(to, amount);
    if (tx.is_valid()) {
        mempool.addTransaction(tx);
        std::cout << "✅ Transaction queued in mempool" << std::endl;
    } else {
        std::cout << "❌ Transaction invalid; not queued" << std::endl;
    }
}
