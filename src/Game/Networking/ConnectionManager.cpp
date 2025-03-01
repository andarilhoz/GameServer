#include "ConnectionManager.h"

#include "../../Utils/Logger.h"
#include "../Config/GameConfig.h"


ConnectionManager::ConnectionManager() :
    tcpServer(io_context, GameConfig::TCP_PORT), 
    udpServer(io_context, GameConfig::UDP_PORT)
{
};

void ConnectionManager::run() {
    Logger::info("Servidor iniciado nas portas TCP: {} e UDP: {}",
        tcpServer.getPort(), udpServer.getPort());
    io_context.run();
}

void ConnectionManager::addTcpConnection(int playerId, std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
    tcpConnections[playerId] = socket;
    tcpConnectionsReverse[socket] = playerId;
}

void ConnectionManager::addUdpConnection(int playerId, boost::asio::ip::udp::endpoint endpoint) {
    udpConnections[playerId] = endpoint;
    udpConnectionsReverse[endpoint] = playerId;
}

void ConnectionManager::removePlayer(int playerId) {
    if (tcpConnections.count(playerId)) {
        tcpConnectionsReverse.erase(tcpConnections[playerId]);
        tcpConnections.erase(playerId);
    }
    if (udpConnections.count(playerId)) {
        udpConnectionsReverse.erase(udpConnections[playerId]);
        udpConnections.erase(playerId);
    }
}

std::shared_ptr<boost::asio::ip::tcp::socket> ConnectionManager::getTcpConnection(int playerId) {
    return tcpConnections.count(playerId) ? tcpConnections[playerId] : nullptr;
}

boost::asio::ip::udp::endpoint ConnectionManager::getUdpConnection(int playerId) {
    return udpConnections.count(playerId) ? udpConnections[playerId] : boost::asio::ip::udp::endpoint();
}

int ConnectionManager::getPlayerIdByTcpConnection(std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
    return tcpConnectionsReverse.count(socket) ? tcpConnectionsReverse[socket] : -1;
}

int ConnectionManager::getPlayerIdByUdpConnection(boost::asio::ip::udp::endpoint endpoint) {
    return udpConnectionsReverse.count(endpoint) ? udpConnectionsReverse[endpoint] : -1;
}

std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> ConnectionManager::getAllTcpConnections() {
    std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> connections;
    for (const auto& [_, socket] : tcpConnections) {
        connections.push_back(socket);
    }
    return connections;
}

std::vector<boost::asio::ip::udp::endpoint> ConnectionManager::getAllUdpConnections() {
    std::vector<boost::asio::ip::udp::endpoint> connections;
    for (const auto& [_, endpoint] : udpConnections) {
        connections.push_back(endpoint);
    }
    return connections;
}


void ConnectionManager::sendTcpMessage(int playerId, const GameMessage& message) {
    if (tcpConnections.count(playerId)) {
        auto connection = tcpConnections[playerId];
        auto messageShared = std::make_shared<std::string>(message.getRawData());
        tcpServer.sendPlayerMessage(connection, messageShared);
    }
    else {
        Logger::warn("Tentativa de enviar mensagem TCP para player {} que não está conectado!", playerId);
    }
}

void ConnectionManager::sendUdpMessage(int playerId, const GameMessage& message) {
    if (udpConnections.count(playerId)) {
        auto connection = udpConnections[playerId];
        auto messageShared = std::make_shared<std::string>(message.getRawData());
        udpServer.sendMessage(connection, messageShared, 
            [this, playerId](boost::asio::ip::udp::endpoint connection) {
                //removePlayer(playerId);
        });
    }
    else {
        Logger::warn("Tentativa de enviar mensagem UDP para player {} que não está conectado!", playerId);
    }
}

void ConnectionManager::broadcastTcpMessage(const GameMessage& message) {
    for (const auto& [playerId, socket] : tcpConnections) {
        sendTcpMessage(playerId, message);
    }
}

void ConnectionManager::broadcastUdpMessage(const GameMessage& message) {
    for (const auto& [playerId, endpoint] : udpConnections) {
        sendUdpMessage(playerId, message);
    }
}

void ConnectionManager::subscribeTcpMessage(std::function<void(std::string, std::shared_ptr<boost::asio::ip::tcp::socket>)> observable) {
    tcpServer.subscribe(observable);
}

void ConnectionManager::subscribeUdpMessage(std::function<void(std::shared_ptr<std::string>, udp::endpoint)> observable) {
    udpServer.subscribe(observable);
}

void ConnectionManager::subscribePlayerUdpDisconnected(std::function<void(udp::endpoint)> observable) {
    udpServer.subscribeDisconnect(observable);
}