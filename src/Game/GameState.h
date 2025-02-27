#pragma once

#include <unordered_map>
#include "Player.h"
#include "Food.h"
#include <chrono>
#include <set>

class GameState {
public:
	void addPlayer(Player player);
	void removePlayer(int playerId);
	void updatePlayerPosition(int playerId, float x, float y);
	void updatePlayerDirection(int playerId, float x, float y);
	Food spawnFood(int foodId, float x, float y);
	void removeFood(int foodId);
	float getDeltaTime();
	std::unordered_map<int, Food>& getFoodList();
	Food getFood(int foodId);
	Player& getPlayer(int playerId);
	bool isPlayerInactive(int playerId, float timeoutSeconds);
	bool isPlayerAdded(int playerId);
	std::unordered_map<int, Player>& getAllPlayers();

private:
	std::unordered_map<int, Player> players;
	std::unordered_map<int, Food> foodList;
	std::unordered_map<int, std::chrono::steady_clock::time_point> lastActivity;
	std::chrono::steady_clock::time_point lastUpdateTime;
};