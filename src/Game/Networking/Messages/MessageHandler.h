#pragma once

#include "GameMessage.h"
#include "../../Core/GameState.h"

class MessageHandler {
public:
	static GameMessage serializePlayersStatus(std::unordered_map<int, Player> players);
	static GameMessage serializeDisconectedPlayer(int playerId);
	static GameMessage serializeEatEvent(int foodId, int playerId);
	static GameMessage serializeFoodSpawn(std::vector<Food> newFoods);
	static GameMessage serializeGameStateData(int playerId, GameState& gameState);
	static GameMessage serializeNewPlayer(Player player);
	static GameMessage serializePong(uint64_t timestamp);
private:
};