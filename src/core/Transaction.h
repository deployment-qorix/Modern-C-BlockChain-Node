#pragma once

#include <string>
#include <openssl/rsa.h>

class Transaction {
public:
    Transaction(const std::string& from_address, const std::string& to_address, double amount);
    
    // Getters for transaction data
    std::string get_from_address() const;
    std::string get_to_address() const;
    double get_amount() const;
    std::string get_signature() const;

    // Methods
    void sign_transaction(RSA* private_key);
    bool is_valid() const;
    std::string to_string() const;

private:
    std::string from_address;
    std::string to_address;
    double amount;
    std::string signature;
};
