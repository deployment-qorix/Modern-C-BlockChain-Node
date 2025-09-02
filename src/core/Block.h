#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "Transaction.h"

class Block {
public:
    // Block constructor
    Block(int index, const std::string& previous_hash, const std::vector<Transaction>& transactions);

    // Getters for Block data
    int get_index() const;
    std::string get_previous_hash() const;
    std::string get_hash() const;
    std::time_t get_timestamp() const;
    int get_nonce() const;
    std::vector<Transaction> get_transactions() const;

    // Methods
    std::string calculate_hash() const;
    void mineBlock(int difficulty);

private:
    int index;
    std::string previous_hash;
    std::string hash;
    std::time_t timestamp;
    int nonce;
    std::vector<Transaction> transactions;
};
