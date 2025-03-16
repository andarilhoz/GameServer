#include "GameLoop.h"
#include "../Networking/Messages/MessageHandler.h"

#include "../Config/GameConfig.h"

static constexpr std::chrono::milliseconds UPDATE_INTERVAL(50);


GameLoop::GameLoop(
	GameState& gameState, 
	MovementHandler& movementSystem,
	PlayerSystem& playerSystem,
	ConnectionManager& connectionManager,
	FoodController& foodController, 
	CollisionSystem& collisionSystem) 
	: 
	gameState(gameState),
	movementSystem(movementSystem),
	playerSystem(playerSystem), 
	connectionManager(connectionManager),
	collisionSystem(collisionSystem),
    foodController(foodController)
{
	updateThread = std::thread(&GameLoop::update, this);
};


void GameLoop::update() {

	auto& allPlayers = gameState.getAllPlayers();

	while (running) {
		float deltaTime = gameState.getDeltaTime();

		generateFood();

		std::vector<int> playersToRemove;
		std::vector<std::pair<Player, Player>> killEvents;

		for (auto& [id, player] : allPlayers) {
			if (gameState.isPlayerInactive(id)) {
				playersToRemove.push_back(id);
				continue;
			}

			if (!player.isAlive()) {
				continue;
			}

			movementSystem.updatePosition(player, deltaTime);
			checkFoodCollision(player);
			int otherId = collisionSystem.checkForPlayerCollision(player);
			if (otherId != -1) {
				Player& other = gameState.getPlayer(otherId);
				playerSystem.eatPlayer(player, other.getSize());
				other.kill();

				killEvents.push_back(std::pair<Player, Player>(player, other));
			}
		}

		for (auto event : killEvents) {
			connectionManager.broadcastTcpMessage(MessageHandler::killPlayer(event.first, event.second));
		}

		connectionManager.broadcastUdpMessage(MessageHandler::serializePlayersStatus(allPlayers));
		removeInactivePlayers(playersToRemove);
		std::this_thread::sleep_for(UPDATE_INTERVAL);
	}
}

void GameLoop::generateFood() {
	int foodAmount = gameState.getFoodList().size();

	if (foodAmount < GameConfig::MIN_FOOD) {
		std::vector<Food> newFoods = foodController.generateFood(GameConfig::MIN_FOOD);
		connectionManager.broadcastTcpMessage(MessageHandler::serializeFoodSpawn(newFoods));
	}
}

void GameLoop::checkFoodCollision(Player& player) {
	std::vector<int> foodsCollected = collisionSystem.checkFoodCollision(player.getX(), player.getY(), player.getSize(), foodController.getFoodGrid());
	if (foodsCollected.empty()) {
		return;
	}
	else {
        for (int foodId : foodsCollected) {
            foodController.removeFood(foodId);
            playerSystem.eatFood(player);
            Logger::info("Jogador {} comeu comida {}, tamanho: {}", player.getNickname(), foodId, player.getSize());
            connectionManager.broadcastTcpMessage(MessageHandler::serializeEatEvent(foodId, player.getId()));
        }
	}
}

void GameLoop::removeInactivePlayers(std::vector<int> playersIds) {
	for (int playerId : playersIds) {
		playerSystem.removePlayer(playerId);
		connectionManager.removePlayer(playerId);
	}
}