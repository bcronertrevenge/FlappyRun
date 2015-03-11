#pragma once
#include <glm/glm.hpp>
#include "Player.h"
#include "Bird.h"

class Pipe : public MovableObject
{
public:
	Pipe(glm::vec3 pos, float endZ);
	~Pipe();

	bool isOutOfMap();
	bool CheckHitObject(MovableObject *_object);

	bool hasHit();
	bool HasPassedPlayer(Player * _player);

private:
	float endZ;
	bool hit;
	bool hasAlreadyPassPlayer;
};

