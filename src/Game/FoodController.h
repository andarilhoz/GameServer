#pragma once

#include <nlohmann/json.hpp>

#include "GameState.h"
#include "MapController.h"
#include <vector>

#include "GridCell.cpp"

class FoodController {
public:
	FoodController(int initialFood, GameState& gameState, MapController mapController);
	std::vector<Food> generateFood(int foodAmount);
	nlohmann::json getFoodInfo();
	int checkFoodCollision(float playerX, float playerY, float playerRadius);
	void removeFood(float foodId);
	std::vector<GridCell> getAllCells(float playerX, float playerY, float playerRadius);
private:

	int initialFood;
	
	MapController mapController;
	GameState& gameState;
	void addFood(int foodId, float x, float y);
	bool isValidFoodPosition(float x, float y);
	int generateFoodId();
};