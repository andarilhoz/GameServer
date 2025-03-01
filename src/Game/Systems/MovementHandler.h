#pragma once

#include "../Core/GameState.h"
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class MovementHandler {
public:
	MovementHandler(GameState& gameState);
	void handleDirectionChange(int playerId, float x, float y);
	void updatePosition(Player& player, float deltaTime);
private:
	GameState& gameState;
};
