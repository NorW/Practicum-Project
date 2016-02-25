#pragma once
#include "StdAfx.h"
#include <glm\gtc\bitfield.hpp>
#include "InputManager.h"
#include "AssetPool.h"
#include "Menu.h"
#define MSSTATUS_CONTINUE 0
#define MSSTATUS_GAME 1
#define MSSTATUS_EXIT 2


/*
	Handles the main menu, option menu, and level select menus.
	The main menu shall have 3 buttons: "Level Select", "Options", and "Exit"
		Selecting level select shall move the user to the level select menu
		Selecting options shall move the user to the options menu
		Selecting exit shall end the game
	THe Level Select menu shall list a number of levels.
		Selecting any level will move the user to the game system
		Pressing the cancel button or selecting exit will move the user back to the main menu
	The Options menu shall list sound options and an option to exit
*/
class MenuSystem
{
private:

	enum MenuType
	{
		Main = 0,
		Level,
		Options,
		N_MENUS
	};

	Menu* _menus[N_MENUS];

	int _curMenu;
	int _curOption;

	std::vector<std::string> _mapNames;

public:
	MenuSystem();
	
	//Returns the name of the currently selected map
	std::string getSelectedMap();

	void setup();

	int update(int ms);

	void draw();
};
