#pragma once

#include "GameState.h"
#include <string>
#include <nlohmann/json.hpp>
#include "../Server/ClientUdpMessage.h"

using json = nlohmann::json;

class MovementHandler {
public:
	MovementHandler(GameState& gameState);
	void handleDirectionChange(int playerId, float x, float y);
	void updatePosition(Player& player, float deltaTime);
private:
	GameState& gameState;
};
