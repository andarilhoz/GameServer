#pragma once

#include <boost/asio.hpp>
#include <unordered_map>
#include <memory>
#include <set>
#include <iostream>
#include "../Game/Player.h"
#include "../Utils/Logger.h"

class GameServer;

using boost::asio::ip::tcp;

class TcpServer {
public:
	TcpServer(boost::asio::io_context& io_context, int port, GameServer& gameServer);
	int getPort() const { return port; }
	void broadcastPlayerDisconnection(int playerId);
	bool isPlayerConnected(int playerId);

private:
	void startAccept();
	void handleAccept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code& error);
	void processClientMessage(std::shared_ptr<tcp::socket> socket);
	int generatePlayerId();
	float generateRandomPosition(float mapSize, float playerHalfSize);

	std::unordered_map<std::shared_ptr<tcp::socket>, int> connectedClients;
	std::set<int> playersConnected;
	tcp::acceptor acceptor;
	int port;
	GameServer& gameServer;
};