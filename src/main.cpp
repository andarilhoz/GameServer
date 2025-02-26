#include <iostream>

#include "Game/GameServer.h"
#include "Utils/Logger.h"

const int TCP_PORT = 5555;
const int UDP_PORT = 7777;

int main()
{
    Logger::setLogLevel(LogLevel::INFO);

    Logger::info("Main initializing");

    try {
        GameServer server(TCP_PORT, UDP_PORT);
        server.run();
    }
    catch (std::exception& e) {
        Logger::error("Erro: {}", e.what());
    }

    return 0;
}