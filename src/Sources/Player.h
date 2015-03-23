#pragma once

#include <glm/glm.hpp>
#include "Sources/Bomb.h"
#include "Sources/MovableObject.h"

class Player : public MovableObject
{
public:
	Player(float width);
	~Player();

	void DropBomb();
	void KillPlayer();

	void MoveLeft();
	void MoveRight();
	bool IsDead();
	void PickBomb(Bomb* bomb);
	bool HasBomb();
	void Jump(float _time);
	bool IsJumping();
	void SetJumping(bool _jump);

private:
	bool hasBomb;
	bool isDead;
	float width;
	Bomb * BombPicked;

	float LastTimeJump;
	bool jumping;
};

