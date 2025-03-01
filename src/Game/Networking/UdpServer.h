#pragma once

#include <set>
#include <boost/asio.hpp>
#include "../Entities/Player.h"

class GameServer;

using boost::asio::ip::udp;

class UdpServer {
public:
	UdpServer(boost::asio::io_context& io_context, int port);
	int getPort() const { return port; }
	void sendMessage(boost::asio::ip::udp::endpoint connection, std::shared_ptr<std::string> message, std::function<void(boost::asio::ip::udp::endpoint)> onClientError);
	void subscribe(std::function<void(std::shared_ptr<std::string>, udp::endpoint)> callback);
	void subscribeDisconnect(std::function<void(udp::endpoint)> callback);

private:
	void startReceive();
	void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred);

	int port;

	udp::socket socket;
	udp::endpoint remote_endpoint;
	std::function<void(std::shared_ptr<std::string>, udp::endpoint)> observable;
	std::function<void(udp::endpoint)> disconnectObservable;
	std::array<char, 1024> recv_buffer;
	boost::asio::io_context& io_context;
};
