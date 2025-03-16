#include "Player.h"
#include <sstream>
#include "../Config/GameConfig.h"

Player::Player() : id(-1), x(0.0f), y(0.0f), directionX(200), directionY(0), alive(false), size(1) {};

Player::Player(int id, std::string nickname, float x, float y, float directionX, float directionY, float size)
	: id(id), nickname(nickname), x(x), y(y), size(size), alive(true), directionX(directionX), directionY(directionY) { }

int Player::getId() const { return id; }
float Player::getX() const { return x; }
float Player::getY() const { return y; }
float Player::getDirectionX() const { return directionX; }
float Player::getDirectionY() const { return directionY; }
std::string Player::getNickname() const { return nickname; }
float Player::getSize() const { return size; }
bool Player::isAlive() const { return alive; }

void Player::setPosition(float newX, float newY) {
	x = newX;
	y = newY;
}

void Player::setDirection(float newX, float newY) {
	directionX = newX;
	directionY = newY;
}

void Player::increaseSize(float valueAdded) {
    if(size + valueAdded > GameConfig::PLAYER_MAX_SIZE){
        size = GameConfig::PLAYER_MAX_SIZE;
        return;
    }
	size += valueAdded;
}

void Player::kill() {
	alive = false;
}

int Player::getPoints() {
	return points;
}

void Player::setPoints(int newPoints){
    points = newPoints;
}

void Player::setSize(int newSize){
    size = newSize;
}

void Player::addPoints(int receivedPoints) {
	points += receivedPoints;
}

std::string Player::toJson() const {
	std::ostringstream ss;
	ss << "{ \"id\": " << id
		<< ", \"x\": " << x
		<< ", \"y\": " << y
		<< ", \"size\": " << size
		<< ", \"alive\": " << (alive ? "true" : "false") << " }";
	return ss.str();
}

void Player::setRespawnTimer(float respawnTime) {
    respawn_timer = respawnTime;
}

float Player::getRespawnTime() {
    return respawn_timer;
}

void Player::updateRespawnTime(float delta){
    respawn_timer -= delta;
}

void Player::respawn(std::pair<float, float> respawnPosition) {
    alive = true;
    x = respawnPosition.first;
    y = respawnPosition.second;
}
