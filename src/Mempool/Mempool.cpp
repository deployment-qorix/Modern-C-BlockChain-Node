#include "Mempool.h"
#include <iostream>

void Mempool::addTransaction(const Transaction& tx) {
    if (!tx.is_valid()) {
        std::cout << "Rejected invalid transaction." << std::endl;
        return;
    }
    _transactions.push_back(tx);
}

std::vector<Transaction> Mempool::getTransactions() const {
    return _transactions;
}

bool Mempool::isEmpty() const {
    return _transactions.empty();
}

void Mempool::clear() {
    _transactions.clear();
}
