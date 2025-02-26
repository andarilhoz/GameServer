#include "UdpServer.h"
#include <iostream>
#include "../Utils/Logger.h"
#include <nlohmann/json.hpp>
#include "../Game/GameServer.h"
#include "ClientUdpMessage.h"

UdpServer::UdpServer(boost::asio::io_context& io_context, int port, GameServer& gameServer)
	:socket(io_context, udp::endpoint(udp::v4(), port)), port(port), gameServer(gameServer) {
	startReceive();
}

void UdpServer::startReceive() {
	socket.async_receive_from(boost::asio::buffer(recv_buffer), remote_endpoint,
		[this](const boost::system::error_code& error, std::size_t bytes_transferred) {
			handle_receive(error, bytes_transferred);
		});
}

void UdpServer::handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred) {
	try {
		if (!error) {

			auto dataPtr = std::make_shared<std::string>(recv_buffer.data(), bytes_transferred);
			json parsedData = json::parse(*dataPtr);

			int playerId = parsedData["playerId"];
			MessageType type = parsedData["type"] == "move" ? MessageType::move : MessageType::unknow;
			float x = parsedData["x"];
			float y = parsedData["y"];

			if (!gameServer.isPlayerTcpConnected(playerId)) {
				return;
			}

			connectedClients.insert(remote_endpoint);

			if (playersEndpoints.find(playerId) == playersEndpoints.end()) {
				Logger::info("Registrando novo cliente UDP - ID: {}", playerId);
				players[remote_endpoint] = playerId;
				playersEndpoints[playerId] = remote_endpoint;
			}
			else {
				Logger::info("Cliente já registrado - ID: {}", playerId);
			}


			ClientUdpMessage message = ClientUdpMessage(type, playerId, x, y);
			gameServer.processPlayerDirection(message);
		}
		else {
			Logger::error("Erro ao receber pacote UDP: {}", error.message());
			if (error == boost::asio::error::connection_refused) {
				if (players.find(remote_endpoint) != players.end()) {
					gameServer.removePlayerFromGame(players[remote_endpoint]);

				}
				Logger::info("Cliente removido da lista de conexoes UDP: {}:{}", remote_endpoint.address().to_string(), remote_endpoint.port());
			}
		}

		startReceive();
	}
	catch (const std::exception& e) {
		Logger::error("Erro ao parsear json: {}", e.what());
	}
}

void UdpServer::broadcastMessage(const std::string& message) {
	std::vector<boost::asio::ip::udp::endpoint> clientsToRemove;
	std::vector<boost::asio::ip::udp::endpoint> clients(connectedClients.begin(), connectedClients.end());

	for (const auto& client : clients) {
		boost::system::error_code error;
		socket.send_to(boost::asio::buffer(message), client, 0, error);

		if (error) {
			Logger::error("Erro ao enviar pacote UDP para {}:{} - {}",
				client.address().to_string(), client.port(), error.message());

			if (error == boost::asio::error::connection_refused) {
				clientsToRemove.push_back(client); // ✅ Agora está seguro remover diretamente
				Logger::info("Cliente removido da lista de conexões UDP: {}:{}",
					client.address().to_string(), client.port());
			}
		}
	}


	for (const auto& client : clientsToRemove) {
		connectedClients.erase(client);
		Logger::info("Cliente removido da lista de conexoes UDP: {}:{}", client.address().to_string(), client.port());
	}
}

void UdpServer::removeClient(int playerId) {
	auto it = playersEndpoints.find(playerId);
	if (it != playersEndpoints.end()) {
		connectedClients.erase(it->second);
		players.erase(it->second);
		playersEndpoints.erase(it);
	}
	gameServer.removePlayer(playerId);
}