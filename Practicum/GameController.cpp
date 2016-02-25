#include "StdAfx.h"
#include "GameController.h"

GameController::GameController()
{
	_menus = new MenuSystem();
	_game = new GameSystem();
	_state = MenuState;
	GLuint shader = AssetPool::loadShader("Tile Shader", "Shaders/shader.vsh", "Shaders/shader.fsh");
	
	//TODO: remove temp code
	_menus->setup();
}

GameController::~GameController()
{
	if(_menus)
	{
		delete _menus;
	}

	if(_game)
	{
		delete _game;
	}
}

bool GameController::update(int ms)
{
	int updateReturn;
	bool continueStatus = false;

	switch(_state)
	{
	case MenuState:
		updateReturn = _menus->update(ms);
		if(updateReturn == MSSTATUS_GAME)
		{
			int mapStatus = _game->setMap(Map::loadMap(_menus->getSelectedMap()));
			
			if(mapStatus == MAP_OK)
			{
				_state = Game;
			}
			else
			{
				//TODO: Handle map errors
				switch (mapStatus)
				{
				case MAP_OK:
					//What
					break;

				case MAP_ERROR_PLAYER_NUMBERS:
					logError("Error: Loaded map player numbers do not match up");
					break;

				case MAP_ERROR_INVALID_MAP:
					logError("Error: Invalid map loaded");
					break;

				case MAP_MATCH_OVER:
					logError("Error: Map match state invalid");
					break;

				case MAP_INACTIVE:
					logError("Error: Map inactive error");
					break;

				default:
					logError("Error: Unknown map set error");
					break;
				}
			}

			continueStatus = true;
		}
		else if(updateReturn == MSSTATUS_EXIT)
		{
			continueStatus = false;
		}
		else if(updateReturn == MSSTATUS_CONTINUE)
		{
			continueStatus = true;
		}
		break;

	case Game:
		updateReturn = _game->update(ms);
		if(updateReturn == GSSTATUS_END)
		{
			_state = MenuState;
		}
		continueStatus = true;
		break;

	default:				//Invalid state.
		::MessageBoxA(NULL, "Invalid state 1", "Critical Error", MB_OK);
		return false;
	}

	return continueStatus;
}

void GameController::draw()
{
	switch(_state)
	{
	case MenuState:
		_menus->draw();
		break;

	case Game:
		_game->draw();
		break;

	default:				//Invalid state.
		::MessageBoxA(NULL, "Invalid state 2", "Critical Error", MB_OK);
		break;
	}
}