#pragma once
#include "StdAfx.h"
#include <fstream>
#include <tuple>
#include "Unit.h"
#include "Tile.h"
#include "Player.h"
#include "AssetPool.h"
#include "Matrix.h"
#include "Camera.h"

#define MAP_OK 0
#define MAP_ERROR_PLAYER_NUMBERS 1
#define MAP_ERROR_INVALID_MAP 2
#define MAP_MATCH_OVER 3
#define MAP_INACTIVE 4

#define MAP_TEMPLATE_UNIT_BYTE 'u'

struct MapTemplate;

class Map
{
private:

	struct MapTemplate
	{
		bool isValid;	//Whether the map template is valid or not
		int width;		//Width of the map
		int height;		//Height of the map
	
		std::vector<std::vector<std::tuple<TerrainType, int>>> map;	//The tiles and owners of tiles (if any)

		std::set<int> playerIDs;	//Set of Player IDs

		std::vector<std::tuple<char, int, int, int>> units;	//Any units that should be created on map creation, stored in the following order: Unit Template Identifier, X position, Y position, Owner ID
		
		MapTemplate(std::string fileName);
	};

	static std::map<char, TerrainType> _terrainMap;			//For parsing map files.
	static std::map<std::string, MapTemplate*> _maps;		//Maps that have been loaded.
	static std::map<TerrainType, std::string> _textureMap;	//Maps TerrainTypes to textures for rendering
	static bool _isInitialized;

	std::vector<std::vector<Tile*>> _tiles;			//Map tiles
	std::vector<std::map<Tile*, int>> _visionMaps;	//Stores which tiles can be visible by which player and by how many units. This is to determing which tiles can be seen and still be seen by each player after a unit moves.
	std::vector<std::set<Unit*>> _visibleUnits;		//Stores which units are visible by which player.
	std::map<Tile*, Unit*> _positionMap;			//Stores which map tiles are being occupied.
	std::vector<std::set<Tile*>> _buildingsByPlayer;//Stores which player owns which building.
	std::set<Tile*> _buildings;						//Set of all buildings
	bool _isValid, _isActive;
	int _width, _height, _nUnits, _nTiles, _nPlayers, _curPlayer, _turnNumber;
	std::vector<Player*> _players;					//Players
	std::vector<std::set<Unit*>> _unitsByPlayer;	//Units owned by each player

	int _status;

#ifdef DEBUG_MODE
	int _view;
	bool _debugOn;
#endif

	std::set<int> _playerIDs;

	void changeTurn(Player* curPlayer);
	
	explicit Map(std::string fileName);	//Create a map from the specified file.

	Map(MapTemplate& original);	//Creates a copy of the map.


public:
	static void initialize();

	//Returns a map based on the map template specified by fileName. Returns null if the map is invalid.
	static Map* loadMap(std::string fileName);

	static void destroyMaps();

	//Returns whether the map is valid or not.
	bool isValid()	{	return _isValid;	}

	//Returns whether the map is active or not. (At least 2 opposing players are not eliminated)
	bool isActive()	{	return _isActive;	}

	//Begins the match
	int beginMatch();

	//Returns map status
	int getStatus()	{	return _status;	}

	int getHeight()	{	return _height;	}

	int getWidth() {	return _width;	}

	bool isOccupied(Tile* tile);

	//Returns a unit if the player can see that tile.
	Unit* getTileUnit(Player* player, Tile* tile)
	{
		if (_visionMaps[player->getID()][tile] != 0)
		{
			return _positionMap[tile];
		}
		return 0;
	}

	//Adds a player to the map. Returns false if player is null or can not be added.
	bool addPlayer(Player* player, int teamNumber);

	//Attempts to move the unit along the path specified. Returns true if the unit moves to the end, returns false otherwise.
	bool issueMoveCommand(Player* owner, Unit* unit, std::vector<Tile*>* path);


	bool issueCaptureCommand(Player* owner, Unit* unit, Tile* target);


	bool issueAttackCommand(Player* owner, Unit* attacker, Unit* target);


	bool issueBuildCommand(Player* owner, char unitType, Tile* location);

	void endTurn(Player* player);

	int getCurrentPlayer()	{	return _curPlayer;	}

	void draw();

	void findValidTiles(Unit* unit);

	Tile* getTile(int x, int y);

	std::vector<std::vector<Tile*>>& getTiles() { return _tiles; }

	std::map<Tile*, int>& getVisibleTiles(Player* player);

	std::set<Unit*>& getKnownUnits(Player* player);

	std::set<Unit*>& getOwnUnits(Player* player);

	std::set<Tile*>& getOwnBuildings(Player* player);

	std::set<Tile*>& getBuildings() {	return _buildings;	}

#ifdef DEBUG_MODE
	static void debugPrint(std::string mapName);
	void debugPrint();
	void debugToggleView(int view) { if(_debugOn) _view = view; }
	void debugToggle(bool on) { _debugOn = on; if (!_debugOn) { _view = 0; } }
#endif
};