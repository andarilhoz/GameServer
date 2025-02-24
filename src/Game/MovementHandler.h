#pragma once

#include "GameState.h"
#include <string>
#include <nlohmann/json.hpp>
#include "../Server/ClientUdpMessage.h"

using json = nlohmann::json;

class MovementHandler {
public:
	MovementHandler(GameState& gameState);
	void handleMovement(ClientUdpMessage movementData);
private:
	GameState& gameState;
};
