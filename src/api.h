#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "../Blockchain/Blockchain.h"
#include <memory>

class Api {
public:
    Api(boost::asio::io_context& ioc, Blockchain* blockchain);
    ~Api();

    void start();
    void stop();

private:
    void do_accept();
    void on_accept(boost::system::error_code ec, boost::asio::ip::tcp::socket socket);

    boost::asio::io_context& ioc_;
    boost::asio::ip::tcp::acceptor acceptor_;
    Blockchain* blockchain_;
};
