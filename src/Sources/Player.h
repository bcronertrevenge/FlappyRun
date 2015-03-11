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

	void applyForce(glm::vec3 _pos);
	float GetSpeed();
	void MoveLeft();
	void MoveRight();
	void SpeedUp();
	void SlowDown();
	bool IsDead();
	void PickBomb(Bomb* bomb);
	bool HasBomb();
	float GetMaxSpeed();
	void Jump(float _time);
	bool IsJumping();
	void SetJumping(bool _jump);

private:
	bool hasBomb;
	float speed;
	bool isDead;
	float width;
	Bomb * BombPicked;
	float speedMax;
	float speedMin;

	float LastTimeJump;
	bool jumping;
};

