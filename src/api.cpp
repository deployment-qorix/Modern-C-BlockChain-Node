#include "api.h"
#include <iostream>
#include <sstream>
#include <boost/asio/strand.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/bind/bind.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

// Simple placeholder to handle HTTP requests
void handle_request(http::request<http::string_body>&& req, http::response<http::string_body>& res, Blockchain* blockchain) {
    if (req.method() == http::verb::get && req.target() == "/blockchain/view") {
        std::stringstream ss;
        ss << "Blockchain:\n";
        for (const auto& block : blockchain->get_chain()) {
            ss << "Block Hash: " << block.get_hash() << "\n";
            ss << "  - Previous Hash: " << block.get_previous_hash() << "\n";
            ss << "  - Timestamp: " << block.get_timestamp() << "\n";
            ss << "  - Nonce: " << block.get_nonce() << "\n";
            ss << "  - Transactions:\n";
            for (const auto& tx : block.get_transactions()) {
                ss << "    - From: " << tx.get_from_address() << "\n";
                ss << "    - To: " << tx.get_to_address() << "\n";
                ss << "    - Amount: " << tx.get_amount() << "\n";
                ss << "    - Signature: " << tx.get_signature() << "\n";
            }
            ss << "\n";
        }
        std::string chain_data = ss.str();
        res.body() = chain_data;
        res.set(http::field::content_type, "text/plain");
        res.result(http::status::ok);
    } else {
        res.result(http::status::not_found);
        res.body() = "Not Found\n";
    }
    res.set(http::field::server, "Blockchain-API");
    res.prepare_payload();
}

// Api class implementation
Api::Api(net::io_context& ioc, Blockchain* blockchain)
    : ioc_(ioc), acceptor_(net::make_strand(ioc)), blockchain_(blockchain) {
    
    // Set up the acceptor to listen on port 8080
    boost::system::error_code ec;
    tcp::endpoint endpoint{tcp::v4(), 8080};
    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
        std::cerr << "Open error: " << ec.message() << std::endl;
        return;
    }
    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    if (ec) {
        std::cerr << "Set option error: " << ec.message() << std::endl;
        return;
    }
    acceptor_.bind(endpoint, ec);
    if (ec) {
        std::cerr << "Bind error: " << ec.message() << std::endl;
        return;
    }
    acceptor_.listen(net::socket_base::max_listen_connections, ec);
    if (ec) {
        std::cerr << "Listen error: " << ec.message() << std::endl;
        return;
    }
}

Api::~Api() {
    stop();
}

void Api::start() {
    do_accept();
    std::cout << "API server started on port 8080." << std::endl;
}

void Api::stop() {
    boost::system::error_code ec;
    acceptor_.close(ec);
}

void Api::do_accept() {
    acceptor_.async_accept(
        net::make_strand(ioc_),
        beast::bind_front_handler(
            &Api::on_accept,
            this));
}

// Session handler that manages a single connection
void Api::on_accept(boost::system::error_code ec, tcp::socket socket) {
    if (!ec) {
        // Create a new session for the accepted socket
        auto session = std::make_shared<HttpSession>(std::move(socket), blockchain_);
        session->start();
    } else {
        // Handle a connection error
        if (ec != net::error::operation_aborted) {
            std::cerr << "Accept error: " << ec.message() << std::endl;
        }
    }
    // Continue accepting new connections
    if (ec != net::error::operation_aborted) {
        do_accept();
    }
}

// New HttpSession class to handle each connection
class HttpSession : public std::enable_shared_from_this<HttpSession> {
public:
    HttpSession(tcp::socket&& socket, Blockchain* blockchain)
        : stream_(std::move(socket)), blockchain_(blockchain) {}

    void start() {
        do_read();
    }

private:
    void do_read() {
        auto self = shared_from_this();
        http::async_read(stream_, buffer_, request_,
            beast::bind_front_handler(
                &HttpSession::on_read,
                self));
    }

    void on_read(beast::error_code ec, std::size_t bytes_transferred) {
        if (ec == http::error::end_of_stream) {
            return do_close();
        }
        if (ec) {
            std::cerr << "Read error: " << ec.message() << std::endl;
            return;
        }

        handle_request(std::move(request_), response_, blockchain_);

        bool keep_alive = response_.keep_alive();
        auto self = shared_from_this();
        http::async_write(stream_, response_,
            beast::bind_front_handler(
                &HttpSession::on_write,
                self, keep_alive));
    }

    void on_write(bool keep_alive, beast::error_code ec, std::size_t bytes_transferred) {
        if (ec) {
            std::cerr << "Write error: " << ec.message() << std::endl;
            return;
        }

        if (!keep_alive) {
            return do_close();
        }

        request_ = {};
        do_read();
    }

    void do_close() {
        beast::error_code ec;
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
    }

    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
    Blockchain* blockchain_;
};
