#pragma once

#include "GameState.h"
#include "../Systems/MovementHandler.h"
#include "../Systems/PlayerSystem.h"
#include "../Networking/ConnectionManager.h"
#include "../Systems/FoodController.h"

#include <thread>

class GameLoop {
public:
	GameLoop(GameState& gameState, MovementHandler& movementSystem, PlayerSystem& playerSystem, ConnectionManager& connectionManager, FoodController& foodController);
private:
	void update();
	void generateFood();
	void checkFoodCollision(Player& player);
	void removeInactivePlayers(std::vector<int> playersIds);

	GameState& gameState;
	ConnectionManager& connectionManager;
	MovementHandler& movementSystem;
	FoodController& foodController;
	PlayerSystem& playerSystem;
	std::thread updateThread;
	std::atomic<bool> running = true;
};