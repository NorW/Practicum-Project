#include "StdAfx.h"
#include "Map.h"

std::map<char, TerrainType> Map::_terrainMap;
std::map<std::string, Map::MapTemplate*> Map::_maps;
std::map<TerrainType, std::string> Map::_textureMap;
bool Map::_isInitialized = false;

//Sets up map textures and stuff
void Map::initialize()
{
	if(!_isInitialized)
	{
		_terrainMap.clear();
		_terrainMap['c'] = City;
		_terrainMap['f'] = Factory;
		_terrainMap['r'] = Road;
		_terrainMap['g'] = Grass;
		_terrainMap['h'] = Hill;
		_terrainMap['m'] = Mountain;
		_terrainMap['s'] = Swamp;

		_textureMap.clear();
		_textureMap[City] = "City";
		_textureMap[Factory] = "Factory";
		_textureMap[Road] = "Road";
		_textureMap[Grass] = "Grass";
		_textureMap[Hill] = "Hill";
		_textureMap[Mountain] = "Mountain";
		_textureMap[Swamp] = "Swamp";

		AssetPool::loadTexture("Assets/City.png", "City");
		AssetPool::loadTexture("Assets/Factory.png", "Factory");
		AssetPool::loadTexture("Assets/Road.png", "Road");
		AssetPool::loadTexture("Assets/Grass.png", "Grass");
		AssetPool::loadTexture("Assets/Hill.png", "Hill");
		AssetPool::loadTexture("Assets/Mountain.png", "Mountain");
		AssetPool::loadTexture("Assets/Swamp.png", "Swamp");

		_isInitialized = true;
	}
}

//Returns a new map based on the map template specified by fileName. Returns null if the file can not be parsed as a map.
Map* Map::loadMap(std::string fileName)
{
	//If the map has already been loaded, return it
	if(_maps.find(fileName) != _maps.end())
	{
		return new Map(*_maps[fileName]);
	}
	else
	{
		MapTemplate* newMap = new MapTemplate(fileName);	//Create a new map from the file
		if(newMap->isValid)				//If the map is valid, return it.
		{
			_maps[fileName] = newMap;
			return new Map(*_maps[fileName]);
		}
	}

	//Return null if the map is invalid
	return 0;
}

//Read in a map template.
Map::MapTemplate::MapTemplate(std::string fileName)
{
	if(!_isInitialized)
	{
		Map::initialize();
	}

	if (!Unit::isInitialized())
	{
		Unit::initialize();
	}

	isValid = true;
	width = height = 0;
	std::ifstream ifs;

	ifs.open(fileName);		//Try to open the file

	if(ifs.fail())			//If file fails to open, it is not a valid map.
	{
		isValid = false;
	}
	else
	{
		int nTiles = 0,		//Number of tiles
			ownerID = 0,	//ID of owner of current tile
			curXPos = 0,	//Current x position
			curYPos = 0;	//Current y position

		char curVal = 0;	//Current character
		TerrainType curType;	//Type of current tile

		ifs >> width >> height;		//Read in width followed by height
		nTiles = width * height;	//Determine number of tiles in map

		if(nTiles <= 1)			//If the map has only 1 tile or less, it is invalid. (A valid map must have space for at least 2 players.)
		{
			isValid = false;
		}
		else
		{
			//Populate the map vector.
			
			map.reserve(height);	//Reserve space ahead of time
			
			for(int i = 0; i < height; i++)
			{
				map.emplace_back();		//Create new vector and push back
				map[i].reserve(width);	//Reserve space ahead of time
			}

			//While there are still tiles to read in
			while(nTiles > 0)
			{
				ifs >> curVal;			//Read in a character

				if(ifs.eof())			//If end of file is reached before n tiles have been found, the map is invalid. (Wrong height/width)
				{
					isValid = false;
					break;
				}
				
				if(_terrainMap.find(curVal) == _terrainMap.end())	//If the character is invalid, skip.
				{
					continue;
				}

				curType = _terrainMap[curVal];	//Get the TerrainType of the tile
				
				//If the tile is a city of factory, check for an owner.
				if(curType == City || curType == Factory)
				{
					ifs >> ownerID;	//Get ownerID

					//If an ID fails to be read in, assume the tile is unowned.
					if(ifs.fail())
					{
						map[curYPos].emplace_back(curType, TILE_UNOWNED);
						ifs.clear();	//Clear the fail/bad/eof bits of the ifstream. This loop should end before EOF is reached.
					}
					else
					{
						map[curYPos].emplace_back(curType, ownerID);

						//Attempt to add owner ID to the playerIDs set.
						if(ownerID != TILE_UNOWNED)
						{
							playerIDs.insert(ownerID);
						}
					}
				}
				else
				{
					map[curYPos].emplace_back(curType, TILE_UNOWNED);
				}

				//If the next character is the MAP_TEMPLATE_UNIT_BYTE
				if (ifs.peek() == MAP_TEMPLATE_UNIT_BYTE)
				{
					ifs.ignore(1);

					ifs >> curVal >> ownerID;	//Get the unit type

					if (!ifs.fail())
					{
						units.emplace_back(curVal, curXPos, curYPos, ownerID);
					}
					else if(!ifs.eof())
					{
						ifs.clear();
					}
				}

				//X and Y position checks.
				if(++curXPos >= width)
				{
					curXPos = 0;

					if(++curYPos >= height)
					{
						break;
					}
				}

				--nTiles;	//Decrement number of tiles.
			}

			if(playerIDs.size() < 2)	//Must have at least 2 players for a map to be valid
			{
				isValid = false;
			}
		}
	}
}

void Map::destroyMaps()
{
	_maps.clear();
}


//Creates a map from the maptemplate "original"
Map::Map(MapTemplate& original) : _isValid(original.isValid), _turnNumber(0), _isActive(false), 
									_width(original.width), _height(original.height), _nPlayers(original.playerIDs.size())
{
#ifdef DEBUG_MODE
	_view = 0;
	_debugOn = false;
	_view = 0;
#endif

	if(_isValid)
	{
		_playerIDs = original.playerIDs;
		_tiles.clear();							//Clear Tiles
		_tiles.reserve(original.map.size());

		_visionMaps.clear();		//Clear Vision Maps
		_visibleUnits.clear();		//Clear Visibile Units
		_unitsByPlayer.clear();		//Clear Units

		_visionMaps.reserve(_nPlayers);
		_visibleUnits.reserve(_nPlayers);
		_unitsByPlayer.reserve(_nPlayers);
		_buildingsByPlayer.reserve(_nPlayers);

		for (int i = 0; i < _nPlayers; i++)
		{
			_visionMaps.emplace_back();
			_visibleUnits.emplace_back();
			_unitsByPlayer.emplace_back();
			_buildingsByPlayer.emplace_back();
		}

		//Set tiles, _positionMap, 
		for(size_t y = 0; y < original.map.size(); y++)
		{
			_tiles.emplace_back();

			for(size_t x = 0; x < original.map[y].size(); x++)
			{
				//Create a new tile. Since loading is only done once a game, making a large amount of tiles isnt too much of an issue. Object pools are a possible fix for slow loading times, but not very optimal unless many large games are played in the same session.
				_tiles[y].push_back(new Tile(std::get<0>(original.map[y][x]), x, y, std::get<1>(original.map[y][x])));
				_positionMap[_tiles[y][x]] = 0;
				
				for (size_t i = 0; i < _visionMaps.size(); i++)
				{
					//If the tile is owned by the player, it is visible.
					if (_tiles[y][x]->getOwner() == i)
					{
						_visionMaps[i][_tiles[y][x]] = 1;
						_buildingsByPlayer[i].insert(_tiles[y][x]);
					}
					else
					{
						_visionMaps[i][_tiles[y][x]] = 0;
					}
				}

				//Add to buildings tile
				if (_tiles[y][x]->getType() == TerrainType::City || _tiles[y][x]->getType() == TerrainType::Factory)
				{
					_buildings.insert(_tiles[y][x]);
				}

				//Set neighbors
				if(y > 0)
				{
					_tiles[y][x]->setNeighbor(TILE_NORTH, _tiles[y - 1][x]);
					_tiles[y - 1][x]->setNeighbor(TILE_SOUTH, _tiles[y][x]);
				}

				if(x > 0)
				{
					_tiles[y][x]->setNeighbor(TILE_WEST, _tiles[y][x-1]);
					_tiles[y][x - 1]->setNeighbor(TILE_EAST, _tiles[y][x]);
				}
			}
		}

		//Get unit positions
		Unit* temp;
		int owner;
		Tile* curTile;
		std::vector<Tile*> unitVision;
		for (auto it : original.units)
		{
			owner = std::get<3>(it);
			curTile = _tiles[std::get<2>(it)][std::get<1>(it)];
			if (temp = Unit::createUnit(std::get<0>(it), curTile, owner, 9999999))
			{
				//Set owner stuff
				_positionMap[curTile] = temp;						//Set position map
				_unitsByPlayer[owner].insert(temp);					//Add unit to owner's set of units

				unitVision = temp->getVision();						//Get unit vision

				for (size_t i = 0; i < unitVision.size(); i++)		//Add unit vison to owner's vison map
				{
					++_visionMaps[owner][unitVision[i]];
				}
				temp->procTurnEnd(0);
			}
		}

		//Insert units into vison maps (now that vision maps are complete).
		for (int unitVector = 0; unitVector < _nPlayers; unitVector++)
		{
			for (auto unit : _unitsByPlayer[unitVector])
			{
				for (int otherVector = 0; otherVector < _nPlayers; otherVector++)
				{
					if (unitVector == otherVector)
					{
						continue;
					}

					if (_visionMaps[otherVector][unit->getPosition()] > 0)
					{
						_visibleUnits[otherVector].insert(unit);
					}
				}

				findValidTiles(unit);
			}
		}

		temp = 0;
		curTile = 0;
	}
}


//Set the id for the player passed in. Returns false if player is null or there are no more valid IDs to hand out (no more players allowed).
bool Map::addPlayer(Player* player, int teamNumber)
{
	if(!player)
	{
		return false;
	}

	_players.push_back(player);
	
	size_t index = _players.size() - 1;

	if(index >= _playerIDs.size())
	{
		_players.pop_back();
		return false;
	}

	std::set<int>::iterator it = _playerIDs.begin();

	for(size_t i = 0; i != index; i++, ++it);

	player->setID(*it);
	player->setFunds(1000);

	return true;
}

bool Map::isOccupied(Tile* tile)
{
	if (_positionMap[tile] != 0)
	{
		return true;
	}
	return false;
}

int Map::beginMatch()
{
	//The map must be valid.
	if(!_isValid)
	{
		_status = MAP_ERROR_INVALID_MAP;
		return MAP_ERROR_INVALID_MAP;
	}

	//The map must have the correct number of players to begin
	if(_nPlayers != _players.size())
	{
		_status = MAP_ERROR_PLAYER_NUMBERS;
		return MAP_ERROR_PLAYER_NUMBERS;
	}

	_status = MAP_OK;
	_isActive = true;
	_curPlayer = 0;
	return 0;
}

/*
	Send a command to the map from the Player "owner" to move the specified unit along the specified path.
	Returns true if the unit has been moved.
	Returns false if the unit can not be moved.

	A unit can be moved if and only if it's currently the unit's owner's turn.
	A unit can only be moved by its owner.
*/
bool Map::issueMoveCommand(Player* owner, Unit* unit, std::vector<Tile*>* path)
{
	if(!unit || !owner || !path || path->size() == 0)
	{
		return false;
	}

	//The unit can only be moved by its owner on the owner's turn.
	if(owner != _players[_curPlayer] || unit->getOwner() != owner->getID())
	{
		return false;
	}

	if (unit->hasMoved())
	{
		return false;
	}
	bool encounteredUnit = false;

	Tile* prevPosition = unit->getPosition(), *newPosition;

	for(std::vector<Tile*>::iterator it = path->begin(); it != path->end(); it++)
	{

		if (*it == unit->getPosition())
		{
			continue;
		}
		//If an enemy unit occupies a tile the player's unit is going to move into, stop there
		if(_positionMap[*it] != 0 && _positionMap[*it]->getOwner() != unit->getOwner())
		{
			if(it != path->begin())
			{
				unit->move(*(it - 1));
			}
			else
			{
				unit->move(unit->getPosition());
			}

			encounteredUnit = true;
			break;
		}
	}

	if(!encounteredUnit)
	{
		unit->move(*(path->end() - 1));
	}

	std::vector<Tile*> prevVision, newVision;
	newPosition = unit->getPosition();

	//If the unit has actually moved
	if(prevPosition != newPosition)
	{
		//Set position map
		_positionMap[prevPosition] = 0;
		_positionMap[unit->getPosition()] = unit;

		//Reset and recalculate vision
		prevVision = unit->getVision();
		unit->recalcualteVision();

		//Update valid tiles
		for (auto unitVector : _unitsByPlayer)
		{
			for (auto mapUnit : unitVector)
			{
				if (Tile::getDistance(prevPosition, mapUnit->getPosition()) <= mapUnit->getMoveDist()||
					Tile::getDistance(newPosition, mapUnit->getPosition()) <= mapUnit->getMoveDist())
				{
					findValidTiles(mapUnit);
				}
			}
		}

		newVision = unit->getVision();

		
		//Add new vision
		for (auto it : newVision)
		{
			if (++_visionMaps[_curPlayer][it] == 1)
			{
				if (_positionMap[it] != 0)
				{
					_visibleUnits[_curPlayer].insert(_positionMap[it]);
				}
			}
		}
		
		//Subtract previous vision and check if they are still visible
		for (auto it : prevVision)
		{
			if (--_visionMaps[_curPlayer][it] == 0 &&	//If the tile can no longer be seen
				_positionMap[it] != 0)					//and if a unit occupies it, remove the unit form vision map
			{
				_visibleUnits[_curPlayer].erase(_positionMap[it]);
			}
		}

		//Update other vision maps
		for (size_t i = 0; i < _visionMaps.size(); i++)
		{
			if (i == _curPlayer)
			{
				continue;
			}

			if (_visionMaps[i][newPosition] > 0)
			{
				_visibleUnits[i].insert(unit);
			}
			else
			{
				_visibleUnits[i].erase(unit);
			}
		}
	}
	return !encounteredUnit;
}

/*

*/
bool Map::issueCaptureCommand(Player* owner, Unit* unit, Tile* target)
{
	if(!unit || !owner || !target)
	{
		return false;
	}

	//Only the current player can issue commands
	if(owner != _players[_curPlayer])
	{
		return false;
	}

	if(unit->getPosition() == target)
	{
		auto side = target->getOwner();

		if (target->capture(owner->getID(), unit->captureAmount()))
		{
			_buildingsByPlayer[side].erase(target);
			_buildingsByPlayer[target->getOwner()].insert(target);
			
			if (--_visionMaps[side][target] == 0)
			{
				_visibleUnits[side].erase(unit);
			}

			if (_buildingsByPlayer[side].size() == 0)
			{
				//TODO: remove player from game
			}
		}
	}
	return true;
}

bool Map::issueAttackCommand(Player* owner, Unit* attacker, Unit* target)
{
	if(!attacker || !owner || !target)
	{
		logError("M.A: null attack.");
		return false;
	}

	//Only the current player can issue commands to their own units
	if(owner != _players[_curPlayer] || owner->getID() != attacker->getOwner())
	{
		logError("M.A: Offside attack.");
		return false;
	}

	//Friendly fire off
	if(attacker->getOwner() == target->getOwner())
	{
		logError("M.A: Friendly Fire.");
		return false;
	}

	//Distance check
	if(Vector2::manhattanDistance(attacker->getPosition()->getPosition(), target->getPosition()->getPosition()) > 1)
	{
		logError("M.A: Distance Check Fail.");
		return false;
	}

	//attack unit
	if(!Unit::handleAttack(attacker, target))	//If the target is no longer alive, remove it from all unit storage structures.
	{
		//destroy target
		_positionMap[target->getPosition()] = 0;	//Remvoe from position map

		auto visibleTiles = target->getVision();
		auto targetOwner = target->getOwner();
		for (auto tile : visibleTiles)
		{
			if (--_visionMaps[targetOwner][tile] == 0 && _positionMap[tile] != 0)
			{
				_visibleUnits[targetOwner].erase(_positionMap[tile]);
			}
		}

		std::set<Unit*>::iterator it;

		//Remove unit from visible units sets
		for(int i = 0; i < _nPlayers; i++)
		{
			it = _visibleUnits[i].find(target);
			if(it != _visibleUnits[i].end())
			{
				_visibleUnits[i].erase(it);
			}
		}

		_unitsByPlayer[target->getOwner()].erase(target);	//Remove unit from player's set of units


		//Update valid tiles
		for (auto unitVector : _unitsByPlayer)
		{
			for (auto mapUnit : unitVector)
			{
				if (Tile::getDistance(target->getPosition(), mapUnit->getPosition()) <= mapUnit->getMoveDist())
				{
					findValidTiles(mapUnit);
				}
			}
		}


		//Check if the unit was the last enemy unit
		int nActivePlayers = 0;

		for(int i = 0; i < _nPlayers; i++)
		{
			if(_unitsByPlayer[i].size() > 0)
			{
				nActivePlayers++;
			}
		}

		//Only 1 player remains.
		if(nActivePlayers < 2)
		{
			_isActive = false;
			_status = MAP_MATCH_OVER;
		}
	}
	return true;
}

bool Map::issueBuildCommand(Player* owner, char unitType, Tile* location)
{
	if(!owner || !location)
	{
		return false;
	}

	//Only the current player can issue commands. If the tile is not owned by the current player or the tile isnt a factory, return.
	if(owner != _players[_curPlayer] || location->getOwner() != owner->getID() 
		|| location->getType() != Factory || _positionMap[location] != 0)
	{
		return false;
	}

	//Attempt to create unit
	Unit* newUnit = Unit::createUnit(unitType, location, owner->getID(), owner->getFunds());

	if(newUnit)
	{
		owner->requestFunds(Unit::unitTemplates[unitType].unitCost);	//Deduct unit costs from player
		_positionMap[location] = newUnit;								//Set position map
		_unitsByPlayer[_curPlayer].insert(newUnit);						//Add unit to owner's set of units
	
		std::vector<Tile*> unitVision = newUnit->getVision();		//Get unit vision

		for(size_t i = 0; i < unitVision.size(); i++)				//Add unit vison to owner's vison map
		{
			if (++_visionMaps[_curPlayer][unitVision[i]] == 1 && _positionMap[unitVision[i]] && _positionMap[unitVision[i]]->getOwner() != owner->getID())
			{
				_visibleUnits[owner->getID()].insert(_positionMap[unitVision[i]]);
			}
		}

		for(size_t i = 0; i < _visibleUnits.size(); i++)				//Add unit to other player's vision map if visible.
		{
			if(i == owner->getID())
			{
				continue;
			}

			if(_visionMaps[i][location] > 0)
			{
				_visibleUnits[i].insert(newUnit);
			}
		}

		for (auto player : _unitsByPlayer)
		{
			for (auto unit : player)
			{
				if (Tile::getDistance(location, unit->getPosition()) <= unit->getMoveDist())
				{
					findValidTiles(unit);
				}
			}
		}

		return true;
	}
	return false;
}

void Map::endTurn(Player* player)
{
	//Only the current player can end their turn.
	if(player != _players[_curPlayer])
	{
		return;
	}

	player->addFunds(100 * _buildingsByPlayer[player->getID()].size());

	TerrainType terrain;

	//For each unit owned by the player, if the unit is on a factory or city owned by the current player, attempt to repair it.
	for(std::set<Unit*>::iterator it = _unitsByPlayer[_curPlayer].begin(); it != _unitsByPlayer[_curPlayer].end(); ++it)
	{
		terrain = (*it)->getPosition()->getType();

		if((*it)->getPosition()->getOwner() == player->getID() && (terrain == City || terrain == Factory))
		{
			(*it)->procTurnEnd(_players[_curPlayer]->requestFunds((*it)->getRepairCost()));
		}
		else
		{
			(*it)->procTurnEnd(0);
		}
	}

	changeTurn(player);
}

void Map::changeTurn(Player* player)
{
	_curPlayer++;

	if(_curPlayer >= _nPlayers)
	{
		_curPlayer = 0;
		_turnNumber++;
	}

	if(_players[_curPlayer] == player)
	{
		//This is the only player that still has units
		_isActive = false;
		_turnNumber--;
		return;
	}

	//If the next player's turn has no units, go to the next player.
	if(_turnNumber && _unitsByPlayer[_curPlayer].size() == 0)
	{
		changeTurn(player);
	}
}

void Map::findValidTiles(Unit* unit)
{
	//Unit must exist and be alive to have valid tiles.
	if (!unit || !unit->isAlive())
	{
		return;
	}

	std::deque<Tile*> openNodes;					//Open nodes to traverse.
	std::map<Tile*, Tile*> tileOrigins;				//Tile origins. Also doubles as a closed nodes list.
	std::map<Tile*, int> tileDistancesFromStart;	//Stores distances from origin (unit position)
	std::set<Tile*> attackableTiles;
	std::set<Tile*> validTiles;

	openNodes.push_back(unit->getPosition());
	tileOrigins[openNodes[0]] = openNodes[0];
	tileDistancesFromStart[openNodes[0]] = 0;
	
	Tile* curNode;		//Current node being processed.
	Tile* curNeighbor;	//Current neighbor being processed.
	int curDist;
	validTiles.insert(openNodes[0]);

	auto owner = unit->getOwner();

	while (!openNodes.empty())
	{
		//Get next node.
		curNode = openNodes[0];
		openNodes.pop_front();

		for (int i = 0; i < TILE_N_NEIGHBORS; i++)
		{
			curNeighbor = curNode->getNeighbor(i);

			//If the current neighbor is null, continue. Else if curNeighbor is occupied by an enemy unit, add it to attackable list and continue.
			if (!curNeighbor)
			{
				continue;
			}
			else if (_positionMap[curNeighbor] && _positionMap[curNeighbor]->getOwner() != owner)
			{
				attackableTiles.insert(curNeighbor);
				continue;
			}

			curDist = tileDistancesFromStart[curNode] + unit->getMoveCost(curNeighbor);

			/*
			If the neighboring tile has been found before and the neighboring node is closer to the unit through curNode
			than its previous originating node, curNeighbor now originates from curNode (because of the shorter distance).
			This is so that the unit can determine all nodes it can travel to.

			Else if the neighboring tile has never been encountered and the distance to the neighbor through curNode is within
			the unit's max move distance, add the neighbor to open nodes, and set the origin and distance values. Add to valid nodes only if the node is empty.

			Else if the neighboring tile can not be reached (1 tile away), add it to the set of attackable tiles.
			*/
			if (tileOrigins.find(curNeighbor) != tileOrigins.end() && tileDistancesFromStart[curNeighbor] > curDist)
			{
				tileOrigins[curNeighbor] = curNode;
				tileDistancesFromStart[curNeighbor] = curDist;
			}
			else if (tileOrigins.find(curNeighbor) == tileOrigins.end() && curDist < unit->getMoveDist())
			{
				tileOrigins[curNeighbor] = curNode;
				tileDistancesFromStart[curNeighbor] = curDist;
				attackableTiles.insert(curNeighbor);
				openNodes.push_back(curNeighbor);

				if (!_positionMap[curNeighbor])
				{
					validTiles.insert(curNeighbor);
				}
			}
			else
			{
				attackableTiles.insert(curNeighbor);
			}
		}
	}

	unit->setAttackableTiles(attackableTiles);
	unit->setValidTiles(validTiles);
	unit->setTileOrigins(tileOrigins);
}

Tile* Map::getTile(int x, int y)
{
	if(x >= _width || y >= _height || x < 0 || y < 0)
	{
		return 0;
	}

	return _tiles[y][x];
}

std::set<Unit*>& Map::getKnownUnits(Player* player)
{
	//Invalid player
	if(player->getID() >= (int)_visibleUnits.size() || player->getID() < 0)
	{
		return std::set<Unit*>();
	}
	return _visibleUnits[player->getID()];
}

std::set<Unit*>& Map::getOwnUnits(Player* player)
{
	return _unitsByPlayer[player->getID()];
}

std::map<Tile*, int>& Map::getVisibleTiles(Player* player)
{
	if(player->getID() >= (int)_visionMaps.size() || player->getID() < 0)
	{
		return std::map<Tile*, int>();
	}
	return _visionMaps[player->getID()];
}

std::set<Tile*>& Map::getOwnBuildings(Player* player)
{
	if (player->getID() >= (int)_visionMaps.size() || player->getID() < 0)
	{
		return std::set<Tile*>();
	}
	return _buildingsByPlayer[player->getID()];
}

void Map::draw()
{
	Tile* curTile;

	//Set shader to use
	GLuint shaderID = AssetPool::getShader("Tile Shader");
	glUseProgram(shaderID);
	
	auto shaderViewVariable = glGetUniformLocation(shaderID, "view");
	auto shaderSelectedVariable = glGetUniformLocation(shaderID, "isSelected");
	auto shaderVisibleVariable = glGetUniformLocation(shaderID, "isVisible");
	auto shaderOwnedVariable = glGetUniformLocation(shaderID, "isOwned");
	auto shaderSideColourVariable = glGetUniformLocation(shaderID, "sideColour");

	//Set projection and view matrices (these do not change between draw calls)
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, false, glm::value_ptr(*Camera::getProjection()));
	glUniformMatrix4fv(shaderViewVariable, 1, false, glm::value_ptr(*Camera::getView()));

	//Enable vertex and index arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_INDEX_ARRAY);

	//Enable attribute arrays 0 and 1
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//Bind buffers
	Tile::bindBuffers();

	
	GLuint curTexture;
	
	glUniform1i(shaderOwnedVariable, false);

	//X offset
	glm::vec3 xOffset = glm::vec3(1, 0, 0);

	//Y offset
	glm::vec3 yOffset = glm::vec3(-_width, 1, 0);

	//Model View matrix
	glm::mat4 modelView = *Camera::getView();

	//Draw tiles
	for(int y = 0; y < _height; y++)
	{
		for(int x = 0; x < _width; x++)
		{
			//Set model view
			glUniformMatrix4fv(shaderViewVariable, 1, false, glm::value_ptr(modelView));

			curTile = _tiles[y][x];

#ifdef DEBUG_MODE
			if (_view == 0)
			{
				if (_visionMaps[0][curTile] == 0)
				{
					glUniform1i(shaderVisibleVariable, false);
				}
				else
				{
					glUniform1i(shaderVisibleVariable, true);
				}
			}
			else if (_view == 1)
			{
				if (_visionMaps[1][curTile] == 0)
				{
					glUniform1i(shaderVisibleVariable, false);
				}
				else
				{
					glUniform1i(shaderVisibleVariable, true);
				}
			}
			else
			{
				glUniform1i(shaderVisibleVariable, true);
			}
#else
			//Set visible variable
			if (_visionMaps[0][curTile] == 0)
			{
				glUniform1i(shaderVisibleVariable, false);
			}
			else
			{
				glUniform1i(shaderVisibleVariable, true);
			}
#endif
			switch (curTile->getOwner())
			{
			case 0:
				glUniform1i(shaderOwnedVariable, true);
				glUniform4f(shaderSideColourVariable, 0.7f, 0.3f, 0.3f, 1);
				break;

			case 1:
				glUniform1i(shaderOwnedVariable, true);
				glUniform4f(shaderSideColourVariable, 0, 0, 1, 1);
				break;

			default:
				glUniform1i(shaderOwnedVariable, false);
				break;
			}

			//Get texture
			curTexture = AssetPool::getTexture(_textureMap[curTile->getType()]);
			
			//Set texture and texture parameters
			glBindTexture(GL_TEXTURE_2D, curTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			//Draw tile
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			//Translate
			modelView = glm::translate(modelView, xOffset);
		}
		modelView = glm::translate(modelView, yOffset);
	}

	glm::mat4 unitModelView;
	glm::vec3 tempTranslateOffset = glm::vec3(0.0f);	//Unit offset
	Vector2 curUnitPosition;					
	auto cameraView = glm::translate(*Camera::getView(), glm::vec3(0, 0, -0.05f));
	glUniform1i(shaderVisibleVariable, true);	//All drawn units are visible

	auto drawUnitFunc = [&](Unit* toDraw) mutable
	{
		switch (toDraw->getOwner())
		{
		case 0:
			glUniform4f(shaderSideColourVariable, 0.7f, 0.3f, 0.3f, 1);
			break;

		case 1:
			glUniform4f(shaderSideColourVariable, 0, 0, 1, 1);
			break;
		}

		if (toDraw->hasMoved())
		{
			glUniform1i(shaderVisibleVariable, false);
		}
		else
		{
			glUniform1i(shaderVisibleVariable, true);
		}

		curUnitPosition = toDraw->getPosition()->getPosition();
		tempTranslateOffset.x = curUnitPosition.x;
		tempTranslateOffset.y = curUnitPosition.y;
		unitModelView = glm::translate(cameraView, tempTranslateOffset);
		glUniformMatrix4fv(shaderViewVariable, 1, false, glm::value_ptr(unitModelView));

		//Set Texture
		curTexture = AssetPool::getTexture(toDraw->getTextureName());
		glBindTexture(GL_TEXTURE_2D, curTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//Draw unit
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	};

	glUniform1i(shaderOwnedVariable, true);

#ifdef DEBUG_MODE
	if (_debugOn)
	{
		if (_view == 0 || _view == 1)
		{
			for (auto unit : _unitsByPlayer[_view])
			{
				drawUnitFunc(unit);
			}

			for (auto unit : getKnownUnits(_players[_view]))
			{
				drawUnitFunc(unit);
			}
		}
		else
		{
			for (auto unitVector : _unitsByPlayer)
			{
				for (auto unit : unitVector)
				{
					drawUnitFunc(unit);
				}
			}
		}
	}
	else
	{
		for (auto unit : _unitsByPlayer[0])
		{
			drawUnitFunc(unit);
		}

		for (auto unit : getKnownUnits(_players[0]))
		{
			drawUnitFunc(unit);
		}
	}

#else
	//TODO: Draw units
	//TODO: Draw player units
	for(std::set<Unit*>::iterator it = _unitsByPlayer[0].begin(); it != _unitsByPlayer[0].end(); ++it)
	{
		//Draw unit

		//Set Position
		curUnitPosition = (*it)->getPosition()->getPosition();
		tempTranslateOffset.x = curUnitPosition.x;
		tempTranslateOffset.y = curUnitPosition.y;
		unitModelView = glm::translate(cameraView, tempTranslateOffset);
		glUniformMatrix4fv(shaderViewVariable, 1, false, glm::value_ptr(unitModelView));

		//Set Texture
		curTexture = AssetPool::getTexture((*it)->getTextureName());
		glBindTexture(GL_TEXTURE_2D, curTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//Draw unit
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	//Draw visible enemy units
	std::set<Unit*> enemyUnits = getKnownUnits(_players[0]);

	for(std::set<Unit*>::iterator it = enemyUnits.begin(); it != enemyUnits.end(); ++it)
	{
		//Set Position
		curUnitPosition = (*it)->getPosition()->getPosition();
		tempTranslateOffset.x = curUnitPosition.x;
		tempTranslateOffset.y = curUnitPosition.y;
		unitModelView = glm::translate(cameraView, tempTranslateOffset);
		glUniformMatrix4fv(shaderViewVariable, 1, false, glm::value_ptr(unitModelView));

		//Set Texture
		curTexture = AssetPool::getTexture((*it)->getTextureName());
		glBindTexture(GL_TEXTURE_2D, curTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//Draw unit
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
#endif

	glUseProgram(0);
	
	//Disable stuff
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
}

#ifdef DEBUG_MODE

void Map::debugPrint(std::string mapName)
{
	std::ofstream output;

	
	output.open(mapName + " Debug Info.txt", std::ios_base::out | std::ios_base::trunc);

	bool valid = _maps.find(mapName) != _maps.end();
	

	output << "Is valid: " << (valid ? "True" : "False") << std::endl;

	if(!valid)
	{
		return;
	}

	MapTemplate* map = _maps[mapName];

	output << "N Tiles: " << map->map.size() * map->map[0].size() << std::endl;

	output << "Width: " << map->map[0].size() << std::endl;

	output << "Height: " << map->map.size() << std::endl;

	output << "N Players: " << map->playerIDs.size() << std::endl;

	output << std::endl;

	for(size_t y = 0; y < map->map.size(); y++)
	{
		for(size_t x = 0; x < map->map[y].size(); x++)
		{
			output << TerrainToString(std::get<0>(map->map[y][x])) << " " << std::get<1>(map->map[y][x]) << " | ";
		}
		output << std::endl;
	}

	//Print units
	output << "Unit type | x pos | y pos | owner ID" << std::endl;
	for (auto unit : map->units)
	{
		output << std::get<0>(unit) << " | " << std::get<1>(unit) << " | " << std::get<2>(unit) << " | " << std::get<3>(unit) << std::endl;
	}
}

void Map::debugPrint()
{
	std::ofstream output;

	
	output.open("Cur Map Debug Info.txt", std::ios_base::out | std::ios_base::trunc);

	
	

	output << "Is valid: " << (_isValid ? "True" : "False") << std::endl;

	if(!_isValid)
	{
		return;
	}

	output << "N Tiles: " << _tiles.size() * _tiles[0].size() << std::endl;

	output << "Width: " << _tiles[0].size() << std::endl;

	output << "Height: " << _tiles.size() << std::endl;

	output << "N Players: " << _nPlayers << std::endl;

	output << std::endl;

	for(size_t y = 0; y < _tiles.size(); y++)
	{
		for(size_t x = 0; x < _tiles[y].size(); x++)
		{
			output << TerrainToString(_tiles[y][x]->getType()) << " " << _tiles[y][x]->getOwner() << " | ";
		}
		output << std::endl;
	}

	//Print units
	output << "Unit type | x pos | y pos | owner ID" << std::endl;

	for (auto player : _unitsByPlayer)
	{
		for (auto unit : player)
		{
			output << unit->getType() << " | " << unit->getPosition()->getPosition().x << " | " << unit->getPosition()->getPosition().y << " | " << unit->getOwner() << std::endl;
		}
	}
}

#endif