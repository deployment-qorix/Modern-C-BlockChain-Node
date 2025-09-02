#include "PeerSession.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>              // std::strtod
#include <boost/bind/bind.hpp>

// The missing header
#include "../core/Transaction.h"

namespace {

inline void rtrim_cr_spaces(std::string& s) {
    while (!s.empty()) {
        char c = s.back();
        if (c == '\r' || c == ' ' || c == '\t') s.pop_back();
        else break;
    }
}

} // namespace

// PeerSession constructor
PeerSession::PeerSession(tcp::socket&& socket, Mempool& mempool)
    : socket_(std::move(socket)), mempool_(mempool) {}

PeerSession::~PeerSession() {
    if (socket_.is_open()) {
        boost::system::error_code ignored;
        socket_.shutdown(tcp::socket::shutdown_both, ignored);
        socket_.close(ignored);
    }
    std::cout << "Session destroyed." << std::endl;
}

void PeerSession::start() {
    try {
        std::cout << "Session started. Connected to " << socket_.remote_endpoint() << std::endl;
    } catch (...) {
        std::cout << "Session started. (endpoint unavailable)" << std::endl;
    }

    // Send a small banner/handshake so the client sees activity
    // (keeps your simple line protocol; no JSON required)
    start_write("hello from node");   // <-- handshake/banner

    start_read(); // begin read loop
}

void PeerSession::start_write(const std::string& message) {
    auto self = shared_from_this();
    // Append a single LF: each logical message is one line
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(message + "\n"),
        boost::bind(&PeerSession::handle_write,
                    self,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred)
    );
}

void PeerSession::handle_write(const boost::system::error_code& error, size_t) {
    if (error) {
        std::cerr << "Write error: " << error.message() << std::endl;
    }
}

void PeerSession::start_read() {
    auto self = shared_from_this();
    // Read until LF; any CR is trimmed below
    boost::asio::async_read_until(
        socket_,
        read_buffer_,
        '\n',
        boost::bind(&PeerSession::handle_read,
                    self,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred)
    );
}

void PeerSession::handle_read(const boost::system::error_code& error, size_t) {
    if (!error) {
        // Extract exactly one line (async_read_until guarantees at least one '\n')
        std::string line;
        {
            std::istream is(&read_buffer_);
            std::getline(is, line); // drops trailing '\n'
        }
        rtrim_cr_spaces(line);

        try {
            std::cout << "Received command from " << socket_.remote_endpoint() << ": " << line << std::endl;
        } catch (...) {
            std::cout << "Received command: " << line << std::endl;
        }

        // ------- simple command parsing --------
        std::stringstream ss(line);
        std::string command;
        ss >> command;

        if (command == "handshake") {
            // optional: "handshake <peer-id-or-key>"
            std::string peer_id;
            std::getline(ss, peer_id);
            if (!peer_id.empty() && peer_id.front() == ' ') peer_id.erase(peer_id.begin());
            start_write("handshake_ok");
        }
        else if (command == "ping") {
            start_write("pong");
        }
        else if (command == "create_transaction") {
            std::string to_address;
            std::string amount_str;
            ss >> to_address >> amount_str;

            if (to_address.empty() || amount_str.empty()) {
                start_write("Error: Bad transaction.");
            } else {
                rtrim_cr_spaces(amount_str);
                char* endp = nullptr;
                double amount = std::strtod(amount_str.c_str(), &endp);
                bool numeric_ok = (endp != nullptr && *endp == '\0');

                if (!numeric_ok || amount <= 0.0) {
                    start_write("Error: Invalid amount.");
                } else {
                    Transaction tx("Peer", to_address, amount);
                    mempool_.addTransaction(tx);
                    std::cout << "Transaction from peer added to mempool." << std::endl;
                    start_write("OK");
                }
            }
        }
        else if (command.empty()) {
            // ignore blank line
        }
        else {
            std::cout << "Unknown command: " << command << std::endl;
            start_write("Error: Unknown command.");
        }
        // ---------------------------------------

        // Keep listening; any extra bytes after delimiter remain buffered
        start_read();
    }
    else if (error == boost::asio::error::eof) {
        std::cout << "Peer disconnected gracefully." << std::endl;
    }
    else if (error == boost::asio::error::operation_aborted) {
        // socket closed from elsewhere; ignore
    }
    else {
        std::cerr << "Read error: " << error.message() << std::endl;
    }
}
