#include <iostream>

#include "Game/GameServer.h"
#include "Utils/Logger.h"

const int TCP_PORT = 5555;
const int UDP_PORT = 7777;

int main()
{
    Logger::setLogLevel(LogLevel::DEBUG);

    Logger::info("Main initializing");

    try {
        GameServer server(TCP_PORT, UDP_PORT);
        server.run();

        Logger::info("Enter para sair...");
        std::cin.get();
        server.stop();
    }
    catch (std::exception& e) {
        Logger::error("Erro: {}", e.what());
    }

    return 0;
}