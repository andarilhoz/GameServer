#include "FoodController.h"

#include <nlohmann/json.hpp>

#include "../Utils/Logger.h"
#include "../GameConfig.h"
#include <vector>


FoodController::FoodController(int initialFood, GameState& gameState, MapController mapController) : initialFood(initialFood), gameState(gameState), mapController(mapController) {};

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


std::unordered_map<GridCell, std::vector<int>> foodGrid;

GridCell getGridCell(float x, float y) {
    return { int(x / GameConfig::GRID_CELL_SIZE), int(y / GameConfig::GRID_CELL_SIZE) };
}

bool FoodController::isValidFoodPosition(float x, float y) {
    GridCell cell = getGridCell(x, y);

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
    GridCell cell = getGridCell(x, y);
    foodGrid[cell].push_back(foodId);
}

nlohmann::json FoodController::getFoodInfo() {
    nlohmann::json foodList = nlohmann::json::array();

    for (const auto& food : gameState.getFoodList()) {
        foodList.push_back({
            {"id", food.second.id},
            {"x", food.second.x},
            {"y", food.second.y}
            });
    }

    return foodList;
}

int FoodController::checkFoodCollision(float playerX, float playerY, float playerRadius)
{
    float foodRadius = GameConfig::FOOD_SIZE;
    std::vector<GridCell> cells = getAllCells(playerX, playerY, playerRadius);
    for (auto cell : cells) {
        for (int foodId : foodGrid[cell]) {
            // Pega a struct/objeto da comida no gameState
            const auto& food = gameState.getFood(foodId);

            // Calcula a distância entre player e a comida
            float dist = std::hypot(food.x - playerX, food.y - playerY);

            // Se a distância for menor do que a soma dos raios, temos colisão
            Logger::info("Radius: {}", playerRadius);
            if (dist < (playerRadius + foodRadius)) {
                return foodId; // Retorna o ID da comida colidida
            }
        }
    }

    // Se não encontrar colisão em nenhuma das células vizinhas, retorna -1
    return -1;
}

void FoodController::removeFood(float foodId) {
    const auto& food = gameState.getFood(foodId);
    GridCell cell = getGridCell(food.x, food.y);
    auto it = foodGrid.find(cell);
    if (it != foodGrid.end()) {
        auto& vec = it->second;
        vec.erase(std::remove(vec.begin(), vec.end(), foodId), vec.end());
    }
    gameState.removeFood(foodId);
    
}


std::vector<GridCell> FoodController::getAllCells(float playerX, float playerY, float playerRadius)
{
    // Calcula o bounding box do círculo do player
    float left = playerX - playerRadius;
    float right = playerX + playerRadius;
    float top = playerY - playerRadius;
    float bottom = playerY + playerRadius;

    // Converte para coordenadas de grade (célula)
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