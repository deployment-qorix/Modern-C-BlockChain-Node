#include "P2PServer.h"
#include "PeerSession.h" // The crucial include
#include <iostream>
#include <boost/bind/bind.hpp>
#include <memory>
#include <list>

P2PServer::P2PServer(io_context& io_context, tcp::endpoint& endpoint, Mempool& mempool)
    : acceptor_(io_context, endpoint), mempool_(mempool) {
    std::cout << "P2P Server started on " << endpoint.address() << ":" << endpoint.port() << std::endl;
    start_accept();
}

void P2PServer::start_accept() {
    // 1. Create a socket that will be used for the new connection.
    auto socket = std::make_shared<tcp::socket>(static_cast<boost::asio::io_context&>(acceptor_.get_executor().context()));
    
    // 2. Asynchronously wait for a new connection on this socket.
    // The handler will be called with the connected socket.
    acceptor_.async_accept(*socket,
        boost::bind(&P2PServer::handle_accept, this, socket,
            boost::asio::placeholders::error));
}

void P2PServer::handle_accept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code& error) {
    if (!error) {
        // 3. Once a connection is accepted, we create the PeerSession with the new socket.
        // This is the correct place to create a shared_ptr for a PeerSession.
        auto new_session = std::make_shared<PeerSession>(std::move(*socket), mempool_);
        sessions_.push_back(new_session);
        new_session->start();
    } else {
        std::cerr << "Accept error: " << error.message() << std::endl;
    }
    
    // Continue accepting new connections
    start_accept();
}
