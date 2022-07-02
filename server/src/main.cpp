#include <iostream>
#include "Server.h"
#include <thread>
#include <ctime>
#include <chrono>

inline std::time_t getTimeNow() {
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

int main(int argc, char *argv[]) {
    auto server = std::make_unique<tcp_server>();
    auto start_time = getTimeNow();
    server->start();
    std::cout << "server started " << std::ctime(&start_time) << std::endl;
    server->wait();
}
