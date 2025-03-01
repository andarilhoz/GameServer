#pragma once

#include <nlohmann/json.hpp>

#include "../Core/GameState.h"
#include "MapController.h"
#include <vector>

#include "../Entities/GridCell.cpp"

class FoodController {
public:
	FoodController(GameState& gameState, MapController& mapController);
	std::vector<Food> generateFood(int foodAmount);

	int checkFoodCollision(float playerX, float playerY, float playerRadius);
	void removeFood(float foodId);
	std::vector<GridCell> getAllCells(float playerX, float playerY, float playerRadius);
private:	
	MapController& mapController;
	GameState& gameState;
	void addFood(int foodId, float x, float y);
	bool isValidFoodPosition(float x, float y);
	int generateFoodId();
};