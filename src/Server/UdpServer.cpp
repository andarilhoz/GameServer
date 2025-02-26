#include "UdpServer.h"
#include <iostream>
#include "../Utils/Logger.h"
#include <nlohmann/json.hpp>
#include "../Game/GameServer.h"

UdpServer::UdpServer(boost::asio::io_context& io_context, int port, GameServer& gameServer)
	:socket(io_context, udp::endpoint(udp::v4(), port)), port(port), gameServer(gameServer), io_context(io_context) {
	startReceive();
}

void UdpServer::startReceive() {
	socket.async_receive_from(boost::asio::buffer(recv_buffer), remote_endpoint,
		[this](const boost::system::error_code& error, std::size_t bytes_transferred) {
			handle_receive(error, bytes_transferred);
		});
}

void UdpServer::handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred) {

	if (!error) {
		auto data = std::make_shared<std::string>(recv_buffer.data(), bytes_transferred);
		gameServer.processUdpMessage(data, remote_endpoint);
	}
	else {
		Logger::error("Erro ao receber pacote UDP: {}", error.message());
		if (error == boost::asio::error::connection_refused) {
			if (playersConnections.find(remote_endpoint) != playersConnections.end()) {
				gameServer.removePlayerFromGame(playersConnections[remote_endpoint]);
			}
			Logger::info("Cliente removido da lista de conexoes UDP: {}:{}", remote_endpoint.address().to_string(), remote_endpoint.port());
		}
	}

	startReceive();
}

void UdpServer::broadcastMessage(std::shared_ptr<std::string> message) {

	for (const auto& [client, playerId] : playersConnections) {
		socket.async_send_to(
			boost::asio::buffer(*message),
			client,
			[this, client, playerId, message](const boost::system::error_code& error, std::size_t bytes) {
				if (error) {
					Logger::error("Erro ao enviar UDP {}", error.message());

					if (error == boost::asio::error::connection_refused) {
						removeClient(std::make_shared<int>(playerId));
					}
				}
			}
		);
	}
}

void UdpServer::removeClient(std::shared_ptr<int> playerId) {

	if (isPlayerConnected(*playerId)) {
		auto connection = playersEndpoints[*playerId];
		io_context.post([this, connection, playerId]() {
			auto it = playersConnections.find(connection);
			if (it != playersConnections.end()) {
				playersConnections.erase(connection);
				playersEndpoints.erase(*playerId);
			}
			gameServer.removePlayer(*playerId);
		});
	}
}

void UdpServer::addConnection(int playerId, boost::asio::ip::udp::endpoint connection) {
	if (isPlayerConnected(playerId)) {
		Logger::debug("Cliente já registrado - ID: {}", playerId);
		return;
	}

	Logger::info("Registrando novo cliente UDP - ID: {}", playerId);
	playersConnections[remote_endpoint] = playerId;
	playersEndpoints[playerId] = remote_endpoint;
}

bool UdpServer::isPlayerConnected(int playerId)
{
	return (playersEndpoints.find(playerId) != playersEndpoints.end());
}
