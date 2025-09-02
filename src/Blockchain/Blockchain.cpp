#include "Blockchain.h"
#include <iostream>
#include <ctime>

// Blockchain constructor
Blockchain::Blockchain(Mempool* mempool) : difficulty_(4), mempool_(mempool) {
    // Create the genesis block
    Transaction genesis_tx("", "genesis_address", 0);
    Block genesis_block(0, "0", {genesis_tx});
    addBlock(genesis_block);
}

void Blockchain::minePendingTransactions() {
    // Create a new block with pending transactions
    Block newBlock(chain.size(), chain.back().get_hash(), mempool_->getTransactions());
    
    // Mine the block
    newBlock.mineBlock(difficulty_);
    
    // Add the new block to the chain
    addBlock(newBlock);
    
    // Clear the mempool
    mempool_->clear();
}

bool Blockchain::isChainValid() {
    for (size_t i = 1; i < chain.size(); ++i) {
        const Block& currentBlock = chain[i];
        const Block& previousBlock = chain[i-1];

        // Check if current block hash is valid
        if (currentBlock.get_hash() != currentBlock.calculate_hash()) {
            return false;
        }

        // Check if previous block hash is correct
        if (currentBlock.get_previous_hash() != previousBlock.get_hash()) {
            return false;
        }
    }
    return true;
}

void Blockchain::addBlock(Block& newBlock) {
    chain.push_back(newBlock);
}

void Blockchain::viewChain() {
    for (const auto& block : chain) {
        std::cout << "Block Index: " << block.get_index() << std::endl;
        std::cout << "Previous Hash: " << block.get_previous_hash() << std::endl;
        std::cout << "Current Hash: " << block.get_hash() << std::endl;
        std::cout << "Nonce: " << block.get_nonce() << std::endl;
        std::cout << "Transactions: " << std::endl;
        for (const auto& tx : block.get_transactions()) {
            std::cout << "  - From: " << tx.get_from_address() << std::endl;
            std::cout << "  - To: " << tx.get_to_address() << std::endl;
            std::cout << "  - Amount: " << tx.get_amount() << std::endl;
            std::cout << "  - Signature: " << tx.get_signature() << std::endl;
        }
        std::cout << std::endl;
    }
}

// THIS IS THE NEW FUNCTION THAT RETURNS THE BLOCKCHAIN
const std::vector<Block>& Blockchain::get_chain() const {
    return chain;
}
