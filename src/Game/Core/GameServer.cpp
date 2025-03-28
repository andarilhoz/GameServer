﻿#include "GameServer.h"

#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>

#include "../../Utils/Logger.h"
#include "../../Utils/Time.h"

#include "../Networking/Messages/MessageHandler.h"

GameServer::GameServer() :
    movementHandler(gameState),
    collisionSystem(gameState),
    foodController(gameState, mapController, collisionSystem),
    playerSystem(gameState, collisionSystem),
    gameLoop(gameState, movementHandler, playerSystem, connectionManager, foodController, collisionSystem)
{
    connectionManager.subscribeTcpMessage(
        [this](std::string message, std::shared_ptr<tcp::socket> socket) { 
            this->processTcpMessage(message, socket); 
        }
    );

    connectionManager.subscribeUdpMessage(
        [this](std::shared_ptr<std::string> message, udp::endpoint connection) { 
            this->processUdpMessage(message, connection); 
        }
    );

    connectionManager.subscribePlayerUdpDisconnected(
        [this](udp::endpoint connection) {
            this->playerDisconnected(connection);
        }
    );
}

void GameServer::run() {
    Logger::info("Run");
    connectionManager.run();
}

void GameServer::removePlayerFromGame(int playerId) {
    gameState.removePlayer(playerId);
    connectionManager.removePlayer(playerId);

    connectionManager.broadcastTcpMessage(MessageHandler::serializeDisconectedPlayer(playerId));
}


void GameServer::playerDisconnected(boost::asio::ip::udp::endpoint connection) {
    int playerId = connectionManager.getPlayerIdByUdpConnection(connection);
    removePlayerFromGame(playerId);
}

// Observables / EventListeners
void GameServer::processTcpMessage(std::string message, std::shared_ptr<boost::asio::ip::tcp::socket> connection) {
    try {
        int playerId = connectionManager.getPlayerIdByTcpConnection(connection);
        if (playerId != -1)
            return;

        json request = json::parse(message);

        if (!request.contains("nickname")) {
            Logger::error("❌ Mensagem sem 'nickname'!");
            return;
        }

        std::string nickname = request["nickname"];
        Player newPlayer = playerSystem.addPlayer(nickname);
        
        connectionManager.addTcpConnection(newPlayer.getId(), connection);
        Logger::info("New player connected: {}, id: {}", newPlayer.getNickname(), newPlayer.getId());
        

        connectionManager.sendTcpMessage(newPlayer.getId(), MessageHandler::serializeGameStateData(newPlayer.getId(), gameState));
        connectionManager.broadcastTcpMessage(MessageHandler::serializeNewPlayer(newPlayer));

        Logger::info("✅ Novo jogador conectado! Nickname: {}, ID: {}, Posição: ({}, {})",
            nickname, newPlayer.getId(), newPlayer.getX(), newPlayer.getY());
    }
    catch (const std::exception& e) {
        Logger::error("❌ Erro ao processar mensagem do cliente: {}", e.what());
    }
}

void GameServer::processUdpMessage(std::shared_ptr<std::string> message, udp::endpoint connection) {
    json parsedData = json::parse(*message);

    std::string type = parsedData["type"];

    if (type == "move") {
        handleMovementCall(parsedData, connection);
    }
    if (type == "ping") {
        auto timestamp = parsedData["timestamp"];

        GameMessage pongMessage = MessageHandler::serializePong(timestamp);
        connectionManager.sendUdpMessage(connection, pongMessage);
    }
}

void GameServer::handleMovementCall(json parsedData, udp::endpoint connection) {

    int playerId = parsedData["playerId"];
    float x = parsedData["x"];
    float y = parsedData["y"];

    if (!connectionManager.getTcpConnection(playerId)) {
        return;
    }

    movementHandler.handleDirectionChange(playerId, x, y);

    connectionManager.addUdpConnection(playerId, connection);
}