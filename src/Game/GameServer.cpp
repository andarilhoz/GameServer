#include "GameServer.h"

#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>

#include "../Utils/Logger.h"

constexpr float DISCONECT_TIMEOUT = 25.0f;
static constexpr std::chrono::milliseconds UPDATE_INTERVAL(50);
constexpr float PLAYER_HALF_SIZE = 64.0f;

const int MinFoodAvailable = 5;
const int PLAYER_MAX_SIZE = 2240;

GameServer::GameServer(int tcpPort, int udpPort)
    : tcpServer(io_context, tcpPort, *this),
    udpServer(io_context, udpPort, *this),
    movementHandler(gameState),
    mapController(),
    foodController(50, gameState, mapController),
    running(true)
{
    Logger::info("Initializing loop thread");
    updateThread = std::thread(&GameServer::gameLoop, this);
}

void GameServer::run()
{
    Logger::info("Servidor iniciado nas portas TCP: {} e UDP: {}",
        tcpServer.getPort(), udpServer.getPort());
    io_context.run();
}

void GameServer::gameLoop()
{
    using namespace std::chrono;

    foodController.generateFood(100);

    while (running) {
        {
            updatePlayerPositionFromDirection();
            broadcastPlayersStatus();
            checkForDisconnectedPlayers();
            checkForFoodCollisions();
            spawnFood();
        }

        std::this_thread::sleep_for(UPDATE_INTERVAL);
    }
}


// PlayersControllers
void GameServer::addPlayer(Player player)
{
    auto connectedMessage = std::make_shared<std::string>(generateConnectedPlayerMessage(player));
    Logger::info("New player connected: {}", player.getNickname());
    tcpServer.broadcastMessage(connectedMessage);
    gameState.addPlayer(player);
}

void GameServer::removePlayerFromGame(int playerId) {
    gameState.removePlayer(playerId);
    udpServer.removeClient(std::make_shared<int>(playerId));
    tcpServer.disconectPlayer(playerId);

    auto disconnectMessage = std::make_shared<std::string>(generateDisconnectPlayerMessage(playerId));
    tcpServer.broadcastMessage(disconnectMessage);
}


// LoopActions

void GameServer::updatePlayerPositionFromDirection()
{
    float deltaTime = gameState.getDeltaTime();

    auto& allPlayers = gameState.getAllPlayers();
    for (auto& [id, player] : allPlayers) {
        movementHandler.updatePosition(player, deltaTime);
        Logger::debug("Jogador {} movido para ({}, {})", id, player.getX(), player.getY());
    }
}

void GameServer::broadcastPlayersStatus()
{
    std::string message = getAllPlayersInfo();
    udpServer.broadcastMessage(std::make_shared<std::string>(message));
}

void GameServer::checkForDisconnectedPlayers()
{
    std::vector<int> playersToRemove;

    auto& allPlayers = gameState.getAllPlayers();
    for (const auto& [id, player] : allPlayers) {
        if (gameState.isPlayerInactive(id, DISCONECT_TIMEOUT)) {
            playersToRemove.push_back(id);
        }
    }

    for (int playerId : playersToRemove) {
        removePlayerFromGame(playerId);
    }
}

void GameServer::checkForFoodCollisions() 
{
    auto& allPlayers = gameState.getAllPlayers();
    for (auto& [id, player] : allPlayers) {
        
        int foodId = foodController.checkFoodCollision(player.getX(), player.getY(), player.getSize());
        if (foodId < 0) {
            continue;
        }
        else {
            foodController.removeFood(foodId);

            if (player.getSize() < PLAYER_MAX_SIZE) {
                player.increaseSize(1);
            }
            
            
            player.addPoints(1);

            Logger::info("Jogador {} comeu comida {}, tamanho: {}", player.getNickname(), foodId, player.getSize());
            
            json foodEatenMessage = {
                {"type", "food_eaten"},
                {"id", foodId},
                {"playerId", player.getId()},
            };

            auto foodMessage = std::make_shared<std::string>(foodEatenMessage.dump());
            tcpServer.broadcastMessage(foodMessage);
        }
    }
}

void GameServer::spawnFood() {
    int foodAmount = gameState.getFoodList().size();
    
    if (foodAmount >= MinFoodAvailable) {
        return;
    }

    std::vector<Food> newFoods = foodController.generateFood(MinFoodAvailable);
    json foodGeneratedMessage = {
        {"type", "food_spawn"}
    };

    for (auto& food : newFoods) {
        foodGeneratedMessage["foods"].push_back({
            {"id", food.id},
            {"x", food.x},
            {"y", food.y}
        });
    }

    auto foodMessage = std::make_shared<std::string>(foodGeneratedMessage.dump());
    tcpServer.broadcastMessage(foodMessage);
}



// Observables / EventListeners
void GameServer::processTcpMessage(std::string message, int playerId) {
    try {
        if (gameState.isPlayerAdded(playerId))
            return;

        json request = json::parse(message);

        if (!request.contains("nickname")) {
            Logger::error("❌ Mensagem sem 'nickname'!");
            return;
        }


        std::string nickname = request["nickname"];
        float startX = mapController.generateRandomPosition(PLAYER_HALF_SIZE);
        float startY = mapController.generateRandomPosition(PLAYER_HALF_SIZE);
        
        Player newPlayer(playerId, nickname, startX, startY, 1, 0, 64);
        addPlayer(newPlayer);
        
        auto allPlayers = gameState.getAllPlayers();


        json response = {
            {"type", "connect"},
            {"id",  playerId},
            {"foods", foodController.getFoodInfo()}
        };

        for (auto& [id, player] : allPlayers) {
            response["players"].push_back({
                {"id",       id},
                {"nickname", player.getNickname()},
                {"x",        player.getX()},
                {"y",        player.getY()},
                {"size",     player.getSize()},
                {"points",   player.getPoints()}
            });
        }


        auto responseMessage = std::make_shared<std::string>(response.dump());
        tcpServer.sendPlayerMessage(playerId, responseMessage);

        Logger::info("✅ Novo jogador conectado! Nickname: {}, ID: {}, Posição: ({}, {})",
            nickname, playerId, startX, startY);
    }
    catch (const std::exception& e) {
        Logger::error("❌ Erro ao processar mensagem do cliente: {}", e.what());
    }
}

void GameServer::processUdpMessage(std::shared_ptr<std::string> message, boost::asio::ip::udp::endpoint connection) {
    json parsedData = json::parse(*message);

    int playerId = parsedData["playerId"];
    float x = parsedData["x"];
    float y = parsedData["y"];

    if (!tcpServer.isPlayerConnected(playerId)) {
        return;
    }

    movementHandler.handleDirectionChange(playerId, x, y);

    udpServer.addConnection(playerId, connection);
}


// Messages
std::string GameServer::getAllPlayersInfo()
{
    json playerData;
    auto allPlayers = gameState.getAllPlayers();

    for (auto& [id, player] : allPlayers) {
        playerData["players"].push_back({
            {"id",       id},
            {"nickname", player.getNickname()},
            {"x",        player.getX()},
            {"y",        player.getY()}
            });
    }

    return playerData.dump();
}

std::string GameServer::generateDisconnectPlayerMessage(int playerId) {
    json disconnectMsg = {
        {"type", "player_disconnect"},
        {"id", playerId}
    };
    return disconnectMsg.dump();
}

std::string GameServer::generateConnectedPlayerMessage(Player player) {
    json connectedMsg = {
        {"type", "player_connect"},
        {"id", player.getId()},
        {"x", player.getX()},
        {"y", player.getY()},
        {"nickname", player.getNickname()},
    };

    return connectedMsg.dump();
}
