#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <list>
#include "PeerSession.h" // Includes PeerSession class

using namespace boost::asio;
using namespace boost::asio::ip;

class P2PServer {
public:
    P2PServer(io_context& io_context, tcp::endpoint& endpoint, Mempool& mempool);
    void start_accept();

private:
    void handle_accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& error);

    tcp::acceptor acceptor_;
    Mempool& mempool_;
    std::list<std::shared_ptr<PeerSession>> sessions_;
};
