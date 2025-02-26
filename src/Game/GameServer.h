#pragma once

#include "../Server/UdpServer.h"
#include "../Server/TcpServer.h"
#include "GameState.h"
#include "MovementHandler.h"
#include <boost/asio.hpp>
#include "Player.h"
#include "../Server/ClientUdpMessage.h"

#include <thread>
#include <atomic>
#include <mutex>

class GameServer {
public:
	GameServer(int tcpPort, int updPort);
	void run();
	void processPlayerDirection(ClientUdpMessage message);
	void addPlayer(Player player);
	void removePlayer(int playerId);
	void removePlayerFromGame(int playerId);
	void updateAllPlayers();
	bool isPlayerTcpConnected(int playerId);
	std::string getAllPlayersInfo();

private:

	void updatePlayerPositionFromDirection();
	void broadcastPlayersStatus();
	void checkForDisconnectedPlayers();

	boost::asio::io_context io_context;
	TcpServer tcpServer;
	UdpServer udpServer;
	GameState gameState;
	MovementHandler movementHandler;
	std::atomic<bool> running = true;
	std::thread updateThread;
};