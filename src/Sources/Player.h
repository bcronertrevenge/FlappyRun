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
	void Move();
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

private:
	bool hasBomb;
	float speed;
	bool isDead;
	float width;
	Bomb * BombPicked;
	float speedMax;
	float speedMin;

	float heightMax;
	bool ascending;
	bool jumping;
	float speedJump;
	float LastTimeJump;
};

