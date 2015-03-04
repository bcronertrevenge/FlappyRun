#pragma once
#include <glm/glm.hpp>
#include "Player.h"
#include "Bird.h"

class Pipe
{
public:
	Pipe(glm::vec3 pos, float endZ);
	~Pipe();
	void Move(float speed);
	glm::vec3 GetPosition();
	void SetPosition(glm::vec3 pos);

	bool isOutOfMap();
	bool CheckHitPlayer(Player *_player);
	bool CheckHitBird(Bird *_bird);

	bool hasHit();
	bool HasPassedPlayer(Player * _player);
	void SetDraggedBird(Bird* _bird);

private:
	glm::vec3 Position;
	float sizePipe;
	float endZ;
	bool hit;
	bool hasAlreadyPassPlayer;

	Bird* birdToDrag;
};

