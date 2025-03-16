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

float MovementHandler::getVelocity(const float& size) {
    float fator = (size - GameConfig::PLAYER_MIN_SIZE) /
                  (GameConfig::PLAYER_MAX_SIZE - GameConfig::PLAYER_MIN_SIZE);
    fator = std::clamp(fator, 0.0f, 1.0f);

    float expoente = 3.0f; // experimente valores entre 1.0 e 5.0
    return GameConfig::MAX_SPEED - std::pow(fator, expoente) * (GameConfig::MAX_SPEED - GameConfig::MIN_SPEED);
}

void MovementHandler::updatePosition(Player& player, float deltaTime) {

    float dx = player.getDirectionX();
    float dy = player.getDirectionY();

    float size = player.getSize();

    float length = std::sqrt(dx * dx + dy * dy);
    if (length > 0.0f) {
        dx /= length;
        dy /= length;
    }


    float newX = player.getX() + dx * getVelocity(size) * deltaTime;
    float newY = player.getY() + dy * getVelocity(size) * deltaTime;


    newX = std::clamp(newX, player.getSize(), GameConfig::MAP_SIZE - player.getSize());
    newY = std::clamp(newY, player.getSize(), GameConfig::MAP_SIZE - player.getSize());

    player.setPosition(newX, newY);
}