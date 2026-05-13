cmake_minimum_required(VERSION 3.16)
project(vVault CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

# Crow is header-only — just needs boost.asio
cmake_policy(SET CMP0167 OLD)
find_package(Boost REQUIRED COMPONENTS system)

add_definitions(-DCROW_USE_STANDALONE_ASIO)
add_definitions(-DASIO_STANDALONE)


add_executable(vvault_server main.cpp)

target_include_directories(vvault_server PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    C:/msys64/ucrt64/include
)

target_include_directories(vvault_server PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(vvault_server
    Threads::Threads
    ${Boost_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
    ws2_32        # ← add this
    mswsock       # ← add this
)

target_compile_options(vvault_server PRIVATE -O2 -Wall)
