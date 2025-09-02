#include "PeerClient.h"
#include <iostream>
#include <boost/bind/bind.hpp>
#include <sstream>

// small helper for trimming
static inline void rtrim(std::string &s) {
    while (!s.empty() && (s.back() == '\r' || s.back() == ' ' || s.back() == '\t'))
        s.pop_back();
}

PeerClient::PeerClient(io_context& io_context, Mempool& mempool)
    : io_context_(io_context), mempool_(mempool), resolver_(io_context) {}

void PeerClient::connect(const std::string& host, const std::string& port) {
    auto socket = std::make_shared<tcp::socket>(io_context_);
    resolver_.async_resolve(host, port,
        boost::bind(&PeerClient::handle_resolve, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::iterator,
            socket));
}

void PeerClient::handle_resolve(const boost::system::error_code& error,
                                tcp::resolver::iterator endpoint_iterator,
                                std::shared_ptr<tcp::socket> socket) {
    if (!error) {
        boost::asio::async_connect(*socket, endpoint_iterator,
            boost::bind(&PeerClient::handle_connect, this,
                boost::asio::placeholders::error,
                socket));
    } else {
        std::cerr << "Resolve error: " << error.message() << std::endl;
    }
}

void PeerClient::handle_connect(const boost::system::error_code& error,
                                std::shared_ptr<tcp::socket> socket) {
    if (!error) {
        std::cout << "Successfully connected to peer at "
                  << socket->remote_endpoint() << std::endl;

        socket_ = socket; // save raw socket for reads/writes

        // immediately send handshake so server knows we are alive
        send("handshake client1");

        // start listening for server messages
        start_read();
    } else {
        std::cerr << "Connection error: " << error.message() << std::endl;
    }
}

void PeerClient::start_read() {
    boost::asio::async_read_until(*socket_, read_buffer_, '\n',
        boost::bind(&PeerClient::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void PeerClient::handle_read(const boost::system::error_code& error, std::size_t) {
    if (!error) {
        std::string line;
        {
            std::istream is(&read_buffer_);
            std::getline(is, line);
        }
        rtrim(line);

        if (!line.empty())
            std::cout << "[server] " << line << std::endl;

        // keep reading
        start_read();
    } else if (error == boost::asio::error::eof) {
        std::cout << "Disconnected from server." << std::endl;
    } else {
        std::cerr << "Read error: " << error.message() << std::endl;
    }
}

void PeerClient::send(const std::string& msg) {
    if (!socket_) {
        std::cerr << "Not connected.\n";
        return;
    }
    auto data = std::make_shared<std::string>(msg + "\n");
    boost::asio::async_write(*socket_, boost::asio::buffer(*data),
        [data](const boost::system::error_code& ec, std::size_t) {
            if (ec) {
                std::cerr << "Send error: " << ec.message() << std::endl;
            }
        });
}

// Interactive shell
void PeerClient::run_shell() {
    std::string command;
    while (std::getline(std::cin, command)) {
        if (!command.empty())
            send(command);
    }
}
