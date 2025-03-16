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
    void setRespawnTimer(float respawnTime);
    float getRespawnTime();
	void increaseSize(float valueAdded);
	void kill();
	int getPoints();
	void addPoints(int points);
    void setSize(int newSize);
    void setPoints(int newPoints);

	std::string toJson() const;

    void respawn(std::pair<float, float> pair1);
    void updateRespawnTime(float delta);

private:
	int id;
	int points = 0;
	std::string nickname;
	float x, y;
	float directionX, directionY;
    float respawn_timer;
	float size;
	bool alive;
};