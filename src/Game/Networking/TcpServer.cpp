#include "TcpServer.h"

#include <random>
#include <nlohmann/json.hpp>

#include "../Core/GameServer.h"
#include "../../Utils/Logger.h"

using json = nlohmann::json;

TcpServer::TcpServer(boost::asio::io_context& io_context, int port)
    : acceptor(io_context, tcp::endpoint(tcp::v4(), port))
    , port(port)
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
        Logger::info("Novo cliente conectado!");
        processClientMessage(socket);
    }
    else {
        Logger::error("Erro de conexão: {}", error.message());
    }
    startAccept();
}

void TcpServer::processClientMessage(std::shared_ptr<tcp::socket> socket)
{
    auto buffer = std::make_shared<std::array<char, 1024>>();

    socket->async_read_some(
        boost::asio::buffer(*buffer),
        [this, socket, buffer](const boost::system::error_code& error, std::size_t bytes_transferred)
        {
            if (!error && bytes_transferred > 0) {
                std::string message(buffer->data(), bytes_transferred);
                Logger::info("📩 Mensagem recebida do cliente: {}, message: {}", message);
                if (observable != nullptr) {
                    observable(message, socket);
                }
                processClientMessage(socket);
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


void TcpServer::sendPlayerMessage(std::shared_ptr<tcp::socket> socket, std::shared_ptr<std::string> message) {
    message->push_back('\n');
    boost::asio::async_write(*socket, boost::asio::buffer(*message),
        [message](const boost::system::error_code& error, std::size_t bytes) {
            if (error) {
                Logger::error("Erro ao enviar mensagem TCP {}", error.message());
            }
        }
    );
}

void TcpServer::subscribe(std::function<void(std::string, std::shared_ptr<tcp::socket>)> callback) {
    observable = callback;
}