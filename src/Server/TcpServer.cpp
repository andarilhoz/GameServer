#include "TcpServer.h"

#include <random>
#include <nlohmann/json.hpp>

#include "../Game/GameServer.h"
#include "../Utils/Logger.h"

using json = nlohmann::json;

TcpServer::TcpServer(boost::asio::io_context& io_context, int port, GameServer& gameServer)
    : acceptor(io_context, tcp::endpoint(tcp::v4(), port))
    , port(port)
    , gameServer(gameServer)
{
    startAccept();
}

void TcpServer::startAccept()
{
    auto socket = std::make_shared<tcp::socket>(acceptor.get_executor());

    acceptor.async_accept(
        *socket,
        [this, socket](const boost::system::error_code& error)
        {
            handleAccept(socket, error);
        }
    );
}

void TcpServer::handleAccept(std::shared_ptr<tcp::socket> socket,
    const boost::system::error_code& error)
{
    if (!error) {
        connectedClients[socket] = generatePlayerId();
        playersConnected[connectedClients[socket]] = socket;
        Logger::info("Novo cliente conectado!, assumindo id: {}", connectedClients[socket]);
        processClientMessage(socket, connectedClients[socket]);
    }
    else {
        Logger::error("Erro de conexão: {}", error.message());
    }
    startAccept();
}

void TcpServer::processClientMessage(std::shared_ptr<tcp::socket> socket, int playerId)
{
    auto buffer = std::make_shared<std::array<char, 1024>>();

    socket->async_read_some(
        boost::asio::buffer(*buffer),
        [this, socket, playerId, buffer](const boost::system::error_code& error, std::size_t bytes_transferred)
        {
            if (!error && bytes_transferred > 0) {
                std::string message(buffer->data(), bytes_transferred);
                Logger::info("📩 Mensagem recebida do cliente: {}, message: {}", playerId, message);

                gameServer.processTcpMessage(message, playerId);
                processClientMessage(socket, playerId);
            }
            else {
                if (error) {
                    Logger::error("❌ Erro ao receber mensagem TCP: {}", error.message());
                }
                else {
                    Logger::info("Conexão encerrada pelo cliente.");
                }
            }
        }
    );
}

int TcpServer::generatePlayerId()
{
    static int nextId = 1;
    return nextId++;
}

void TcpServer::disconectPlayer(int playerId) {
    connectedClients.erase(playersConnected[playerId]);
    playersConnected.erase(playerId);
}


bool TcpServer::isPlayerConnected(int playerId)
{
    return (playersConnected.find(playerId) != playersConnected.end());
}


void TcpServer::broadcastMessage(std::shared_ptr<std::string> message)
{
    for (auto& [socket, id] : connectedClients) {
        boost::asio::async_write(
            *socket,
            boost::asio::buffer(*message),
            [message](const boost::system::error_code& error, std::size_t bytes)
            {
                if (error) {
                    Logger::error("❌ Erro ao enviar mensagem de broadcast: {}", error.message());
                }
            }
        );
    }
}

void TcpServer::sendPlayerMessage(int playerId, std::shared_ptr<std::string> message) {
    auto& socket = playersConnected[playerId];

    boost::asio::async_write(
        *socket,
        boost::asio::buffer(*message),
        [socket, message](const boost::system::error_code& write_error, std::size_t bytes)
        {
            if (write_error) {
                Logger::error("❌ Erro ao enviar resposta para o cliente: {}",
                    write_error.message());
            }
        }
    );
}