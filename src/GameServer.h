#pragma once

#include "Server/UdpServer.h"
#include "Server/TcpServer.h"
#include "Game/GameState.h"
#include "Game/MovementHandler.h"
#include <boost/asio.hpp>
#include "Game/Player.h"
#include "Server/ClientUdpMessage.h"

#include <thread>
#include <atomic>
#include <mutex>

class GameServer {
public:
	GameServer(int tcpPort, int updPort);
	void run();
	void processPlayerMovement(ClientUdpMessage message);
	void addPlayer(Player player);
	void removePlayer(int playerId);
	void removePlayerFromEverything(int playerId);
	void stop();
	void updateAllPlayers();
	bool isPlayerTcpConnected(int playerId);
	std::string getAllPlayersInfo();

private:

	void updatePlayerPositionFromDirection();
	void broadcastPlayersPositions();
	void checkForDisconnectedPlayers();

	boost::asio::io_context io_context;
	TcpServer tcpServer;
	std::unique_ptr<UdpServer> udpServer;
	GameState gameState;
	MovementHandler movementHandler;
	std::atomic<bool> running = true;
	std::thread updateThread;
};