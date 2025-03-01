#pragma once

#include <boost/asio.hpp>
#include "TcpServer.h"
#include "UdpServer.h"

#include "Messages/GameMessage.h"


class ConnectionManager {
public:
    ConnectionManager();
    void run();

    void addTcpConnection(int playerId, std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    void addUdpConnection(int playerId, boost::asio::ip::udp::endpoint endpoint);

    void removePlayer(int playerId);

    std::shared_ptr<boost::asio::ip::tcp::socket> getTcpConnection(int playerId);
    boost::asio::ip::udp::endpoint getUdpConnection(int playerId);

    int getPlayerIdByTcpConnection(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    int getPlayerIdByUdpConnection(boost::asio::ip::udp::endpoint endpoint);

    std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> getAllTcpConnections();
    std::vector<boost::asio::ip::udp::endpoint> getAllUdpConnections();

    void sendTcpMessage(int playerId, const GameMessage& message);
    void sendUdpMessage(int playerId, const GameMessage& message);

    void broadcastTcpMessage(const GameMessage& message);
    void broadcastUdpMessage(const GameMessage& message);

    void subscribeTcpMessage(std::function<void(std::string, std::shared_ptr<boost::asio::ip::tcp::socket>)> observable);
    void subscribeUdpMessage(std::function<void(std::shared_ptr<std::string>, udp::endpoint)> observable);
    
    
    void subscribePlayerUdpDisconnected(std::function<void(udp::endpoint)> observable);

private:

    std::unordered_map<int, std::shared_ptr<boost::asio::ip::tcp::socket>> tcpConnections;
    std::unordered_map<int, boost::asio::ip::udp::endpoint> udpConnections;

    std::unordered_map<std::shared_ptr<boost::asio::ip::tcp::socket>, int> tcpConnectionsReverse;
    std::unordered_map<boost::asio::ip::udp::endpoint, int> udpConnectionsReverse;
    
    boost::asio::io_context io_context;
    TcpServer tcpServer;
    UdpServer udpServer;
};