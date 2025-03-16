#pragma once

#include <nlohmann/json.hpp>

#include "../Core/GameState.h"
#include "MapController.h"
#include <vector>

#include "../Entities/GridCell.cpp"
#include "CollisionSystem.h"

class FoodController {
public:
	FoodController(GameState& gameState, MapController& mapController, CollisionSystem& collisionSystem);
	std::vector<Food> generateFood(int foodAmount);
	void removeFood(int foodId);

    std::unordered_map<GridCell, std::vector<int>>& getFoodGrid();

private:
	MapController& mapController;
	GameState& gameState;
    CollisionSystem& collisionSystem;


    std::unordered_map<GridCell, std::vector<int>> foodGrid;

	void addFood(int foodId, float x, float y);
	bool isValidFoodPosition(float x, float y);
	int generateFoodId();
};