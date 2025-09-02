#include <iostream>
#include <thread>
#include <chrono>

// Include the header file for the Api class, not the .cpp file.
#include "api.h"

#include "Telemetry/TelemetryHandler.hpp"
#include "Wallet/Wallet.h"

int main(int argc, char* argv[]) {
    std::cout << "🚗 Vehicle Health Monitor & Remote Diagnostic Dashboard" << std::endl;
    std::cout << "🔧 Starting backend server..." << std::endl;

    try {
        // Initialize Telemetry handler
        TelemetryHandler telemetry("data/telemetry_sample.csv");
        telemetry.loadData();

        // Start API (REST + WebSocket)
        Api server(8080, &telemetry);
        server.start();

        // Example Wallet
        Wallet wallet;
        std::cout << "✅ Wallet Public Key: " << wallet.getPublicKey() << std::endl;

        // Keep CLI alive
        while (true) {
            std::cout << "\n(CLI) Commands:\n";
            std::cout << "1. status   - Show latest telemetry\n";
            std::cout << "2. exit     - Stop server\n";
            std::cout << "> ";

            std::string cmd;
            std::cin >> cmd;

            if (cmd == "status") {
                telemetry.printLatest();
            } else if (cmd == "exit") {
                std::cout << "⏹ Stopping server..." << std::endl;
                server.stop();
                break;
            } else {
                std::cout << "❌ Unknown command." << std::endl;
            }
        }

        server.stop();

    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "👋 Server stopped. Goodbye!" << std::endl;
    return 0;
}
