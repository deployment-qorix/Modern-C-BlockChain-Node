#ifndef PEERSESSION_H
#define PEERSESSION_H

#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "../Mempool/Mempool.h"

using namespace boost::asio;
using namespace boost::asio::ip;

class PeerSession : public std::enable_shared_from_this<PeerSession> {
public:
    PeerSession(tcp::socket&& socket, Mempool& mempool);
    ~PeerSession();

    void start();
    
    // Moved from private to public
    void start_write(const std::string& message);

private:
    void start_read();
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error, size_t bytes_transferred);
    
    tcp::socket socket_;
    Mempool& mempool_;
    boost::asio::streambuf read_buffer_; // Changed to streambuf for async_read_until
};

#endif // PEERSESSION_H