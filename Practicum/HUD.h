#pragma once
#include "StdAfx.h"
#include "InputManager.h"
#include "Unit.h"
#include "Tile.h"
#include "Camera.h"
#include "Map.h"
#include "Menu.h"

#define HUD_END_TURN 123
#define HUD_EXIT_GAME 543
#define HUD_BUILD_UNIT 5234
#define HUD_MOVE_UNIT 654
#define HUD_ATTACK_UNIT 876
#define HUD_CAPTURE_TILE 899

class HUD
{
private:
	Unit* _unitSelected;
	Tile* _factorySelected;
	Tile** _selectedTile;
	Map* _map;

	std::set<Tile*> _highlightedTiles;
	int _highlightType;

	Vector2 _curTilePosition;
	std::vector<std::pair<char, int>> _priceVec;	//For use in factory menu
	bool _mainMenuOpen, _infoMenuOpen, _isFactorySelected, _unitMenuOpen, _gameOver, _attackMenuOpen;
	size_t _toBuild;

	std::unique_ptr<Menu> _mainMenu, _factoryMenu, _unitMenu, _attackMenu, _unitCaptureMenu, _attackCaptureMenu;

#ifdef DEBUG_MODE
	bool _debugOn, _paused;
	int _view;
	Tile* _targetTile;
#endif

	static HUD* _gameHUD;

	HUD();

public:

	static HUD* getHUD()
	{
		return _gameHUD;
	}

	char getToBuild() { return _priceVec[_toBuild].first; }

	void setUnitTemplates();

	void reset()
	{
		_mainMenuOpen = _infoMenuOpen = _isFactorySelected = _unitMenuOpen = _attackMenuOpen = false;
		_factorySelected = 0;
		_unitSelected = 0;
		_selectedTile = 0;
		_map = 0;
		_view = 0;
		_debugOn = false;
		_paused = false;
		_targetTile = 0;
		_gameOver = false;
		_highlightType = 0;
	}

	void highlightMoveTiles(std::set<Tile*>& moveTiles) { _highlightedTiles = moveTiles; _highlightType = 1; }

	void highlightAttackTiles(std::set<Tile*>& attackTiles) { _highlightedTiles = attackTiles; _highlightType = 2; }

	void unhighlightTiles() { _highlightType = 0; }

	void setMap(Map* map)
	{
		_map = map;
	}

	void setSelectedTileRef(Tile** selectedTile)
	{
		_selectedTile = selectedTile;
	}

	void setSelectedFactory(Tile* factory)
	{
		_factorySelected = factory;
		_isFactorySelected = factory != 0;	//False if no factory is selected (0)
		_factoryMenu->setSelectedOption(0);
	}

	void setSelectedUnit(Unit* unit)
	{
		_unitSelected = unit;
		_unitMenuOpen = unit != 0;	//False if no unit is selected (0)
		_unitMenu->setSelectedOption(0);
		_unitCaptureMenu->setSelectedOption(0);
	}

	void openMainMenu()
	{
		_mainMenuOpen = true;
		_mainMenu->setSelectedOption(0);
	}

	void openAttackMenu()
	{
		_attackMenuOpen = true;
		_attackMenu->setSelectedOption(0);
		_attackCaptureMenu->setSelectedOption(0);
	}

	bool isMenuOpen()
	{
		return _mainMenuOpen || _isFactorySelected || _unitMenuOpen || _attackMenuOpen;
	}

	int handleAttackMenu();

	int handleMainMenu();

	int handleFactory();

	int handleUnit();

	int update();

	void setGameOver(bool gameOver) { _gameOver = gameOver; }

#ifdef DEBUG_MODE
	void toggleDebug(bool debug) { _debugOn = debug; if (!_debugOn) _view = 0; }
	void toggleView(int view) { _view = view; }
	void togglePaused(bool paused) { _paused = paused; }
	void setTargetTile(Tile* tile) { _targetTile = tile; }
#endif

	void draw(size_t money);
};