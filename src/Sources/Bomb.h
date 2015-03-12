#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Sources/MovableObject.h"

class Player;
class Bird;

class Bomb : public MovableObject
{
public:
	Bomb(glm::vec3 _pos, float size);
	~Bomb();
	void Move(float speed);
	void SetPosition(glm::vec3 pos);

	void ExplodeBird(Bird* bird, const std::vector<Bird*>& birds);
	bool IsActive();
	void SetActive(bool _active);
	bool isOutOfMap();
	bool IsPicked();
	void SetPicked(bool _picked);
	void SetReadyToExplode(bool _explode);
	bool IsReadyToExplode();

private:
	float endZ;
	bool active;
	bool picked;
	bool readyToExplode;
};

