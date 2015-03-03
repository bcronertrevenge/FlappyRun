#pragma once
#include <glm/glm.hpp>
#include "Player.h"

class Pipe
{
public:
	Pipe(glm::vec3 pos, float startZ, float endZ);
	~Pipe();
	void Move(float speed);
	glm::vec3 GetPosition();
	bool isOutOfMap();
	bool CheckHitPlayer(Player *_player);

private:
	glm::vec3 Position;
	float sizePipe;
	float startZ;
	float endZ;

};

