#include "Bird.h"


Bird::Bird(Player* _player) : player(_player)
{
	Position = glm::vec3(player->GetPosition().x, player->GetPosition().y, player->GetPosition().z - 3);
	size = glm::vec3(1);
	speed = 2.5;
}


Bird::~Bird()
{
}

void Bird::KillBird()
{
	//TODO
}