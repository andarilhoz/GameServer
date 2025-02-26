#include "GameState.h"
#include "../Utils/Logger.h"

void GameState::addPlayer(Player player) {
	Logger::info("Adicionando player: {}", player.getNickname());
	players[player.getId()] = player;
	lastActivity[player.getId()] = std::chrono::steady_clock::now();
}

void GameState::removePlayer(int playerId) {
	Logger::info("Removendo player: {}", playerId);
	players.erase(playerId);
	lastActivity.erase(playerId);
}

void GameState::updatePlayerPosition(int playerId, float x, float y) {
	Logger::debug("Localizando player: {}", playerId);
	if (players.find(playerId) == players.end())
		return;

	players[playerId].setPosition(x, y);
	Logger::debug("Jogador {}, movido para ({},{})", playerId, x, y);
}

void GameState::updatePlayerDirection(int playerId, float x, float y) {
	Logger::debug("Localizando player: {}", playerId);
	if (players.find(playerId) == players.end())
		return;

	players[playerId].setDirection(x, y);
	lastActivity[playerId] = std::chrono::steady_clock::now();
	Logger::debug("Jogador {}, com direcao para ({},{})", playerId, x, y);
}

bool GameState::isPlayerInactive(int playerId, float timeoutSeconds) {
	using namespace std::chrono;

	if (lastActivity.find(playerId) == lastActivity.end()) return true;

	float elapsed = duration<float>(steady_clock::now() - lastActivity[playerId]).count();
	return elapsed > timeoutSeconds;
}

Player& GameState::getPlayer(int playerId) {
	return players[playerId];
}

std::unordered_map<int, Food> GameState::getFoodList() {
	return foodList;
}

Food GameState::getFood(int foodId) {
	return foodList[foodId];
}

std::unordered_map<int, Player>& GameState::getAllPlayers() {
	return players;
}

float GameState::getDeltaTime() {
	using namespace std::chrono;

	auto now = steady_clock::now();
	float delta = duration<float>(now - lastUpdateTime).count();
	lastUpdateTime = now;

	return delta;
}

void GameState::spawnFood(int foodId, float x, float y) {
	Food food = Food(foodId, x, y);
	foodList[foodId] = food;
}