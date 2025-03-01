#pragma once

#include "../Core/GameState.h"
#include  "../Entities/Player.h"

class PlayerSystem {
public:
	PlayerSystem(GameState& gameState);
	Player addPlayer(const std::string& name);
	void removePlayer(int playerId);
	Player& getPlayer(int playerId);

	void eatFood(Player& playerId);
private:
	GameState& gameState;
	int nextPlayerId = 1;
};