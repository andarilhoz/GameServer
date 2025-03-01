#pragma once

#include "../Networking/UdpServer.h"
#include "../Networking/TcpServer.h"
#include "GameState.h"
#include "../Systems/MovementHandler.h"
#include "../Systems/FoodController.h"
#include "../Entities/Player.h"
#include "../Systems/MapController.h"
#include "../Systems/PlayerSystem.h"
#include "../Networking/ConnectionManager.h"
#include "GameLoop.h"

#include <thread>
#include <atomic>
#include <boost/asio.hpp>

class GameServer {
public:
	GameServer();
	void run();
	void removePlayerFromGame(int playerId);

	void processTcpMessage(std::string message, std::shared_ptr<boost::asio::ip::tcp::socket> connection);
	void processUdpMessage(std::shared_ptr<std::string> message, boost::asio::ip::udp::endpoint connection);

private:
	void playerDisconnected(boost::asio::ip::udp::endpoint connection);
	
	GameState gameState;
	GameLoop gameLoop;
	FoodController foodController;
	PlayerSystem playerSystem;
	ConnectionManager connectionManager;
	MovementHandler movementHandler;
	MapController mapController;
};