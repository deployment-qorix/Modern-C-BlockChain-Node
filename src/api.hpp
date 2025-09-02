#pragma once

#include "Telemetry/TelemetryHandler.hpp"

#include <boost/asio.hpp>
#include <thread>
#include <memory>
#include <iostream>

class Api {
public:
    Api(int port, TelemetryHandler* telemetry);
    ~Api();                         // ✅ Ensure proper cleanup

    void start();
    void stop();

private:
    void do_accept();

    int _port;
    TelemetryHandler* _telemetry;
    boost::asio::io_context _io;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> _acceptor;
    std::thread _serverThread;
    bool _running{false};           // ✅ Initialize to false
};
