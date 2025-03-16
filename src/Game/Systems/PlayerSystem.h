#pragma once

#include "../Core/GameState.h"
#include "CollisionSystem.h"
#include  "../Entities/Player.h"

class PlayerSystem {
public:
	PlayerSystem(GameState& gameState, CollisionSystem& collisionSystem);
	Player addPlayer(const std::string& name);
	void removePlayer(int playerId);
	Player& getPlayer(int playerId);

	void eatFood(Player& playerId);
	void eatPlayer(Player& player, int victimSize);
    void kill(Player& player);

	void respawn(Player& player);

private:
	GameState& gameState;
    CollisionSystem& collisionSystem;
	int nextPlayerId = 1;
};