#include "StdAfx.h"
#include "GameSystem.h"
//#define MAP_UPDATE_OFF

GameSystem::GameSystem(): _player(new Player(true)), _AI(new AI()), _unitSelected(0), _map(0), _factorySelected(0), _gameMenuOpen(false), _HUD(HUD::getHUD())
{
	_HUD->setSelectedTileRef(&_selectedTile);
	_curPath = new std::vector<Tile*>();
}

int GameSystem::setMap(Map* map)
{
	_HUD->reset();
	_map = map;
	_unitSelected = 0;
	_gameMenuOpen = false;
	_factorySelected = 0;
	_debugOn = false;
	Camera::setPosition(0, 0);
	_movingUnit = false;
	_attackingUnit = false;
	if(_map)
	{
		_map->addPlayer(_player, 0);
		_map->addPlayer(_AI, 1);
		_AI->setMap(_map);
		_map->beginMatch();
		
		if(_map->getStatus() == MAP_OK)
		{
			_selectedTile = _map->getTile(0, 0);
			if(!_selectedTile)
			{
				return MAP_ERROR_INVALID_MAP;
			}
			_HUD->setSelectedTileRef(&_selectedTile);
		}
		_HUD->setMap(_map);
		_HUD->setUnitTemplates();

		return _map->getStatus();
	}

	return -1;
}

void GameSystem::handlePlayerMapInput()
{
	if (InputManager::isPressed(Select))
	{
		if (_movingUnit)
		{
			_movingUnit = false;
			_map->issueMoveCommand(_player, _unitSelected, _curPath);
			_HUD->unhighlightTiles();
			_HUD->openAttackMenu();
		}
		else if (_attackingUnit)
		{
			_attackingUnit = false;
			_map->issueAttackCommand(_player, _unitSelected, _map->getTileUnit(_player, _selectedTile));
			_HUD->unhighlightTiles();
		}
		else
		{
			//Check current tile for a unit or factory
			auto unit = _map->getTileUnit(_player, _selectedTile);

			//If there is a unit that belongs to the player on the tile, and it can still attack or move, select it. Else if the tile is a factory belonging to the player, select it.
			if (unit != 0)
			{
				//A factory that has a unit in it can not produce more units. Therefore the check for whether the unit has moved or attacked is in here.
				if (!unit->hasMoved() && !unit->hasAttacked())
				{
					_unitSelected = unit;
					_HUD->setSelectedUnit(unit);
				}
			}
			else if (_selectedTile->getType() == Factory && _selectedTile->getOwner() == _player->getID())
			{
				//_factorySelected = _selectedTile;
				_HUD->setSelectedFactory(_selectedTile);
			}
		}
	}
	else if (InputManager::isPressed(Cancel))
	{
		if (_movingUnit)
		{
			_movingUnit = false;
			_HUD->unhighlightTiles();
		}
		else if (_attackingUnit)
		{
			_attackingUnit = false;
			_HUD->unhighlightTiles();
		}
		else
		{
			_HUD->openMainMenu();
		}
	}
	else if (InputManager::isPressed(Left))
	{
		if (_selectedTile->getNeighbor(TILE_WEST))
		{
			_selectedTile = _selectedTile->getNeighbor(TILE_WEST);
			Camera::shiftCamera(1, 0);
			
			if (_movingUnit)
			{
				if (_unitSelected->canMoveTo(_selectedTile))
				{
					_curPath->push_back(_selectedTile);
					if (!_unitSelected->canTraversePath(_curPath))
					{
						_curPath = _unitSelected->getPathTo(_selectedTile);
					}
				}
			}
		}
	}
	else if (InputManager::isPressed(Right))
	{
		if (_selectedTile->getNeighbor(TILE_EAST))
		{
			_selectedTile = _selectedTile->getNeighbor(TILE_EAST);
			Camera::shiftCamera(-1, 0);

			if (_movingUnit)
			{
				if (_unitSelected->canMoveTo(_selectedTile))
				{
					_curPath->push_back(_selectedTile);
					if (!_unitSelected->canTraversePath(_curPath))
					{
						_curPath = _unitSelected->getPathTo(_selectedTile);
					}
				}
			}
		}
	}
	else if (InputManager::isPressed(Up))
	{
		if (_selectedTile->getNeighbor(TILE_NORTH))
		{
			_selectedTile = _selectedTile->getNeighbor(TILE_NORTH);
			Camera::shiftCamera(0, 1);

			if (_movingUnit)
			{
				if (_unitSelected->canMoveTo(_selectedTile))
				{
					_curPath->push_back(_selectedTile);
					if (!_unitSelected->canTraversePath(_curPath))
					{
						_curPath = _unitSelected->getPathTo(_selectedTile);
					}
				}
			}
		}
	}
	else if (InputManager::isPressed(Down))
	{
		if (_selectedTile->getNeighbor(TILE_SOUTH))
		{
			_selectedTile = _selectedTile->getNeighbor(TILE_SOUTH);
			Camera::shiftCamera(0, -1);

			if (_movingUnit)
			{
				if (_unitSelected->canMoveTo(_selectedTile))
				{
					_curPath->push_back(_selectedTile);
					if (!_unitSelected->canTraversePath(_curPath))
					{
						_curPath = _unitSelected->getPathTo(_selectedTile);
					}
				}
			}
		}
	}

#ifdef DEBUG_MODE
	if (InputManager::isPressed(TEMPENDTURN))
	{
		_map->endTurn(_player);
	}
#endif
}

void GameSystem::handleAI()
{
	if (!AIManager::isRunning())
	{
		if (!AIManager::run(_AI))
		{
			AIErrorCodes error = AIManager::getError();
			switch (error)
			{
			case OK:
				::MessageBoxA(NULL, "Error: No error", "Error", MB_OK);
				_map->endTurn(_AI);
				break;

			case NullAI:
				::MessageBoxA(NULL, "Error: Null AI", "Error", MB_OK);
				break;

			case MultipleAI:
				::MessageBoxA(NULL, "Error: Another AI already running", "Error", MB_OK);
				break;

			default:
				::MessageBoxA(NULL, "Error: ", "Error", MB_OK);
				_map->endTurn(_AI);
				break;
			}
		}
	}
}

#ifdef DEBUG_MODE
void GameSystem::handleDebug()
{
	if (InputManager::isPressed(DebugToggle))
	{
		_debugOn = !_debugOn;
		_map->debugToggle(_debugOn);
		_AI->toggleDebugMode(_debugOn);
		_HUD->toggleDebug(_debugOn);

		if (!_debugOn)
		{
			_HUD->setTargetTile(0);
		}
	}

	if (_debugOn)
	{
		if (InputManager::isPressed(DebugSwitchView1))
		{
			_map->debugToggleView(0);
			_HUD->toggleView(0);
		}

		if (InputManager::isPressed(DebugSwitchView2))
		{
			_map->debugToggleView(1);
			_HUD->toggleView(1);
		}

		if (InputManager::isPressed(DebugSwitchViewAll))
		{
			_map->debugToggleView(2);
			_HUD->toggleView(2);
		}

		if (InputManager::isPressed(DebugToggleAIPause))
		{
			_AI->togglePause(!_AI->debugIsPaused());
			_HUD->togglePaused(_AI->debugIsPaused());
		}

		if (InputManager::isPressed(DebugAINextStep))
		{
			_AI->debugNextStep();
		}

		_HUD->setTargetTile(_AI->debugGetTargetTile());
	}
}
#endif

/*
	Updates the game.

*/
int GameSystem::update(int ms)
{
	if(!_map)
	{
		::MessageBoxA(NULL, "Null map", "Critical Error", MB_OK);
		logError("GS: Null Map Error");
		return GSSTATUS_END;
	}
#ifndef MAP_UPDATE_OFF
	int mapStatus;

	//If the map is still active (at least 2 players alive)
	if(_map->isActive())
	{

#ifdef DEBUG_MODE
		handleDebug();
#endif

		//If its the player's turn, handle player input. Otherwise, let the AI run.
		if(_map->getCurrentPlayer() == _player->getID())
		{
			if (_HUD->isMenuOpen())	//If a menu is open, let HUD handle updating those.
			{
				int result = _HUD->update();
				std::set<Tile*> tiles;
				Tile* curTile;
				switch (result)
				{
				case 0:
					break;

				case HUD_CAPTURE_TILE:
					_map->issueCaptureCommand(_player, _unitSelected, _unitSelected->getPosition());
					break;

				case HUD_ATTACK_UNIT:
					
					for (int i = 0; i < TILE_N_NEIGHBORS; i++)
					{
						curTile = _unitSelected->getPosition()->getNeighbor(i);
						if (curTile && (!_map->getTileUnit(_player, curTile) || _map->getTileUnit(_player, curTile)->getOwner() != _player->getID()))
						{
							tiles.insert(_unitSelected->getPosition()->getNeighbor(i));
						}
					}
					_HUD->highlightAttackTiles(tiles);
					_attackingUnit = true;
					break;

				case HUD_MOVE_UNIT:
					_HUD->highlightMoveTiles(_unitSelected->getValidTiles());
					_movingUnit = true;
					_curPath->clear();
					_curPath->push_back(_unitSelected->getPosition());
					break;

				case HUD_BUILD_UNIT:
					_map->issueBuildCommand(_player, _HUD->getToBuild(), _selectedTile);
					break;

				case HUD_END_TURN:
					_map->endTurn(_player);
					break;

				case HUD_EXIT_GAME:
					return GSSTATUS_END;
				}
			}
			else						//else handle general map traversal input.
			{
				handlePlayerMapInput();
			}
		}
		else
		{
			handleAI();
		}
	}
	else
	{
		mapStatus = _map->getStatus();
		switch(mapStatus)
		{
		case MAP_OK:
			::MessageBoxA(NULL, "Error: Inactive Map Error", "Unexpected Error", MB_OK);
			logError("GS: Inactive Map Error");
			return GSSTATUS_END;

		case MAP_ERROR_PLAYER_NUMBERS:
			::MessageBoxA(NULL, "Error: Failed to initialize players", "Critical Error", MB_OK);
			logError("GS: MAP ERROR PLAYER NUMBERS");
			return GSSTATUS_END;

		case MAP_ERROR_INVALID_MAP:
			::MessageBoxA(NULL, "Error: Invalid Map", "Critical Error", MB_OK);
			logError("GS: MAP ERROR INVALID MAP");
			return GSSTATUS_END;

		case MAP_MATCH_OVER:
			//TODO: Display end of match message
			_HUD->setGameOver(true);
			if(InputManager::isPressed(Select))
			{
				return GSSTATUS_END;
			}
			break;

		default:
			::MessageBoxA(NULL, "Unexpected Map Status", "Critical Error", MB_OK);
			logError("GS: INVALID ERROR CODE");
			return GSSTATUS_END;
		};
	}
#endif
	return GSSTATUS_CONTINUE;
}

void GameSystem::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(_map)
	{
		_map->draw();
	}

	if (_HUD)
	{
		_HUD->draw(_player->getFunds());
	}

	glFlush();
	glutSwapBuffers();
}