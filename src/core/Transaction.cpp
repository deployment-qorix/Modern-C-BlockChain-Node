#include "Transaction.h"
#include <sstream>
#include <openssl/sha.h>
#include "../crypto/Utils.h"

// Transaction constructor
Transaction::Transaction(const std::string& from_address, const std::string& to_address, double amount)
    : from_address(from_address), to_address(to_address), amount(amount) {
}

// Getters
std::string Transaction::get_from_address() const {
    return from_address;
}

std::string Transaction::get_to_address() const {
    return to_address;
}

double Transaction::get_amount() const {
    return amount;
}

std::string Transaction::get_signature() const {
    return signature;
}

// Sign the transaction
void Transaction::sign_transaction(RSA* private_key) {
    std::string data = to_string();
    signature = Crypto::sign(data, private_key);
}

// Check if the transaction is valid
bool Transaction::is_valid() const {
    if (from_address.empty()) {
        return true; // Miner reward transaction
    }
    return Crypto::verify(to_string(), signature, from_address);
}

// Convert transaction to string for hashing/signing
std::string Transaction::to_string() const {
    std::stringstream ss;
    ss << from_address << to_address << amount;
    return ss.str();
}
