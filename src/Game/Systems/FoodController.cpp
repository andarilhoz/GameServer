#include "FoodController.h"

#include <nlohmann/json.hpp>

#include "../../Utils/Logger.h"
#include "../Config/GameConfig.h"
#include <vector>


FoodController::FoodController(GameState &gameState, MapController &mapController,
                               CollisionSystem &collisionSystem)
        : gameState(gameState), mapController(mapController), collisionSystem(collisionSystem) {};

std::vector<Food> FoodController::generateFood(int foodAmount) {
    
    std::vector<Food> generatedFood;

    while (gameState.getFoodList().size() < foodAmount) {
        float x = mapController.generateRandomPosition(GameConfig::FOOD_SIZE);
        float y = mapController.generateRandomPosition(GameConfig::FOOD_SIZE);

        if (isValidFoodPosition(x, y)) {
            int foodId = generateFoodId();
            Food foodGenerated = gameState.spawnFood(foodId, x, y);
            generatedFood.push_back(foodGenerated);
            addFood(foodId, x, y);
        }
    }
    return generatedFood;
}

int FoodController::generateFoodId() {
    static int nextFoodId = 1000;  // Evita conflito com IDs de players
    return nextFoodId++;
}

bool FoodController::isValidFoodPosition(float x, float y) {
    GridCell cell = collisionSystem.getGridCell(x, y);

    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            GridCell neightbor = { cell.x + dx, cell.y + dy };
            if (foodGrid.count(neightbor)) {
                for (int foodId : foodGrid[neightbor]) {
                    if (std::hypot(gameState.getFood(foodId).x - x, gameState.getFood(foodId).y - y) < GameConfig::FOOD_SIZE) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

void FoodController::addFood(int foodId, float x, float y) {
    gameState.spawnFood(foodId, x, y);
    GridCell cell = collisionSystem.getGridCell(x, y);
    foodGrid[cell].push_back(foodId);
}


void FoodController::removeFood(int foodId) {
    const auto& food = gameState.getFood(foodId);
    GridCell cell = collisionSystem.getGridCell(food.x, food.y);
    auto it = foodGrid.find(cell);
    if (it != foodGrid.end()) {
        auto& vec = it->second;
        vec.erase(std::remove(vec.begin(), vec.end(), foodId), vec.end());
    }
    gameState.removeFood(foodId);
    
}

std::unordered_map<GridCell, std::vector<int>>& FoodController::getFoodGrid() {
    return foodGrid;
}
