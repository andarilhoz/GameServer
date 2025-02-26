#pragma once

#include "../Server/UdpServer.h"
#include "../Server/TcpServer.h"
#include "GameState.h"
#include "MovementHandler.h"
#include <boost/asio.hpp>
#include "Player.h"
#include "MapController.h"

#include <thread>
#include <atomic>
#include <mutex>

class GameServer {
public:
	GameServer(int tcpPort, int updPort);
	void run();
	void addPlayer(Player player);
	void removePlayer(int playerId);
	void removePlayerFromGame(int playerId);
	void updateAllPlayers();
	bool isPlayerTcpConnected(int playerId);
	std::string getAllPlayersInfo();

	void processTcpMessage(std::string message, int playerId);
	void processUdpMessage(std::shared_ptr<std::string> message, boost::asio::ip::udp::endpoint connection);

private:

	void updatePlayerPositionFromDirection();
	void broadcastPlayersStatus();
	void checkForDisconnectedPlayers();

	std::string generateDisconnectPlayerMessage(int playerId);
	std::string generateConnectedPlayerMessage(Player player);

	boost::asio::io_context io_context;
	TcpServer tcpServer;
	UdpServer udpServer;
	GameState gameState;
	MovementHandler movementHandler;
	MapController mapController;
	std::atomic<bool> running = true;
	std::thread updateThread;
};