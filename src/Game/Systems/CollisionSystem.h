#pragma once

#include "../Core/GameState.h"
#include <optional>

#include "../Entities/GridCell.cpp"

class CollisionSystem {
public:
	CollisionSystem(GameState& gameState);
	int checkForPlayerCollision(Player& player);
    std::vector<int> checkFoodCollision(float playerX, float playerY, float playerRadius, std::unordered_map<GridCell, std::vector<int>>& foodGrid);
    GridCell getGridCell(float x, float y);
    std::pair<float, float> getRandomSafePositionForSpawn();
private:

	GameState& gameState;
    std::vector<GridCell> getAllCells(float playerX, float playerY, float playerRadius);


};