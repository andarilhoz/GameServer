#include "TcpServer.h"
#include <nlohmann/json.hpp>
#include "../GameServer.h"
#include <random>
#include "../Utils/Logger.h" // Ajuste o caminho do Logger, se necessário

using json = nlohmann::json;
namespace {
    // Constantes de mapa/posicionamento
    constexpr float PLAYER_HALF_SIZE = 64.0f;
    constexpr float MAP_SIZE = 5000.0f;
}

TcpServer::TcpServer(boost::asio::io_context& io_context, int port, GameServer& gameServer)
    : acceptor(io_context, tcp::endpoint(tcp::v4(), port))
    , port(port)
    , gameServer(gameServer)
{
    // Inicia o processo de aceitar conexões
    startAccept();
}

void TcpServer::startAccept()
{
    // Cria um socket compartilhado
    auto socket = std::make_shared<tcp::socket>(acceptor.get_executor());

    // Aceita conexão de forma assíncrona
    acceptor.async_accept(
        *socket,
        [this, socket](const boost::system::error_code& error)
        {
            handleAccept(socket, error);
        }
    );
}

/**
 * Callback que lida com uma nova conexão aceita.
 */
void TcpServer::handleAccept(std::shared_ptr<tcp::socket> socket,
    const boost::system::error_code& error)
{
    if (!error) {
        Logger::info("Novo cliente conectado!");

        // Inicia a leitura de mensagens deste cliente
        processClientMessage(socket);

        // Marca o socket como conectado, ainda sem ID (define -1 temporariamente)
        connectedClients[socket] = -1;
    }
    else {
        Logger::error("Erro de conexão: {}", error.message());
    }

    // Continua aceitando novas conexões
    startAccept();
}

/**
 * Lê mensagens de um cliente de forma assíncrona.
 * Ao receber dados, processa e continua lendo.
 */
void TcpServer::processClientMessage(std::shared_ptr<tcp::socket> socket)
{
    // Buffer compartilhado para leitura
    auto buffer = std::make_shared<std::array<char, 1024>>();

    socket->async_read_some(
        boost::asio::buffer(*buffer),
        [this, socket, buffer](const boost::system::error_code& error, std::size_t bytes_transferred)
        {
            if (!error && bytes_transferred > 0) {
                // Converte o buffer lido para string
                std::string message(buffer->data(), bytes_transferred);
                Logger::info("📩 Mensagem recebida do cliente: {}", message);

                // Processa a mensagem como JSON
                try {
                    json request = json::parse(message);

                    // Verifica se contém "nickname"
                    if (!request.contains("nickname")) {
                        Logger::error("❌ Mensagem sem 'nickname'!");
                        // Continuamos lendo, pois pode ser uma mensagem inválida
                        processClientMessage(socket);
                        return;
                    }

                    // Gera ID e posição inicial
                    std::string nickname = request["nickname"];
                    int playerId = generatePlayerId();
                    float startX = generateRandomPosition(MAP_SIZE, PLAYER_HALF_SIZE);
                    float startY = generateRandomPosition(MAP_SIZE, PLAYER_HALF_SIZE);

                    // Cria o jogador e adiciona no GameServer
                    Player newPlayer(playerId, nickname, startX, startY, 1 /*size*/, 0 /*direction*/);
                    gameServer.addPlayer(newPlayer);

                    // Associa o playerId ao socket
                    connectedClients[socket] = playerId;
                    playersConnected.insert(playerId);

                    // Prepara JSON de resposta
                    json response = {
                        {"type",     "connect"},
                        {"playerId", playerId},
                        {"nickname", nickname},
                        {"x",        startX},
                        {"y",        startY}
                    };

                    // Envia de forma assíncrona
                    auto responseMessage = std::make_shared<std::string>(response.dump());
                    boost::asio::async_write(
                        *socket,
                        boost::asio::buffer(*responseMessage),
                        [socket, responseMessage](const boost::system::error_code& write_error, std::size_t /*bytes*/)
                        {
                            if (write_error) {
                                Logger::error("❌ Erro ao enviar resposta para o cliente: {}",
                                    write_error.message());
                            }
                            // Se necessário, poderia continuar algo aqui após o envio.
                        }
                    );

                    Logger::info("✅ Novo jogador conectado! Nickname: {}, ID: {}, Posição: ({}, {})",
                        nickname, playerId, startX, startY);
                }
                catch (const std::exception& e) {
                    Logger::error("❌ Erro ao processar mensagem do cliente: {}", e.what());
                }

                // Continua ouvindo novas mensagens deste mesmo cliente
                processClientMessage(socket);

            }
            else {
                if (error) {
                    Logger::error("❌ Erro ao receber mensagem TCP: {}", error.message());
                }
                else {
                    Logger::info("Conexão encerrada pelo cliente.");
                }
                // Saindo deste lambda não chamamos novamente processClientMessage; 
                // o socket não lerá mais nada.
            }
        }
    );
}

/**
 * Gera uma posição aleatória dentro dos limites do mapa.
 */
float TcpServer::generateRandomPosition(float mapSize, float playerHalfSize)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(playerHalfSize, mapSize - playerHalfSize);
    return dist(gen);
}

/**
 * Gera um ID único para cada novo jogador.
 */
int TcpServer::generatePlayerId()
{
    static int nextId = 1;
    return nextId++;
}

/**
 * Notifica todos os sockets de que um jogador se desconectou,
 * remove-o de connectedClients e do conjunto playersConnected.
 */
void TcpServer::broadcastPlayerDisconnection(int playerId)
{
    Logger::warn("Desconectando player");
    // Descobre quais sockets precisam ser notificados (todos menos o que tinha o playerId)
    std::vector<std::shared_ptr<tcp::socket>> clientsToNotify;
    for (const auto& [socket, id] : connectedClients) {
        if (id != playerId) {
            clientsToNotify.push_back(socket);
        }
    }

    // Monta a mensagem JSON
    json disconnectMsg = {
        {"type", "player_disconnect"},
        {"playerId", playerId}
    };
    auto messagePtr = std::make_shared<std::string>(disconnectMsg.dump());

    // Envia de forma assíncrona para cada cliente
    for (auto& socket : clientsToNotify) {
        boost::asio::async_write(
            *socket,
            boost::asio::buffer(*messagePtr),
            [messagePtr](const boost::system::error_code& error, std::size_t /*bytes*/)
            {
                if (error) {
                    Logger::error("❌ Erro ao enviar mensagem de desconexão: {}", error.message());
                }
            }
        );
    }

    // Remove o jogador dos mapas internos
    for (auto it = connectedClients.begin(); it != connectedClients.end();) {
        if (it->second == playerId) {
            it = connectedClients.erase(it);
        }
        else {
            ++it;
        }
    }
    // Também remove do conjunto
    playersConnected.erase(playerId);

    Logger::info("Jogador {} desconectado e broadcast enviado", playerId);
}

/**
 * Retorna true se o playerId estiver no conjunto de jogadores conectados.
 */
bool TcpServer::isPlayerConnected(int playerId)
{
    return (playersConnected.find(playerId) != playersConnected.end());
}
