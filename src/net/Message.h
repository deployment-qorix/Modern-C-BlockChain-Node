#pragma once
#include <string>
#include <vector>

enum MessageType {
    TRANSACTION_BROADCAST,
    BLOCK_BROADCAST,
    BLOCKCHAIN_REQUEST,
    BLOCKCHAIN_RESPONSE
};

struct Message {
    MessageType type;
    std::string payload; // JSON or serialized data
};