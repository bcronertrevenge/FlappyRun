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
	void MoveLeft();
	void MoveRight();

private:
	glm::vec3 Position;
	float sizePlayer;
	bool hasBomb;
	float speed;
	bool isDead;
	float width;
};

