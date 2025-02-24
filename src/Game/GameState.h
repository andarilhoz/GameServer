#pragma once

#include <unordered_map>
#include "Player.h"
#include <chrono>

class GameState {
public:
	void addPlayer(Player player);
	void removePlayer(int playerId);
	void updatePlayerPosition(int playerId, float x, float y);
	void updatePlayerDirection(int playerId, float x, float y);
	float getDeltaTime();
	Player& getPlayer(int playerId);
	bool isPlayerInactive(int playerId, float timeoutSeconds);
	std::unordered_map<int, Player>& getAllPlayers();

private:
	std::unordered_map<int, Player> players;
	std::unordered_map<int, std::chrono::steady_clock::time_point> lastActivity;
	std::chrono::steady_clock::time_point lastUpdateTime;
};