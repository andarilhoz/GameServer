#pragma once

#include "../Server/UdpServer.h"
#include "../Server/TcpServer.h"
#include "GameState.h"
#include "MovementHandler.h"
#include "FoodController.h"
#include "Player.h"
#include "MapController.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <boost/asio.hpp>

class GameServer {
public:
	GameServer(int tcpPort, int updPort);
	void run();
	void addPlayer(Player player);
	void removePlayerFromGame(int playerId);
	void gameLoop();
	std::string getAllPlayersInfo();

	void processTcpMessage(std::string message, int playerId);
	void processUdpMessage(std::shared_ptr<std::string> message, boost::asio::ip::udp::endpoint connection);

private:

	void updatePlayerPositionFromDirection();
	void broadcastPlayersStatus();
	void checkForDisconnectedPlayers();
	void checkForFoodCollisions();
	void spawnFood();

	std::string generateDisconnectPlayerMessage(int playerId);
	std::string generateConnectedPlayerMessage(Player player);

	boost::asio::io_context io_context;
	TcpServer tcpServer;
	UdpServer udpServer;
	GameState gameState;
	FoodController foodController;
	MovementHandler movementHandler;
	MapController mapController;
	std::atomic<bool> running = true;
	std::thread updateThread;
};