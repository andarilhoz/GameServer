#include "PlayerSystem.h"

#include "../Config/GameConfig.h"
#include "../../Utils/Logger.h"

const int PLAYER_MAX_SIZE = 2240;

PlayerSystem::PlayerSystem(GameState& state) : gameState(state) {}


Player PlayerSystem::addPlayer(const std::string& nickname) {
    float startX = rand() % int(GameConfig::MAP_SIZE);
    float startY = rand() % int(GameConfig::MAP_SIZE);

    Player newPlayer(nextPlayerId, nickname, startX, startY, 0, 0, 64);
    gameState.addPlayer(newPlayer);
    nextPlayerId++;

    return newPlayer;
}

void PlayerSystem::removePlayer(int playerId) {
    gameState.removePlayer(playerId);
}

Player& PlayerSystem::getPlayer(int playerId) {
    return gameState.getPlayer(playerId);
}

void PlayerSystem::eatFood(Player& player) {
    if (player.getSize() < PLAYER_MAX_SIZE) {
        player.increaseSize(1);
    }

    player.addPoints(1);
}

void PlayerSystem::eatPlayer(Player& player, int victimSize) {
    if (player.getSize() < PLAYER_MAX_SIZE) {
        Logger::info("Player increasing size: {}", victimSize / 2);
        Logger::info("Size before {}", player.getSize());
        player.increaseSize(int(victimSize/2));
        Logger::info("Final Size: {}", player.getSize());
    }

    player.addPoints(victimSize/2);
}