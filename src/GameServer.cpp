#include "GameServer.h"
#include "Server/ClientUdpMessage.h"
#include "Utils/Logger.h"

// Se você usar JSON via nlohmann, por exemplo:
#include <nlohmann/json.hpp>
// using json = nlohmann::json;

#include <chrono>
#include <thread>
#include <algorithm>
#include <cmath>

static constexpr float PLAYER_HALF_SIZE = 64.0f;
static constexpr float MAP_SIZE = 5000.0f;
static constexpr float MAX_SPEED = 200.0f;
static constexpr std::chrono::milliseconds UPDATE_INTERVAL(50);

GameServer::GameServer(int tcpPort, int udpPort)
    : tcpServer(io_context, tcpPort, *this),
    movementHandler(gameState),
    running(true)
{
    // Cria o servidor UDP
    udpServer = std::make_unique<UdpServer>(io_context, udpPort, *this);

    Logger::info("Iniciando thread de atualização dos jogadores...");

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
    // Encaminha para a lógica de movimento
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
 * Loop principal de atualização de todos os jogadores.
 * - Executa até que 'running' seja false.
 * - Atualiza as posições de cada jogador com base na direção.
 * - Envia posições a todos via broadcast UDP.
 * - Verifica se há jogadores desconectados.
 */
void GameServer::updateAllPlayers()
{
    using namespace std::chrono;

    while (running) {
        {
            // 1) Atualiza posição de cada jogador
            updatePlayerPositionFromDirection();

            // 2) Envia broadcast com as posições atualizadas
            broadcastPlayersPositions();

            // 3) Remove jogadores inativos
            checkForDisconnectedPlayers();
        }

        // Espera um tempo fixo antes de continuar (50 ms)
        std::this_thread::sleep_for(UPDATE_INTERVAL);
    }
}

/**
 * Atualiza a posição de cada jogador de acordo com a direção.
 * O delta time é obtido de dentro do 'gameState'.
 */
void GameServer::updatePlayerPositionFromDirection()
{
    float delta = gameState.getDeltaTime();

    // Percorre todos os jogadores do estado de jogo
    auto& allPlayers = gameState.getAllPlayers();
    for (auto& [id, player] : allPlayers) {

        float dx = player.getDirectionX();
        float dy = player.getDirectionY();

        // Normaliza a direção para que a velocidade seja constante
        float length = std::sqrt(dx * dx + dy * dy);
        if (length > 0.0f) {
            dx /= length;
            dy /= length;
        }

        // Calcula nova posição
        float newX = player.getX() + dx * MAX_SPEED * delta;
        float newY = player.getY() + dy * MAX_SPEED * delta;

        // Restringe aos limites do mapa
        newX = std::clamp(newX, PLAYER_HALF_SIZE, MAP_SIZE - PLAYER_HALF_SIZE);
        newY = std::clamp(newY, PLAYER_HALF_SIZE, MAP_SIZE - PLAYER_HALF_SIZE);

        // Define no objeto player
        player.setPosition(newX, newY);

        // Log para depuração
        Logger::info("Jogador {} movido para ({}, {})", id, player.getX(), player.getY());
    }
}

/**
 * Monta um JSON com as informações de todos os jogadores
 * e solicita que o servidor UDP envie via broadcast.
 */
void GameServer::broadcastPlayersPositions()
{
    // Gera a string com os dados de todos os jogadores
    std::string message = getAllPlayersInfo();

    // Envia para todos (atenção para a implementação de broadcastMessage!)
    // Ela deve copiar internamente 'message', caso seja assíncrona.
    udpServer->broadcastMessage(message);
}

/**
 * Retorna uma string JSON com as informações de todos os jogadores.
 * Importante: a chamada deve ocorrer enquanto o 'gameStateMutex' estiver travado.
 * Caso contrário, poderemos ter race condition ao iterar sobre 'gameState'.
 */
std::string GameServer::getAllPlayersInfo()
{
    // Não travamos o mutex aqui porque assumimos que quem chamar
    // esta função (como broadcastPlayersPositions) já está dentro
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
 * Verifica se algum jogador está inativo há muito tempo (timeout)
 * e remove do estado caso esteja.
 */
void GameServer::checkForDisconnectedPlayers()
{
    constexpr float TIMEOUT = 25.0f;
    std::vector<int> playersToRemove;

    // Vamos iterar sobre uma cópia das chaves para não invalidar iteradores ao remover
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