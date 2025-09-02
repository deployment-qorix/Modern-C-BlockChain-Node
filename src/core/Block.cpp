#include "Block.h"
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <limits>

// Block constructor
Block::Block(int index, const std::string& previous_hash, const std::vector<Transaction>& transactions)
    : index(index), previous_hash(previous_hash), transactions(transactions), nonce(0) {
    timestamp = std::time(nullptr);
    hash = calculate_hash();
}

// Getters
int Block::get_index() const {
    return index;
}

std::string Block::get_previous_hash() const {
    return previous_hash;
}

std::string Block::get_hash() const {
    return hash;
}

std::time_t Block::get_timestamp() const {
    return timestamp;
}

int Block::get_nonce() const {
    return nonce;
}

std::vector<Transaction> Block::get_transactions() const {
    return transactions;
}

// Calculate the block's hash
std::string Block::calculate_hash() const {
    std::stringstream ss;
    ss << index << previous_hash << timestamp << nonce;
    for (const auto& tx : transactions) {
        ss << tx.to_string();
    }
    std::string data = ss.str();
    
    unsigned char hash_bytes[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.length(), hash_bytes);
    
    std::stringstream hash_ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        hash_ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash_bytes[i]);
    }
    
    return hash_ss.str();
}

// Mine the block
void Block::mineBlock(int difficulty) {
    std::string prefix(difficulty, '0');
    while (hash.substr(0, difficulty) != prefix) {
        nonce++;
        hash = calculate_hash();
        if (nonce == std::numeric_limits<int>::max()) {
            std::cerr << "Nonce limit reached. Could not mine block." << std::endl;
            return;
        }
    }
    std::cout << "Block mined: " << hash << std::endl;
}
