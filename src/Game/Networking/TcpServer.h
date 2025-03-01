#pragma once

#include <boost/asio.hpp>
#include <unordered_map>
#include <memory>
#include <set>
#include <iostream>
#include "../Entities/Player.h"
#include "../../Utils/Logger.h"

class GameServer;

using boost::asio::ip::tcp;

class TcpServer {
public:
	TcpServer(boost::asio::io_context& io_context, int port);
	int getPort() const { return port; }
	void sendPlayerMessage(std::shared_ptr<tcp::socket> socket, std::shared_ptr<std::string> message);
	void subscribe(std::function<void(std::string, std::shared_ptr<tcp::socket>)> callback);

private:
	void startAccept();
	void handleAccept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code& error);
	void processClientMessage(std::shared_ptr<tcp::socket> socket);
	
	
	std::function<void(std::string, std::shared_ptr<tcp::socket>)> observable = nullptr;
	tcp::acceptor acceptor;
	int port;
};