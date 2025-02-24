#pragma once

#include <set>
#include <boost/asio.hpp>
#include "../Game/Player.h"

class GameServer;

using boost::asio::ip::udp;

class UdpServer {
public:
	UdpServer(boost::asio::io_context& io_context, int port, GameServer& gameServer);
	int getPort() const { return port; }
	void broadcastMessage(const std::string& message);
	void removeClient(int playerId);

private:
	void startReceive();
	void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred);
	void broadcastGameState();


	udp::socket socket;
	udp::endpoint remote_endpoint;
	std::array<char, 1024> recv_buffer;
	std::set<boost::asio::ip::udp::endpoint> connectedClients;
	std::unordered_map<boost::asio::ip::udp::endpoint, int> players;
	std::unordered_map<int, boost::asio::ip::udp::endpoint> playersEndpoints;
	int port;

	GameServer& gameServer;
};
