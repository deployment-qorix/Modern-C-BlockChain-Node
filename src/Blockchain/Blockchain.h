#pragma once

#include <vector>
#include "../core/Block.h"
#include "../Mempool/Mempool.h"

class Blockchain {
public:
    Blockchain(Mempool* mempool);
    void minePendingTransactions();
    bool isChainValid();
    void addBlock(Block& newBlock);
    void viewChain();
    
    // THIS IS THE MISSING FUNCTION
    const std::vector<Block>& get_chain() const;

private:
    int difficulty_;
    std::vector<Block> chain;
    Mempool* mempool_;
};