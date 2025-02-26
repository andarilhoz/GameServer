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
	void disconectPlayer(int playerId);
	void broadcastMessage(std::shared_ptr<std::string> message);
	void sendPlayerMessage(int playerId, std::shared_ptr<std::string> message);
	bool isPlayerConnected(int playerId);

private:
	void startAccept();
	void handleAccept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code& error);
	void processClientMessage(std::shared_ptr<tcp::socket> socket, int playerId);
	int generatePlayerId();

	std::unordered_map<std::shared_ptr<tcp::socket>, int> connectedClients;
	std::unordered_map<int, std::shared_ptr<tcp::socket>> playersConnected;

	tcp::acceptor acceptor;
	int port;
	GameServer& gameServer;
};