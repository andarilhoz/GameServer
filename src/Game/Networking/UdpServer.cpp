#include "UdpServer.h"
#include <iostream>
#include "../../Utils/Logger.h"
#include "../../Utils/Time.h"
#include <nlohmann/json.hpp>
#include "../Core/GameServer.h"

UdpServer::UdpServer(boost::asio::io_context& io_context, int port)
	:socket(io_context, udp::endpoint(udp::v4(), port)), port(port), io_context(io_context) {
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

		Time::setInitialTimestamp();

		auto data = std::make_shared<std::string>(recv_buffer.data(), bytes_transferred);
		if (observable != nullptr) {
			observable(data, remote_endpoint);
		}
	}
	else {
		Logger::error("Erro ao receber pacote UDP: {}", error.message());
		if (error == boost::asio::error::connection_refused) {
			if (disconnectObservable) {
				disconnectObservable(remote_endpoint);
			}
			Logger::info("Cliente removido da lista de conexoes UDP: {}:{}", remote_endpoint.address().to_string(), remote_endpoint.port());
		}
	}

	startReceive();
}

void UdpServer::sendMessage(
	boost::asio::ip::udp::endpoint connection,
	std::shared_ptr<std::string> message,
	std::function<void(boost::asio::ip::udp::endpoint)> onClientError) {

	socket.async_send_to(
		boost::asio::buffer(*message),
		connection,
		[this, onClientError, connection, message](const boost::system::error_code& error, std::size_t bytes) {
			if (error) {
				Logger::error("Erro ao enviar UDP {}", error.message());

				if (error == boost::asio::error::connection_refused) {
					if (onClientError) {
						onClientError(connection);
					}
				}
			}
		}
	);
}

void UdpServer::subscribe(std::function<void(std::shared_ptr<std::string>, udp::endpoint)> callback) {
	observable = callback;
}

void UdpServer::subscribeDisconnect(std::function<void(udp::endpoint)> callback) {
	disconnectObservable = callback;
}