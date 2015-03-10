#pragma once
#include <glm/glm.hpp>

class Player;
class Bird;

class Bomb
{
public:
	Bomb(glm::vec3 _pos, float size);
	~Bomb();
	void Move(float speed);
	glm::vec3 GetPosition();
	void SetPosition(glm::vec3 pos);

	void ExplodeBird(Bird* bird);
	bool IsActive();
	void SetActive(bool _active);
	bool CheckHitPlayer(Player *_player);
	bool CheckHitBird(Bird *_bird);
	bool isOutOfMap();
	bool IsPicked();
	void SetPicked(bool _picked);
	void SetReadyToExplode(bool _explode);
	bool IsReadyToExplode();

private:
	glm::vec3 Position;
	float sizeBomb;
	float endZ;
	bool active;
	bool picked;
	bool readyToExplode;
};

