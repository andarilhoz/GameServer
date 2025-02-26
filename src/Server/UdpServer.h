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
	void broadcastMessage(std::shared_ptr<std::string> message);
	void removeClient(std::shared_ptr<int> playerId);
	bool isPlayerConnected(int playerId);
	void addConnection(int playerId, boost::asio::ip::udp::endpoint);

private:
	void startReceive();
	void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred);

	int port;

	udp::socket socket;
	udp::endpoint remote_endpoint;

	std::array<char, 1024> recv_buffer;
	boost::asio::io_context& io_context;
	std::unordered_map<boost::asio::ip::udp::endpoint, int> playersConnections;
	std::unordered_map<int, boost::asio::ip::udp::endpoint> playersEndpoints;

	GameServer& gameServer;
};
