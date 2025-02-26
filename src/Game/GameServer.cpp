#include "GameServer.h"

#include <chrono>
#include <thread>
#include <algorithm>
#include <cmath>
#include <random>
#include <nlohmann/json.hpp>

#include "../Server/ClientUdpMessage.h"
#include "../Utils/Logger.h"

static constexpr float PLAYER_HALF_SIZE = 64.0f;
static constexpr float MAP_SIZE = 5000.0f;
static constexpr float MAX_SPEED = 200.0f;
static constexpr std::chrono::milliseconds UPDATE_INTERVAL(50);

const int GRID_CELL_SIZE = 32;
const int FOOD_SIZE = 16;
const int FOOD_COUNT = 50;

GameServer::GameServer(int tcpPort, int udpPort)
    : tcpServer(io_context, tcpPort, *this),
    movementHandler(gameState),
    running(true)
{
    // Cria o servidor UDP
    udpServer = std::make_unique<UdpServer>(io_context, udpPort, *this);

    Logger::info("Iniciando thread de atualiza��o dos jogadores...");

    // Inicia a thread de update (loop principal do jogo)
    updateThread = std::thread(&GameServer::updateAllPlayers, this);
}

void GameServer::run()
{
    Logger::info("Servidor iniciado nas portas TCP: {} e UDP: {}",
        tcpServer.getPort(), udpServer->getPort());
    io_context.run(); // Aqui o IO service (boost::asio) entra no loop de eventos.
}

void GameServer::stop()
{
    running = false;
    if (updateThread.joinable()) {
        updateThread.join();
    }
}

void GameServer::processPlayerMovement(ClientUdpMessage movementData)
{
    // Encaminha para a l�gica de movimento
    movementHandler.handleMovement(movementData);
}

void GameServer::addPlayer(Player player)
{
    gameState.addPlayer(player);
}

void GameServer::removePlayer(int playerId)
{
    gameState.removePlayer(playerId);
}

bool GameServer::isPlayerTcpConnected(int playerId)
{
    return tcpServer.isPlayerConnected(playerId);
}

/**
 * Loop principal de atualiza��o de todos os jogadores.
 * - Executa at� que 'running' seja false.
 * - Atualiza as posi��es de cada jogador com base na dire��o.
 * - Envia posi��es a todos via broadcast UDP.
 * - Verifica se h� jogadores desconectados.
 */
void GameServer::updateAllPlayers()
{
    using namespace std::chrono;

    generateFood();

    while (running) {
        {
            // 1) Atualiza posi��o de cada jogador
            updatePlayerPositionFromDirection();

            // 2) Envia broadcast com as posi��es atualizadas
            broadcastPlayersPositions();

            // 3) Remove jogadores inativos
            checkForDisconnectedPlayers();
        }

        // Espera um tempo fixo antes de continuar (50 ms)
        std::this_thread::sleep_for(UPDATE_INTERVAL);
    }
}

/**
 * Atualiza a posi��o de cada jogador de acordo com a dire��o.
 * O delta time � obtido de dentro do 'gameState'.
 */
void GameServer::updatePlayerPositionFromDirection()
{
    float delta = gameState.getDeltaTime();

    // Percorre todos os jogadores do estado de jogo
    auto& allPlayers = gameState.getAllPlayers();
    for (auto& [id, player] : allPlayers) {

        float dx = player.getDirectionX();
        float dy = player.getDirectionY();

        // Normaliza a dire��o para que a velocidade seja constante
        float length = std::sqrt(dx * dx + dy * dy);
        if (length > 0.0f) {
            dx /= length;
            dy /= length;
        }

        // Calcula nova posi��o
        float newX = player.getX() + dx * MAX_SPEED * delta;
        float newY = player.getY() + dy * MAX_SPEED * delta;

        // Restringe aos limites do mapa
        newX = std::clamp(newX, PLAYER_HALF_SIZE, MAP_SIZE - PLAYER_HALF_SIZE);
        newY = std::clamp(newY, PLAYER_HALF_SIZE, MAP_SIZE - PLAYER_HALF_SIZE);

        // Define no objeto player
        player.setPosition(newX, newY);

        // Log para depura��o
        Logger::info("Jogador {} movido para ({}, {})", id, player.getX(), player.getY());
    }
}

/**
 * Monta um JSON com as informa��es de todos os jogadores
 * e solicita que o servidor UDP envie via broadcast.
 */
void GameServer::broadcastPlayersPositions()
{
    // Gera a string com os dados de todos os jogadores
    std::string message = getAllPlayersInfo();

    // Envia para todos (aten��o para a implementa��o de broadcastMessage!)
    // Ela deve copiar internamente 'message', caso seja ass�ncrona.
    udpServer->broadcastMessage(message);
}

/**
 * Retorna uma string JSON com as informa��es de todos os jogadores.
 * Importante: a chamada deve ocorrer enquanto o 'gameStateMutex' estiver travado.
 * Caso contr�rio, poderemos ter race condition ao iterar sobre 'gameState'.
 */
std::string GameServer::getAllPlayersInfo()
{
    // N�o travamos o mutex aqui porque assumimos que quem chamar
    // esta fun��o (como broadcastPlayersPositions) j� est� dentro
    // de um std::lock_guard<std::mutex> gameStateMutex.

    json playerData;
    auto& allPlayers = gameState.getAllPlayers();

    for (auto& [id, player] : allPlayers) {
        // Monta o objeto JSON para cada jogador
        playerData["players"].push_back({
            {"id",       id},
            {"nickname", player.getNickname()},
            {"x",        player.getX()},
            {"y",        player.getY()},
            {"size",     player.getSize()}
            });
    }

    // Converte para string JSON
    return playerData.dump();
}

/**
 * Verifica se algum jogador est� inativo h� muito tempo (timeout)
 * e remove do estado caso esteja.
 */
void GameServer::checkForDisconnectedPlayers()
{
    constexpr float TIMEOUT = 25.0f;
    std::vector<int> playersToRemove;

    // Vamos iterar sobre uma c�pia das chaves para n�o invalidar iteradores ao remover
    auto& allPlayers = gameState.getAllPlayers();
    for (const auto& [id, player] : allPlayers) {
        if (gameState.isPlayerInactive(id, TIMEOUT)) {
            playersToRemove.push_back(id);
        }
    }

    // Agora removemos de fato
    for (int playerId : playersToRemove) {
        removePlayerFromEverything(playerId);
    }
}

void GameServer::removePlayerFromEverything(int playerId) {
    gameState.removePlayer(playerId);
    udpServer->removeClient(playerId);
    tcpServer.broadcastPlayerDisconnection(playerId);
}


void GameServer::generateFood() {

    while (gameState.getFoodList().size() < FOOD_COUNT) {
        float x = generateRandomPosition();
        float y = generateRandomPosition();

        if (isValidFoodPosition(x, y)) {
            int foodId = generateFoodId();
            gameState.spawnFood(foodId, x, y);
        }
    }

    //sendFoodToClients();
}


float GameServer::generateRandomPosition() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(FOOD_SIZE, MAP_SIZE - FOOD_SIZE);
    return dist(gen);
}

int GameServer::generateFoodId() {
    static int nextFoodId = 1000;  // Evita conflito com IDs de players
    return nextFoodId++;
}


struct GridCell {
    int x, y;
    bool operator == (const GridCell& other) const {
        return x == other.x && y == other.y;
    }
};

namespace std {
    template<> struct hash<GridCell> {
        size_t operator()(const GridCell& cell) const {
            return hash<int>()(cell.x) ^ hash<int>()(cell.y);
        }
    };
}

std::unordered_map<GridCell, std::vector<int>> foodGrid;

GridCell getGridCell(float x, float y) {
    return { int(x / GRID_CELL_SIZE), int(y / GRID_CELL_SIZE) };
}

bool GameServer::isValidFoodPosition(float x, float y) {
    GridCell cell = getGridCell(x, y);

    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            GridCell neightbor = { cell.x + dx, cell.y + dy };
            if (foodGrid.count(neightbor)) {
                for (int foodId : foodGrid[neightbor]) {
                    if (std::hypot(gameState.getFood(foodId).x - x, gameState.getFood(foodId).y - y) < FOOD_SIZE) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

void GameServer::addFood(int foodId, float x, float y) {
    gameState.spawnFood(foodId, x, y);
    GridCell cell = getGridCell(x, y);
    foodGrid[cell].push_back(foodId);
}


void GameServer::sendFoodToClients() {
    json foodMessage;
    foodMessage["type"] = "food_spawn";

    for (const auto& food : gameState.getFoodList()) {
        foodMessage["foods"].push_back({
            {"id", food.second.id},
            {"x", food.second.x},
            {"y", food.second.y}
            });
    }

    std::string message = foodMessage.dump();
    tcpServer.broadcastMessage(message);
}

json GameServer::getFoodInfo() {
    json foodMessage;

    for (const auto& food : gameState.getFoodList()) {
        foodMessage["foods"].push_back({
            {"id", food.second.id},
            {"x", food.second.x},
            {"y", food.second.y}
            });
    }

    return foodMessage;
}