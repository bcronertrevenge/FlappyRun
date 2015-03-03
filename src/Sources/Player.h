#pragma once

#include <glm/glm.hpp>

class Player
{
public:
	Player(float width);
	~Player();

	void DropBomb();
	void KillPlayer();

	glm::vec3 GetPosition();
	float GetSize();
	float GetSpeed();
	void MoveLeft();
	void MoveRight();
	void SpeedUp();
	void SlowDown();

private:
	glm::vec3 Position;
	float sizePlayer;
	bool hasBomb;
	float speed;
	bool isDead;
	float width;
};

