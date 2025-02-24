#include "MovementHandler.h"
#include "../Utils/Logger.h"

MovementHandler::MovementHandler(GameState& gameState) : gameState(gameState) {}

const float SPEED = 200;

void MovementHandler::handleMovement(ClientUdpMessage movementData) {
	try {
		gameState.updatePlayerDirection(movementData.playerId, movementData.x, movementData.y);
		Logger::info("Movimento recebido de: {}, X={}, Y={}", movementData.playerId, movementData.x, movementData.y);

	}
	catch (const json::exception& e) {
		Logger::error("Erro ao processar json de movimento: {}", e.what());
	}
}