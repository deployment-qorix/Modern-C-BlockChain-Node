cmake_minimum_required(VERSION 3.10)
project(ModernBlockchain)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Boost components (Asio is header-only)
find_package(Boost 1.83.0 COMPONENTS system thread REQUIRED)
find_package(OpenSSL REQUIRED)

# Include all header directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/Blockchain
    ${CMAKE_SOURCE_DIR}/src/Mempool
    ${CMAKE_SOURCE_DIR}/src/net
    ${CMAKE_SOURCE_DIR}/src/crypto
    ${CMAKE_SOURCE_DIR}/src/Wallet
    ${CMAKE_SOURCE_DIR}/src/Telemetry
    ${CMAKE_SOURCE_DIR}/libs
)

# List all source files explicitly
set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/api.cpp
    
    ${CMAKE_SOURCE_DIR}/src/core/Block.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Transaction.cpp
    
    ${CMAKE_SOURCE_DIR}/src/Blockchain/Blockchain.cpp
    
    ${CMAKE_SOURCE_DIR}/src/Mempool/Mempool.cpp
    
    ${CMAKE_SOURCE_DIR}/src/net/P2PServer.cpp
    ${CMAKE_SOURCE_DIR}/src/net/PeerSession.cpp
    ${CMAKE_SOURCE_DIR}/src/net/PeerClient.cpp
    
    ${CMAKE_SOURCE_DIR}/src/crypto/Utils.cpp
    
    ${CMAKE_SOURCE_DIR}/src/Wallet/Wallet.cpp
    
    ${CMAKE_SOURCE_DIR}/src/Telemetry/TelemetryHandler.cpp
)

add_executable blockchainadd_executable(blockchain
    src/main.cpp
    src/net/PeerClient.cpp
    src/net/P2PServer.cpp
    src/net/PeerSession.cpp
    src/api.cpp
    # ... other files
)

# Link libraries
target_link_libraries(blockchain PRIVATE Boost::system Boost::thread OpenSSL::Crypto OpenSSL::SSL)
