#include "FoodController.h"


#include <random>
#include <nlohmann/json.hpp>


const int GRID_CELL_SIZE = 32;
const int FOOD_SIZE = 16;
const int FOOD_COUNT = 50;

static constexpr float MAP_SIZE = 5000.0f;


FoodController::FoodController(int initialFood, GameState gameState) : initialFood(initialFood), gameState(gameState) {};

void FoodController::generateFood(int foodAmount) {

    while (gameState.getFoodList().size() < foodAmount) {
        float x = generateRandomPosition();
        float y = generateRandomPosition();

        if (isValidFoodPosition(x, y)) {
            int foodId = generateFoodId();
            gameState.spawnFood(foodId, x, y);
        }
    }
}


float FoodController::generateRandomPosition() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(FOOD_SIZE, MAP_SIZE - FOOD_SIZE);
    return dist(gen);
}

int FoodController::generateFoodId() {
    static int nextFoodId = 1000;  // Evita conflito com IDs de players
    return nextFoodId++;
}


struct GridCell {
    int x, y;
    bool operator == (const GridCell& other) const {
        return x == other.x && y == other.y;
    }
};

namespace std {
    template<> struct hash<GridCell> {
        size_t operator()(const GridCell& cell) const {
            return hash<int>()(cell.x) ^ hash<int>()(cell.y);
        }
    };
}

std::unordered_map<GridCell, std::vector<int>> foodGrid;

GridCell getGridCell(float x, float y) {
    return { int(x / GRID_CELL_SIZE), int(y / GRID_CELL_SIZE) };
}

bool FoodController::isValidFoodPosition(float x, float y) {
    GridCell cell = getGridCell(x, y);

    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            GridCell neightbor = { cell.x + dx, cell.y + dy };
            if (foodGrid.count(neightbor)) {
                for (int foodId : foodGrid[neightbor]) {
                    if (std::hypot(gameState.getFood(foodId).x - x, gameState.getFood(foodId).y - y) < FOOD_SIZE) {
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
    nlohmann::json foodMessage;

    for (const auto& food : gameState.getFoodList()) {
        foodMessage["foods"].push_back({
            {"id", food.second.id},
            {"x", food.second.x},
            {"y", food.second.y}
            });
    }

    return foodMessage;
}