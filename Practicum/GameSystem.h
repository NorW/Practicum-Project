#pragma once
#include "StdAfx.h"
#include "InputManager.h"
#include "Map.h"
#include "AI.h"
#include "AIManager.h"
#include "Camera.h"
#include "HUD.h"
#define GSSTATUS_CONTINUE 0
#define GSSTATUS_END 1

class GameSystem
{
private:
	Map* _map;
	Player* _player;
	AI* _AI;
	Unit* _unitSelected;
	Tile* _factorySelected;
	Tile* _selectedTile;
	std::vector<Tile*>* _curPath;

	bool _gameMenuOpen;
	bool _movingUnit;
	bool _attackingUnit;

	std::shared_ptr<HUD> _HUD;

	void handlePlayerMapInput();

	void handleAI();

#ifdef DEBUG_MODE
	bool _debugOn;

	void handleDebug();
#endif

public:
	GameSystem();

	int setMap(Map* map);

	int update(int ms);

	void draw();
};