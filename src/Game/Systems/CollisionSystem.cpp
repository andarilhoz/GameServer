#include "CollisionSystem.h"

#include "../../Utils/Logger.h"
#include "../Config/GameConfig.h"
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

std::pair<float, float> CollisionSystem::getRandomSafePositionForSpawn() {
    auto &players = gameState.getAllPlayers();
    int mapWidth = GameConfig::MAP_SIZE;
    int mapHeight = GameConfig::MAP_SIZE;

    const int maxAttempts = 100;

    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        // Generate random coordinates within map boundaries
        float randomX = static_cast<float>(rand() % mapWidth);
        float randomY = static_cast<float>(rand() % mapHeight);

        bool isSafe = true;

        // Check if the position is safe from collisions with existing players
        for (const auto &[id, player]: players) {
            if (!player.isAlive())
                continue;

            float playerX = player.getX();
            float playerY = player.getY();
            float playerSize = player.getSize();

            // Calculate distance to the player
            float distance = std::hypot(randomX - playerX, randomY - playerY);

            if (distance < playerSize * 2) { // Ensure enough space to avoid collisions
                isSafe = false;
                break;
            }
        }

        // If the position is verified as safe, return it
        if (isSafe) {
            return std::make_pair(randomX, randomY);
        }
    }

    // If no safe position is found, return a default value and log a warning
    Logger::warn("Could not find a safe spawning position after multiple attempts.");
    return std::make_pair(0.0f, 0.0f);
}


std::vector<int> CollisionSystem::checkFoodCollision(float playerX, float playerY, float playerRadius, std::unordered_map<GridCell, std::vector<int>>& foodGrid)
{
    float foodRadius = GameConfig::FOOD_SIZE;
    float combinedRadius = playerRadius + foodRadius;
    float combinedRadiusSquared = combinedRadius * combinedRadius; // Calcula o raio combinado ao quadrado
    std::vector<int> collisions;

    // Obtém todas as células que precisam ser verificadas (somente ao redor do jogador)
    std::vector<GridCell> cells = getAllCells(playerX, playerY, playerRadius);

    for (const auto& cell : cells) {
        for (int foodId : foodGrid[cell]) {
            // Pega os dados da comida no gameState
            const auto& food = gameState.getFood(foodId);

            // Calcula a distância ao quadrado entre o jogador e a comida
            float distSquared = (food.x - playerX) * (food.x - playerX) +
                                (food.y - playerY) * (food.y - playerY);

            // Verifica colisão com base na distância quadrada
            if (distSquared < combinedRadiusSquared) {
                collisions.emplace_back(foodId);
            }
        }
    }
    return collisions;
}

std::vector<GridCell> CollisionSystem::getAllCells(float playerX, float playerY, float playerRadius)
{
    // Calcula o bounding box do c�rculo do player
    float left = playerX - playerRadius;
    float right = playerX + playerRadius;
    float top = playerY - playerRadius;
    float bottom = playerY + playerRadius;

    // Converte para coordenadas de grade (c�lula)
    int minCellX = static_cast<int>(std::floor(left / GameConfig::GRID_CELL_SIZE));
    int maxCellX = static_cast<int>(std::floor(right / GameConfig::GRID_CELL_SIZE));
    int minCellY = static_cast<int>(std::floor(top / GameConfig::GRID_CELL_SIZE));
    int maxCellY = static_cast<int>(std::floor(bottom / GameConfig::GRID_CELL_SIZE));

    std::vector<GridCell> cells;
    cells.reserve((maxCellX - minCellX + 1) * (maxCellY - minCellY + 1));

    for (int cx = minCellX; cx <= maxCellX; ++cx) {
        for (int cy = minCellY; cy <= maxCellY; ++cy) {
            cells.push_back({ cx, cy });
        }
    }

    return cells;
}



GridCell CollisionSystem::getGridCell(float x, float y) {
    return { int(x / GameConfig::GRID_CELL_SIZE), int(y / GameConfig::GRID_CELL_SIZE) };
}