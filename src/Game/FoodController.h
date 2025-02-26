#pragma once

#include <nlohmann/json.hpp>

#include "GameState.h"

class FoodController {
public:
	FoodController(int initialFood, GameState gameState);
	nlohmann::json getFoodInfo();
private:

	int initialFood;

	GameState gameState;
	void addFood(int foodId, float x, float y);
	void generateFood(int foodAmount);
	bool isValidFoodPosition(float x, float y);
	float generateRandomPosition();
	int generateFoodId();
};