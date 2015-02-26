#pragma once
#include "Player.h"
#include "Bird.h"
#include "Pipe.h"
#include "Bomb.h"
#include <vector>

class Game
{
public:
	Game();
	~Game();

	void Start();
	void End();
	void CrossPipe();

private:
	Player* player;
	std::vector<Bird*> Birds;
	std::vector<Bomb*> Bombs;
	std::vector<Pipe*> Pipes;
	int score;

};

