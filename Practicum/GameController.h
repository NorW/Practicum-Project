#pragma once
#include "StdAfx.h"
#include "MenuSystem.h"
#include "GameSystem.h"

enum GameState
{
	MenuState,
	Game
};

/*
	Handles the current game state as well as state changes.
*/
class GameController
{
private:
	MenuSystem* _menus;
	GameSystem* _game;
	GameState _state;

public:

	GameController();

	~GameController();

	bool update(int ms);

	void draw();
};