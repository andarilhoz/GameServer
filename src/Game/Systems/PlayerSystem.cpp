#include "PlayerSystem.h"

#include "../Config/GameConfig.h"
#include "../../Utils/Logger.h"

PlayerSystem::PlayerSystem(GameState &state, CollisionSystem &collisionSystem)
        : gameState(state), collisionSystem(collisionSystem) {}


Player PlayerSystem::addPlayer(const std::string& nickname) {
    auto [startX, startY] = collisionSystem.getRandomSafePositionForSpawn();

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
    if (player.getSize() < GameConfig::PLAYER_MAX_SIZE) {
        player.increaseSize(1);
    }

    player.addPoints(1);
}

void PlayerSystem::eatPlayer(Player& player, int victimSize) {
    if (player.getSize() < GameConfig::PLAYER_MAX_SIZE) {
        Logger::info("Player increasing size: {}", victimSize / 2);
        Logger::info("Size before {}", player.getSize());
        player.increaseSize(int(victimSize/2));
        Logger::info("Final Size: {}", player.getSize());
    }

    player.addPoints(victimSize/2);
}

void PlayerSystem::kill(Player& player) {
    player.setRespawnTimer( 5.0f);
    player.setPoints(0);
    player.setSize(GameConfig::PLAYER_MIN_SIZE);
    player.kill();
}

void PlayerSystem::respawn(Player& player) {
    auto spawnPosition = collisionSystem.getRandomSafePositionForSpawn();
    player.respawn(spawnPosition);
    Logger::info("Player {} respawned at position ({}, {}).", player.getNickname(), spawnPosition.first,
                 spawnPosition.second);
}

