#include "StdAfx.h"
#include "AI.h"

AI::AI(): Player(false)
{
	_isRunning = false;
	_ownBuildingThreatNumber = 5;
	_otherBuildingThreatNumber = 1;
	_unitThreatNumber = 1;
	_capturingUnitThreatNumber = 1;
	_map = 0;


#ifdef DEBUG_MODE
	_debugMode = false;
	_isPaused = false;
	_debugTargetTile = 0;
#endif
}

void AI::setupCounters()
{
	_counterLookup.clear();
	auto templates = Unit::unitTemplates;

	int strength, curStrength;
	char strongestType;

	for (auto toCounter : templates)
	{
		strength = 0;
		strongestType = ' ';
		for (auto type : templates)
		{
			curStrength = Unit::weaponEffectivenessTable[type.second.weapon][toCounter.second.armour];

			if (curStrength > strength)
			{
				strength = curStrength;
				strongestType = type.first;
			}
		}
		_counterLookup[toCounter.first] = strongestType;
	}
}

std::shared_ptr<std::vector<Tile*>> AI::findShortestPath(Unit* unit, Tile* from, Tile* to)
{
	std::map<Tile*, Tile*> origins;
	std::map<Tile*, int> distanceFromStart;
	std::map<Tile*, int> estDistToEnd;
	std::set<Tile*> openNodes;
	std::set<Tile*> closedNodes;
	Tile* curNode, *curNeighbor;
	int curDist;

	auto findClosestNode = [&]() -> Tile*
	{
		int dist = 9999;
		Tile* curClosest = 0;
		for (auto node : openNodes)
		{
			if (estDistToEnd[node] < dist)
			{
				dist = estDistToEnd[node];
				curClosest = node;
			}
		}
		return curClosest;
	};

	openNodes.insert(from);
	distanceFromStart[from] = 0;
	estDistToEnd[from] = Tile::getDistance(from, to);

	while (!openNodes.empty())
	{
		curNode = findClosestNode();
		openNodes.erase(curNode);
		closedNodes.insert(curNode);

		//Destination has been found.
		if (curNode == to)
		{
			break;
		}

		for (size_t i = 0; i < TILE_N_NEIGHBORS; i++)
		{
			curNeighbor = curNode->getNeighbor(i);

			if (!curNeighbor)
			{
				continue;
			}

			curDist = distanceFromStart[curNode] + unit->getMoveCost(curNeighbor);

			//If the neighbor has already been travelled to, or the distance to the neighbor from its current origin node is shorter than from the current node, continue.
			if (closedNodes.find(curNeighbor) != closedNodes.end() || (distanceFromStart.find(curNeighbor) != distanceFromStart.end() && distanceFromStart[curNeighbor] <= curDist))
			{
				continue;
			}

			if (openNodes.find(curNeighbor) == openNodes.end() || curDist < distanceFromStart[curNeighbor])
			{
				openNodes.insert(curNeighbor);
				origins[curNeighbor] = curNode;
				distanceFromStart[curNeighbor] = curDist;
				estDistToEnd[curNeighbor] = curDist + Tile::getDistance(curNeighbor, to);
			}
		}
	}

	auto path = std::make_shared<std::vector<Tile*>>();
	std::vector<Tile*> reversedPath;
	reversedPath.push_back(to);
	for (curNode = to; curNode != from; (curNode = origins[curNode]), reversedPath.push_back(curNode));
	for (auto node : reversedPath)
	{
		path->push_back(node);
	}
	return path;
}


int AI::update()
{
	//TODO: Update
	if(_map == 0 || !_map->isValid() || !_map->isActive())	//No Map to play on
	{
		return 1;
	}
	_isRunning = true;

	
	std::set<Unit*> oldKnownUnits, knownUnits;				//Known enemy units
	std::map<Tile*, int> visibleTiles;						//Visible tiles
	std::set<Unit*> getOwnUnits = _map->getOwnUnits(this);	//Ai's own units
	std::set<Unit*> unMovedUnits = getOwnUnits;				//Set of units that can still move and attack.
	std::vector<std::pair<Unit*, int>> sortedThreats;
	std::map<Tile*, int> interestRating;

	std::set<Tile*> buildings = _map->getBuildings();		//Buildings not owned by the player
	std::set<Tile*> ownBuildings = _map->getOwnBuildings(this);	//Own buildings.



	std::map<char, int> unitTypeCount;

	for (auto unitType : Unit::unitTemplates)
	{
		unitTypeCount[unitType.first] = 0;
	}

	auto threatRatingAlgorithym = [&](Unit* toInsert) -> int
	{
		int threatRating = 0;	//Unit's threat rating

		auto& unitThreatRange = toInsert->getValidTiles();
		auto& unitAttackRange = toInsert->getAttackableTiles();

		//Determine unit's building threat
		for (auto threatenedTile : unitThreatRange)
		{
			if (buildings.find(threatenedTile) != buildings.end())
			{
				if (threatenedTile->getOwner() == this->getID())
				{
					threatRating += _ownBuildingThreatNumber;
				}
				else if(threatenedTile->getOwner() != toInsert->getOwner())
				{
					threatRating += _otherBuildingThreatNumber;
				}
			}
		}

		

		for (auto unit : getOwnUnits)
		{
			if (unitAttackRange.find(unit->getPosition()) != unitAttackRange.end())
			{
				//TODO: Increase threat rating based on threat to unit and whether unit is capturing a building.
				if (unit->getPosition()->getCapturingSide() != this->getID())
				{
					//Increase
					threatRating += _capturingUnitThreatNumber;
				}
				else
				{
					threatRating += _unitThreatNumber;
				}
			}
		}



		return threatRating;
	};

	auto pointOfinterestAnalysisAlgorithym = [&](Tile* tile)
	{
		int rating = 0;

		if (tile->getOwner() == _playerID)
		{
			rating -= 10000;
		}

		if (tile->getType() == Factory && tile->getOwner() != _playerID)
		{
			rating += 10;
		}
		else if (tile->getType() == City && tile->getOwner() != _playerID)
		{
			rating += 5;
		}

		for (auto ownUnit : getOwnUnits)
		{
			if (ownUnit->getPosition() == tile)
			{
				rating -= 10;
			}
		}

		interestRating[tile] = rating;
	};

	
	knownUnits = _map->getKnownUnits(this);
	visibleTiles = _map->getVisibleTiles(this);
	sortedThreats.reserve(knownUnits.size());

	Unit* strongestUnit = 0;
	int strength = 0;
	Tile* farthestTile = 0, *curNeighborTile = 0, *targetTile = 0;
	int distance = 0, curDist = 0, interest = 0;
	char unitType;

#ifdef DEBUG_MODE
	if (sortedThreats.size() > 0)
	{
		_debugTargetTile = sortedThreats[0].first->getPosition();
	}
#endif

#pragma region Capture_Phase
	std::vector<Unit*> capturingUnits;
	TerrainType type;

	//If the unit can attempt capture, capture.
	for (auto unit : unMovedUnits)
	{
		type = unit->getPosition()->getType();
		if ((type == City || type == Factory) && unit->getPosition()->getOwner() != _playerID)
		{
			_map->issueCaptureCommand(this, unit, unit->getPosition());
			capturingUnits.push_back(unit);
		}
	}

	//Remove units that are capturing from the unmoved unit list.
	for (auto unit : capturingUnits)
	{
		unMovedUnits.erase(unit);
	}
#pragma endregion

	//Remove own buildings from the buildings set.
	for (auto bld : ownBuildings)
	{
		if (buildings.find(bld) != buildings.end())
		{
			buildings.erase(bld);
		}
	}

	for (auto unit : getOwnUnits)
	{
		knownUnits.erase(unit);
	}

#pragma region Initial_Assessment_Phase
	//Get list of known enemy units, identify and rank threats/points of interest, and take actions accordingly.
	//Threats are ranked based on how close they are to buildings.
	//Points of interest are either buildings, or rough areas to scout (mainly around buildings).
	//Additionally, moving units away from combat and to owned buildings for repairs is another lesser goal.
	//Finally, at the end of the turn, create units to counter known units in factories.

	//Insert units into the sortedThreats list
	for (auto unit : knownUnits)
	{
		if (unit)
		{
			insertThreat(unit, threatRatingAlgorithym(unit), sortedThreats);
			unitTypeCount[unit->getType()]++;
		}
	}

	for (auto building : buildings)
	{
		pointOfinterestAnalysisAlgorithym(building);
	}
#pragma endregion

	//Attack/scout phase
	while(unMovedUnits.size() != 0)
	{
		//Reset all data
		strongestUnit = 0;
		strength = 0;
		distance = 0;
		curDist = 0;
		farthestTile = 0;
		curNeighborTile = 0;

		//Deal with threats first, then scout.
		if (sortedThreats.size() != 0)
		{
			//TODO: Determine which unit type to attack with
#ifdef DEBUG_MODE
			_debugTargetTile = sortedThreats[0].first->getPosition();
			if (_debugMode && _isPaused)
			{
				std::unique_lock<std::mutex> lock(_pauseLock);
				_cv.wait(lock);
			}
			_debugTargetTile = 0;
#endif

			//Determine which units can reach the target and find the best canididate to attack with.
			for (auto unit : unMovedUnits)
			{
				//If the current unit is more effective than the currently selected unit, and is within attack range, set that unit as the strongest.
				if (Unit::weaponEffectivenessTable[unit->getWeapon()][unit->getArmour()] > strength && 
					unit->getAttackableTiles().find(sortedThreats[0].first->getPosition()) != unit->getAttackableTiles().end())
				{
					strength = Unit::weaponEffectivenessTable[unit->getWeapon()][unit->getArmour()];
					strongestUnit = unit;
				}
			}
			
			//If there is a unit that can attack the threat, move to attack it. Otherwise remove the threat from the list (since nothing can attack it).
			if (strongestUnit)
			{
				for (int i = 0; i < TILE_N_NEIGHBORS; i++)
				{
					curNeighborTile = sortedThreats[0].first->getPosition()->getNeighbor(i);

					if (!curNeighborTile)
					{
						continue;
					}

					if (strongestUnit->canMoveTo(curNeighborTile))
					{
						curDist = Tile::getDistance(curNeighborTile, strongestUnit->getPosition());
						if (curDist >= distance)
						{
							farthestTile = curNeighborTile;
							distance = curDist;
						}
					}
				}

				//If the unit moved to the tiles successfully, attack.
				if (farthestTile == strongestUnit->getPosition() || _map->issueMoveCommand(this, strongestUnit, strongestUnit->getPathTo(farthestTile)))
				{
					unitType = sortedThreats[0].first->getType();
					_map->issueAttackCommand(this, strongestUnit, sortedThreats[0].first);

					//If the unit is destroyed, remove it from lists.
					if (!sortedThreats[0].first->isAlive())
					{
						sortedThreats.erase(sortedThreats.begin());
						unitTypeCount[unitType]--;

						//If the game is over, return.
						if (!_map->isActive())
						{
							strongestUnit = 0;
							farthestTile = 0;
							curNeighborTile = 0;
							oldKnownUnits.clear();
							knownUnits.clear();
							visibleTiles.clear();
							_isRunning = false;
							return 0;
						}
					}
				}

				//Remove the moved unit from unmoved units set.
				unMovedUnits.erase(strongestUnit);
			}
			else
			{
				sortedThreats.erase(sortedThreats.begin());
			}
		}
		else
		{
			//Scout
			interest = -9999;
			
			//For each unmoved unit, get the unit that is closest to an unallied building.
			for (auto unit : unMovedUnits)
			{
				for (auto pointOfInterest : interestRating)
				{
					if (interest < pointOfInterest.second)
					{
						interest = pointOfInterest.second;
						strongestUnit = unit;
						targetTile = pointOfInterest.first;
					}
				}
			}

			if (!strongestUnit)
			{
				break;
			}

#ifdef DEBUG_MODE
			_debugTargetTile = targetTile;
			if (_debugMode && _isPaused)
			{
				std::unique_lock<std::mutex> lock(_pauseLock);
				_cv.wait(lock);
			}
			_debugTargetTile = 0;
#endif
			
			auto path = findShortestPath(strongestUnit, strongestUnit->getPosition(), targetTile);
			auto validTiles = strongestUnit->getValidTiles();

			//Find the farthest the unit can travel towards the target building.
			for (auto tile : *path)
			{
				if (validTiles.find(tile) != validTiles.end())
				{
					farthestTile = tile;
					break;
				}
			}

			//Move the target to the fathest tile.
			_map->issueMoveCommand(this, strongestUnit, strongestUnit->getPathTo(farthestTile));

			//Capture if the unit has moved to the tile.
			if (farthestTile == targetTile)
			{
				_map->issueCaptureCommand(this, strongestUnit, targetTile);
			}
			unMovedUnits.erase(strongestUnit);
			pointOfinterestAnalysisAlgorithym(targetTile);	//Reassess the point of interest.
		}
		
		oldKnownUnits = knownUnits;					//set the values for oldKnownUnits to knownUnits
		knownUnits = _map->getKnownUnits(this);		//Get the new known units
		visibleTiles = _map->getVisibleTiles(this);	//Get the new visible tiles

		//Insert newly discovered units into threat listing.
		for (auto unit : knownUnits)
		{
			if (unit->getOwner() != _playerID && oldKnownUnits.find(unit) == oldKnownUnits.end())
			{
				insertThreat(unit, threatRatingAlgorithym(unit), sortedThreats);
				unitTypeCount[unit->getType()]++;
			}
		}

		for (auto building : buildings)
		{
			pointOfinterestAnalysisAlgorithym(building);
		}
	}

	strongestUnit = 0;
	farthestTile = 0;
	curNeighborTile = 0;

	//TODO: get own buildings, determine counters to enemy units based on number and distance from each factory, and build.
	std::vector<Tile*> toRemove;
	ownBuildings = _map->getOwnBuildings(this);

	for (auto curBuilding : ownBuildings)
	{
		if (curBuilding->getType() == City || _map->isOccupied(curBuilding))
		{
			toRemove.push_back(curBuilding);
		}
	}

	for (auto curBuilding : toRemove)
	{
		ownBuildings.erase(curBuilding);
	}

	//Create units
	if (knownUnits.size() != 0)
	{
		std::map<char, int> threateningUnits;
		std::vector<std::tuple<Tile*, char, int>> buildOrder;
		int rating, curRating;
		char strongestType;

		auto insertBuildOrder = [&](Tile* factory, char type, int rating)
		{
			for (auto curEntry = buildOrder.begin(); curEntry != buildOrder.end(); ++curEntry)
			{
				if (rating > std::get<int>(*curEntry))
				{
					buildOrder.emplace(curEntry, factory, type, rating);
					return;
				}
			}

			buildOrder.emplace_back(factory, type, rating);
		};

		//For each building, get threatening units, rated by proximity.
		for (auto curBuilding : ownBuildings)
		{
			threateningUnits.clear();
			rating = 0;

			//Get rating
			for (auto unit : knownUnits)
			{
				threateningUnits[unit->getType()] += std::max(10 - Tile::getDistance(unit->getPosition(), curBuilding), 0);
			}

			//Determine biggest threat
			for (auto type : threateningUnits)
			{
				curRating = type.second;

				if (curRating > rating)
				{
					rating = curRating;
					strongestType = type.first;
				}
			}

			//Insert into build orders.
			if (rating != 0)
			{
				insertBuildOrder(curBuilding, strongestType, rating);
			}
		}

		if (buildOrder.size() != 0)
		{
			for (auto order : buildOrder)
			{
				_map->issueBuildCommand(this, getOpposingType(std::get<1>(order)), std::get<0>(order));
			}
		}
		else
		{
			buildCapturingUnits(ownBuildings, interestRating);
		}
	}
	else
	{
		buildCapturingUnits(ownBuildings, interestRating);
	}

	_map->endTurn(this);

	return 0;
}

void AI::buildCapturingUnits(std::set<Tile*>& ownBuildings, std::map<Tile*, int>& interestRating)
{
	int interest;
	Tile* targetTile;

	std::vector<std::pair<Tile*, int>> buildOrder;
	auto insertBuildOrder = [&](Tile* factory, int rating)
	{
		for (auto curEntry = buildOrder.begin(); curEntry != buildOrder.end(); ++curEntry)
		{
			if (rating > (*curEntry).second)
			{
				buildOrder.emplace(curEntry, factory, rating);
				return;
			}
		}

		buildOrder.emplace_back(factory, rating);
	};

	int curInterest = 0;

	for (auto curBuilding : ownBuildings)
	{
		interest = -9999;
		targetTile = 0;
		for (auto pointOfInterest : interestRating)
		{
			curInterest = pointOfInterest.second - Tile::getDistance(curBuilding, pointOfInterest.first);
			if (interest < curInterest)
			{
				targetTile = curBuilding;
				interest = curInterest;
			}
		}

		if (targetTile)
		{
			insertBuildOrder(targetTile, interest);
		}
	}

	for (auto entry : buildOrder)
	{
		_map->issueBuildCommand(this, 'i', entry.first);
	}
}
