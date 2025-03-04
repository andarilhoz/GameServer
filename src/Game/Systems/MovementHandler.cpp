#include "MovementHandler.h"

#include <cmath>

#include "../../Utils/Logger.h"
#include "../Config/GameConfig.h"


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

    float newX = player.getX() + dx * GameConfig::MAX_SPEED * deltaTime;
    float newY = player.getY() + dy * GameConfig::MAX_SPEED * deltaTime;


    newX = std::clamp(newX, player.getSize(), GameConfig::MAP_SIZE - player.getSize());
    newY = std::clamp(newY, player.getSize(), GameConfig::MAP_SIZE - player.getSize());

    float clampedX = std::max(player.getSize(), std::min(newX, GameConfig::MAP_SIZE - player.getSize()));
    float clampedY = std::max(player.getSize(), std::min(newY, GameConfig::MAP_SIZE - player.getSize()));


    player.setPosition(newX, newY);
}