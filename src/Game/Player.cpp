#include "Player.h"
#include <sstream>

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

void Player::setSize(float newSize) {
	size = newSize;
}

void Player::kill() {
	alive = false;
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