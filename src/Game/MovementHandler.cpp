#include "MovementHandler.h"

#include <cmath>

#include "../Utils/Logger.h"


const float SPEED = 200;
static constexpr float MAX_SPEED = 200.0f;
static constexpr float PLAYER_HALF_SIZE = 64.0f;
static constexpr float MAP_SIZE = 5000.0f;

MovementHandler::MovementHandler(GameState& gameState) : gameState(gameState) {}

void MovementHandler::handleDirectionChange(int playerId, float x, float y) {
	try {
		gameState.updatePlayerDirection(playerId, x, y);
		Logger::debug("Movimento recebido de: {}, X={}, Y={}", playerId, x, y);

	}
	catch (const json::exception& e) {
		Logger::error("Erro ao processar json de movimento: {}", e.what());
	}
}

void MovementHandler::updatePosition(Player& player, float deltaTime) {

    float dx = player.getDirectionX();
    float dy = player.getDirectionY();

    float length = std::sqrt(dx * dx + dy * dy);
    if (length > 0.0f) {
        dx /= length;
        dy /= length;
    }

    float newX = player.getX() + dx * MAX_SPEED * deltaTime;
    float newY = player.getY() + dy * MAX_SPEED * deltaTime;

    newX = std::clamp(newX, PLAYER_HALF_SIZE, MAP_SIZE - PLAYER_HALF_SIZE);
    newY = std::clamp(newY, PLAYER_HALF_SIZE, MAP_SIZE - PLAYER_HALF_SIZE);

    player.setPosition(newX, newY);
}