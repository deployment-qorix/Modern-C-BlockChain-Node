#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include "../Mempool/Mempool.h"

using boost::asio::ip::tcp;
using boost::asio::io_context;

class PeerClient {
public:
    PeerClient(io_context& io_context, Mempool& mempool);

    void connect(const std::string& host, const std::string& port);
    void run_shell();                 // interactive shell
    void send(const std::string& msg); // send a message to server

private:
    // async steps
    void handle_resolve(const boost::system::error_code& error,
                        tcp::resolver::iterator endpoint_iterator,
                        std::shared_ptr<tcp::socket> socket);

    void handle_connect(const boost::system::error_code& error,
                        std::shared_ptr<tcp::socket> socket);

    void start_read();
    void handle_read(const boost::system::error_code& error, std::size_t bytes);

private:
    io_context& io_context_;
    Mempool& mempool_;
    tcp::resolver resolver_;

    std::shared_ptr<tcp::socket> socket_;   // <— was missing
    boost::asio::streambuf read_buffer_;    // <— was missing
};
