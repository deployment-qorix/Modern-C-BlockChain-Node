#pragma once
#include <vector>
#include "../core/Transaction.h"

class Mempool {
public:
    void addTransaction(const Transaction& tx);
    std::vector<Transaction> getTransactions() const;
    bool isEmpty() const;
    void clear();

private:
    std::vector<Transaction> _transactions;
};
