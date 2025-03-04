#pragma once

#include "../Core/GameState.h"
#include <optional>

class CollisionSystem {
public:
	CollisionSystem(GameState& gameState);
	int checkForPlayerCollision(Player& player);
private:

	GameState& gameState;
};