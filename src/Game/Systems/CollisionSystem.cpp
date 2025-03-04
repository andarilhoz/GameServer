#include "CollisionSystem.h"

#include "../../Utils/Logger.h"
#include <cmath>

CollisionSystem::CollisionSystem(GameState& gameState) : gameState(gameState) {}

int CollisionSystem::checkForPlayerCollision(Player& currentPlayer) {
	auto& players = gameState.getAllPlayers();
	
	int playerX = currentPlayer.getX();
	int playerY = currentPlayer.getY();
	float playerSize = currentPlayer.getSize();

	for (auto& [id, player] : players) {
		if (player.getId() == currentPlayer.getId())
			continue;

		if (!player.isAlive()) {
			continue;
		}

		int otherX = player.getX();
		int otherY = player.getY();
		float otherSize = player.getSize();

		
		float dist = std::hypot(otherX - playerX, otherY - playerY);

		
		if (dist < (playerSize + otherSize)) {
			if (playerSize > otherSize) {
				return player.getId();
			}
		}
	}
	return -1;
}