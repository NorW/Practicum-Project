#include "StdAfx.h"
#include "Unit.h"

int Unit::weaponEffectivenessTable[N_WEAPONS][N_ARMOURS] =	{{25, 10, 5},	//Pistol
														{65, 20, 10},	//Rifle
														{10, 40, 60},	//Anti-tank
														{80, 65, 40}};	//Cannon
int Unit::movementTable[N_TILE_TYPES][N_MOVEMENT_TYPES] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1},	//Cities, factories, and roads
													{1, 2, 1}, {2, 2, 3},				//Grass and hill
													{UNIT_INVALID_MOVE_SPACE, UNIT_INVALID_MOVE_SPACE, UNIT_INVALID_MOVE_SPACE},	//Mountain
													{2, 3, 4}};	//Swamp

bool Unit::_isInitialized = false;

std::map<char, UnitTemplate> Unit::unitTemplates;

std::deque<Unit*> Unit::_objectPool;

Unit* Unit::createUnit(char unitType, Tile* position, int owner, int funds)
{
	Unit* temp = 0;
	if(unitTemplates.find(unitType) != unitTemplates.end())
	{
		if(funds > unitTemplates[unitType].unitCost)	//If there are enough funs, make the unit
		{
			//if the first object is alive, there are no open units.
			if (!_objectPool.front()->isAlive())
			{
				//Get the first object, remove it from the object pool, and set its new data.
				temp = _objectPool.front();
				_objectPool.pop_front();
				temp->setNewData(unitTemplates[unitType], position, owner);
			}
			else
			{
				//Create a new unit (since the object pool is full).
				temp = new Unit(unitTemplates[unitType], position, owner);
			}
			_objectPool.push_back(temp);	//Push the object into the back of the object pool.
			temp = 0;
			return _objectPool.back();		//Return the object.
		}
	}

	return 0;
}

void Unit::resetPool()
{
	for (auto unit : _objectPool)
	{
		unit->_isAlive = false;
	}
}

//Parses and creates a new unit template from the parser and adds it to the unit template map.
bool Unit::parseUnitTemplateEntry(std::istringstream& parser)
{
	char weaponType, armourType, moveType;
	int moveDist, viewDist, unitCost;
	std::string unitName, textureName, rawNameData;
	size_t nameStart, nameEnd;
	WeaponType wT;
	ArmourType aT;
	MoveType mT;
	char iD;
	char buffer[256];

	parser >> iD >> weaponType >> armourType >> moveType >> moveDist >> viewDist >> unitCost;
	parser.getline(buffer, 256);
	rawNameData.clear();
	rawNameData = buffer;

	//Sanity check unit name
	nameStart = rawNameData.find_first_of('\"');	//Get start of name

	if (nameStart == rawNameData.npos)
	{
		logError("Error: Failed to find unit name");
		return false;
	}

	nameEnd = rawNameData.find_first_of('\"', nameStart + 1);	//Get end of name

	if (nameEnd == rawNameData.npos)
	{
		logError("Error: Failed to find unit name");
		return false;
	}

	unitName = rawNameData.substr(nameStart + 1, nameEnd - nameStart - 1);

	if (unitName.size() == 0)
	{
		logError("Error: Failed to find unit name");
		return false;
	}

	rawNameData = rawNameData.substr(nameEnd + 1);

	//Sanity check texture name
	nameStart = rawNameData.find_first_of('\"');

	if (nameStart == rawNameData.npos)
	{
		logError("Error: Failed to find unit texture name");
		return false;
	}

	nameEnd = rawNameData.find_first_of('\"', nameStart + 1);

	if (nameEnd == rawNameData.npos)
	{
		logError("Error: Failed to find unit texture name");
		return false;
	}

	textureName = rawNameData.substr(nameStart + 1, nameEnd - nameStart - 1);

	if (textureName.size() == 0)
	{
		logError("Error: Failed to find unit texture name");
		return false;
	}

	if (parser.fail())
	{
		logError("Error: Failed to read unit template entry");
		return false;
	}
	else if (unitTemplates.find(iD) != unitTemplates.end())
	{
		logError("Error: Duplicate unit ID found");
		return false;
	}
	
	//Parse weapon type
	switch (weaponType)
	{
	case 'p':
		wT = Pistol;
		break;

	case 'r':
		wT = Rifle;
		break;

	case 'a':
		wT = AntiTank;
		break;

	case 'c':
		wT = Cannon;
		break;

	default:
		logError("Error: Failed to parse weapon type");
		return false;
	}

	//Parse armour type
	switch (armourType)
	{
	case 'i':
		aT = Infantry;
		break;
	
	case 'l':
		aT = Light;
		break;

	case 'h':
		aT = Heavy;
		break;

	default:
		logError("Error: Failed to parse armour type");
		return false;
	}

	//Parse move type
	switch (moveType)
	{
	case 'f':
		mT = Feet;
		break;

	case 'w':
		mT = Wheels;
		break;

	case 't':
		mT = Tread;
		break;

	default:
		logError("Error: Failed to parse move type");
		return false;
	}

	unitTemplates[iD] = UnitTemplate(iD, wT, aT, mT, moveDist, viewDist, unitCost, unitName, textureName);	//Add new template.
	return true;
}

//Clears and re-populates the unit templates map with entries from the specified file.
int Unit::getUnitTemplates(const char* file)
{
	FILE* fp = fopen(file, "r");
	if (!fp)
	{
		logError("Critical Error: Failed to open unit template file.");
		return UNIT_TEMPLATE_READ_ERROR;
	}
	fclose(fp);

	unitTemplates.clear();

	auto reader = std::ifstream(file);
	std::istringstream parser;
	std::string tempStr;

	char temp[256];
	
	size_t entryCommentPos;

	while (!reader.eof())
	{
		parser.clear();
		reader.getline(temp, 256);
		tempStr.assign(temp);
		entryCommentPos = tempStr.find("//");

		if (entryCommentPos == tempStr.npos)
		{
			parser.str(tempStr);
		}
		else if(entryCommentPos != 0)
		{
			parser.str(tempStr.substr(0, entryCommentPos));
		}
		else
		{
			continue;
		}

		if (!parseUnitTemplateEntry(parser))
		{
			logError("Failed to load template");
		}
	}

	return 0;
}

//Loads unit templates and may deal with other things needing initialization.
int Unit::initialize(const char* fileName)
{
	auto unitReadError = getUnitTemplates(fileName);

	if (unitReadError == 0)
	{
		_isInitialized = true;
	}

	for (int i = 0; i < UNIT_DEFAULT_POOL_SIZE; i++)
	{
		_objectPool.push_back(new Unit());
	}

	return unitReadError;
}

void Unit::dispose()
{
	for (auto unit : _objectPool)
	{
		//delete unit;
	}
}

Unit::Unit()
{
	_isAlive = false;
}

Unit::Unit(UnitTemplate& data, Tile* position, int owner): _unitType(data.unitType), _weapon(data.weapon), _armour(data.armour), _movement(data.movement), 
														_moveDistance(data.moveDistance), _viewDistance(data.viewDistance), _position(position), 
														_unitCost(data.unitCost), _owner(owner), _texture(data.texture)
{
	_validTiles.clear();
	_visibleTiles.clear();
	_tileOrigins.clear();
	_attackableTiles.clear();
	_hasAttacked = true;
	_hasMoved = true;
	_health = 100;
	_isAlive = true;

	if (position->getType() == Hill)
	{
		_bonusViewDist = 1;
	}
	else
	{
		_bonusViewDist = 0;
	}
}

bool Unit::setNewData(UnitTemplate& data, Tile* position, int owner)
{
	if (_isAlive)
	{
		return false;
	}

	_unitType = data.unitType;
	_unitCost = data.unitCost;
	_weapon = data.weapon;
	_armour = data.armour;
	_movement = data.movement;
	_moveDistance = data.moveDistance;
	_viewDistance = data.viewDistance;
	_position = position;
	_owner = owner;
	_texture = data.texture;
	_validTiles.clear();
	_visibleTiles.clear();
	_tileOrigins.clear();
	_attackableTiles.clear();
	_hasAttacked = true;
	_hasMoved = true;
	_health = 100;
	_isAlive = true;

	if (position->getType() == Hill)
	{
		_bonusViewDist = 1;
	}
	else
	{
		_bonusViewDist = 0;
	}
}

//Handles attacks. Returns true if the defender is alive, false otherwise.
bool Unit::handleAttack(Unit* attacker, Unit* defender)
{
	if (attacker->_hasAttacked)
	{
		return false;
	}

	attacker->_hasAttacked = true;
	attacker->_hasMoved = true;

	defender->_health -= weaponEffectivenessTable[attacker->_weapon][defender->_armour];
	defender->_isAlive = (defender->_health > 0);
	
	//If the defender is no longer alive, move it to the front of the object pool.
	if (!defender->_isAlive)
	{
		//Iterate through the pool in reverse order (with reverse iterators) since alive objects are kept in the end of the object pool, while dead objects are kept at the beginning.
		for (auto it = _objectPool.begin(); it != _objectPool.end(); ++it)
		{
			if ((*it) == defender)
			{
				_objectPool.erase(it);
				_objectPool.push_front(defender);
				break;
			}
		}
	}
	
	return defender->_isAlive;
}

/*
	Determines the tiles this unit can see.
	This algorithm determines which tiles the unit can see by grabbing neighboring tiles of the tiles in the openNodes set that are not in the closedNodes set
	and adding them to currentNeighbors and visibleTiles, repeating this proccess over the unit's view distance. This algorithm should work on any map that uses
	tiles that are convex in shape if the tile are only connected to tiles that touch its sides and if each tile is identical in shape (e.g. chessboards).
*/
void Unit::findVisibleTiles()
{
	std::set<Tile*> closedNodes;		//Previous level of nodes
	std::set<Tile*> openNodes;			//Current level of nodes
	std::set<Tile*> currentNeighbors;	//Next level of nodes

	openNodes.insert(_position);
	Tile* curNode;
	Tile* curNeighbor;

	_visibleTiles.push_back(_position);

	int finalViewDistance = _viewDistance + _bonusViewDist;

	for(int i = 0; i < finalViewDistance; i++)
	{
		for(std::set<Tile*>::iterator it = openNodes.begin(); it != openNodes.end(); ++it)
		{
			curNode = *it;
			for(int j = 0; j < TILE_N_NEIGHBORS; j++)
			{
				curNeighbor = curNode->getNeighbor(j);
				if(curNeighbor && (closedNodes.find(curNeighbor) == closedNodes.end()))	//If the current neighbor exists and is not in the closed nodes list, add it.
				{
					currentNeighbors.insert(curNeighbor);
					_visibleTiles.push_back(curNeighbor);
				}
			}
		}
		closedNodes.clear();
		closedNodes = openNodes;
		openNodes.clear();
		openNodes = currentNeighbors;
	}
}

/*
	Finds all the tiles the unit may move to and stores them in _validTiles.
	Also finds the shortest paths to each tile from the unit and stores them in _tileOrigins.

	Note: This function should only be called from the function "recalcualteVisionAndMovement".
	If this function is called from any other place, take not that this function assumes _validTiles is empty.
*/
void Unit::findValidTiles()
{
	_tileOrigins.clear();					//Map of tile origins (shortest distance from the unit to each tile). Also doubles as a list of neighbors found.
	_attackableTiles.clear();
	std::deque<Tile*> openNodes;			//List of nodes to proccess
	std::map<Tile*, int> distanceFromUnit;	//Minimum distances from the unit to each tile
	
	openNodes.push_back(_position);			//Add current position to open nodes.
	_tileOrigins[_position] = _position;	//Current position originates from itself (to prevent origin checks from getting a null)
	distanceFromUnit[_position] = 0;		//Current position is a distance of 0 away from itself

	Tile* curNode;		//Current node being proccessed from openNodes
	Tile* curNeighbor;	//Current neighbor of curNode being proccessed
	int curDist;		//Distance from unit through curNode to curNeighbor

	//While there are still nodes to proccess
	while(!openNodes.empty())
	{
		curNode = openNodes.front();		//Grab the next node
		openNodes.pop_front();				//Remove it from openNodes

		//For each neighbor
		for(int i = 0; i < TILE_N_NEIGHBORS; i++)
		{
			curNeighbor = curNode->getNeighbor(i);

			//If the neighbor is null, dont proccess it.
			if(!curNeighbor)
			{
				continue;
			}

			//Calculate the move cost to the neighbor through curNode
			curDist = distanceFromUnit[curNode] + getMoveCost(curNeighbor);

			/*
				If the neighboring tile has been found before and the neighboring node is closer to the unit through curNode 
				than its previous originating node, curNeighbor now originates from curNode (because of the shorter distance).
				This is so that the unit can determine all nodes it can travel to.

				Else if the neighboring tile has never been encountered and the distance to the neighbor through curNode is within
				the unit's max move distance, add the neighbor to open nodes, valid nodes, and set the origin and distance values.

				Else if the neighboring tile can not be reached (1 tile away), add it to the set of attackable tiles.
			*/
			if(_tileOrigins.find(curNeighbor) != _tileOrigins.end() && distanceFromUnit[curNeighbor] >= curDist)
			{
				distanceFromUnit[curNeighbor] = curDist;
				_tileOrigins[curNeighbor] = curNode;
			}
			else if(_tileOrigins.find(curNeighbor) == _tileOrigins.end() && curDist <= _moveDistance)
			{
				distanceFromUnit[curNeighbor] = curDist;
				_tileOrigins[curNeighbor] = curNode;
				openNodes.push_back(curNeighbor);
				_validTiles.insert(curNeighbor);
			}
			else if (curDist > _moveDistance)
			{
				_attackableTiles.insert(curNeighbor);
			}
		}
	}

	//Add all tiles the unit can move to into attackable tiles (since the unit may attack any tile that it can move to.
	for (auto tile : _validTiles)
	{
		_attackableTiles.insert(tile);
	}
}

std::set<Tile*>& Unit::getAttackableTiles()
{
	return _attackableTiles;
}

std::set<Tile*>& Unit::getValidTiles()
{
	return _validTiles;
}
	
std::vector<Tile*>& Unit::getVision()
{
	//If the visible tiles are unknown
	if(_visibleTiles.empty())
	{
		findVisibleTiles();
	}

	return _visibleTiles;
}

void Unit::recalcualteVision()
{
	_visibleTiles.clear();
	findVisibleTiles();
}

//Reconstructs the shortest path to tile from tile origins determined in findValidTiles() if tile is in the list of valid tiles the unit can go to. The path does NOT include the tile the unit is currently at.
std::vector<Tile*>* Unit::getPathTo(Tile* tile)
{
	//Path to the specified tile
	std::vector<Tile*>* path = new std::vector<Tile*>();

	//If the tile specified is in the list of valid tiles.
	if(_tileOrigins.find(tile) != _tileOrigins.end())
	{
		//Reversed path
		std::vector<Tile*> reversedPath;

		//Current tile. Start at the specified tile.
		Tile* curTile = tile;

		//Add the destination to the path
		reversedPath.push_back(tile);

		//While the current tile is not null or the unit's current position
		while(curTile && curTile != _position)
		{
			//Grab the tile that curTile originated from
			curTile = _tileOrigins[curTile];

			//If the tile is null break
			if(!curTile)
			{
				break;
			}

			//Else add the tile to reversed path
			reversedPath.push_back(curTile);
		}

		//Re reverse the path to get the path in the right order
		for(int i = reversedPath.size() - 1; i >= 0; i--)
		{
			path->push_back(reversedPath[i]);
		}
	}

	return path;
}

void Unit::procTurnEnd(int availableFunds)
{
	_hasMoved = false;
	_hasAttacked = false;
	_health = std::min(((availableFunds * 100) / _unitCost) + _health, 100);
}

int Unit::captureAmount()
{
	if(_armour == Infantry)
	{
		return 50;
	}

	_hasAttacked = true;

	return 25;
}

void Unit::move(Tile* newPosition)
{
	_position = newPosition;
	_hasMoved = true;

	if (newPosition->getType() == Hill)
	{
		_bonusViewDist = 1;
	}
	else
	{
		_bonusViewDist = 0;
	}

	_attackableTiles.clear();
	
	//The unit may now only attack tiles next to it.
	for (int i = 0; i < TILE_N_NEIGHBORS; i++)
	{
		_attackableTiles.insert(newPosition->getNeighbor(i));
	}
}

int Unit::getMoveCost(Tile* tile)
{
	return movementTable[tile->getType()][_movement];
}

int Unit::getRepairCost()
{
	return (std::min(100 - _health, 20) * _unitCost) / 100;
}