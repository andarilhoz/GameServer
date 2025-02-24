#pragma once

#include <string>

class Player {
public:
	Player();
	Player(int id, std::string nickname, float x, float y, float directionX, float directionY, float size = 1.0f);

	int getId() const;
	std::string getNickname() const;
	float getX() const;
	float getY() const;
	float getSize() const;
	bool isAlive() const;

	float getDirectionX() const;
	float getDirectionY() const;

	void setPosition(float newX, float newY);
	void setDirection(float newX, float newY);
	void setSize(float newSize);
	void kill();

	std::string toJson() const;

private:
	int id;
	std::string nickname;
	float x, y;
	float directionX, directionY;
	float size;
	bool alive;
};