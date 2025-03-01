#include <iostream>

#include "Game/Core/GameServer.h"
#include "Utils/Logger.h"


int main()
{
    Logger::setLogLevel(LogLevel::INFO);

    Logger::info("Main initializing");

    try {
        GameServer server;
        server.run();
    }
    catch (std::exception& e) {
        Logger::error("Erro: {}", e.what());
    }

    return 0;
}