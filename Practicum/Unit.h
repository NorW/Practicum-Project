#pragma once
#include "StdAfx.h"
#include <deque>
#include <set>
#include <sstream>
#include "Tile.h"
#include "AssetPool.h"
#define UNIT_INVALID_MOVE_SPACE 1324
#define UNIT_TEMPLATE_READ_ERROR -1
#define UNIT_DEFAULT_POOL_SIZE 50


enum WeaponType
{
	Pistol = 0,
	Rifle,
	AntiTank,
	Cannon,
	N_WEAPONS
};

enum ArmourType
{
	Infantry = 0,
	Light,
	Heavy,
	N_ARMOURS
};

enum MoveType
{
	Feet = 0,
	Wheels,
	Tread,
	N_MOVEMENT_TYPES
};



struct UnitTemplate
{
	WeaponType weapon;	//Unit's weapon type
	ArmourType armour;	//Unit's armour type
	MoveType movement;	//Unit's movement type
	int moveDistance;	//Unit's movement distance per turn
	int viewDistance;	//Unit's visual distance
	int unitCost;		//Unit's creation cost
	char unitType;		//Integer used to identify the unit type.
	std::string name;
	std::string texture;//Name of image/animation used to represent the unit

	UnitTemplate() : texture("ASP_ERROR_TEXTURE") {}

	UnitTemplate(char uID, WeaponType w, ArmourType a, MoveType m, int mD, int vD, int uC, std::string n, std::string t) : unitType(uID), weapon(w), armour(a), movement(m),
																												moveDistance(mD), viewDistance(vD), unitCost(uC),
																												name(n), texture(t) 
	{
		AssetPool::loadTexture(t, t);
	}
};

class Unit
{
private:
	//TODO: implement an object pool for units.
	static std::deque<Unit*> _objectPool;

	static bool _isInitialized;

	static bool parseUnitTemplateEntry(std::istringstream& entry);
	static int getUnitTemplates(const char* file);
	static void resetPool();
	
	
	Tile* _position;						//The tile the unit currently occupies
	std::set<Tile*> _attackableTiles;		//List of tiles this unit can attack.
	std::set<Tile*> _validTiles;			//List of valid tiles this unit can move to
	std::vector<Tile*> _visibleTiles;		//List of tiles this unit can see
	std::map<Tile*, Tile*> _tileOrigins;	//Lists the shortest paths from the unit to each valid tile.
	WeaponType _weapon;				//Unit's weapon type
	ArmourType _armour;				//Unit's armour type
	MoveType _movement;				//Unit's movement type
	int _moveDistance;				//Unit's movement distance
	int _viewDistance;				//Unit's visual distance
	int _bonusViewDist;
	int _unitCost;
	char _unitType;
	std::string _texture;					//Name of image/animation used to represent the unit
	int _health;							//Unit's current health out of 100
	bool _hasMoved;							//Stores whether the unit has moved or not
	bool _hasAttacked;						//Stores whether the unit has attacked or not
	bool _isAlive;
	int _owner;								//The unit's owner

	void findValidTiles();					//Determines the tiles the unit may move to
	void findVisibleTiles();				//Determines the tiles the unit may see
	
	
	Unit();									//Creates a dead unit.

	Unit(UnitTemplate& data, Tile* position, int owner);

	bool setNewData(UnitTemplate& data, Tile* position, int owner);

public:
	static int weaponEffectivenessTable[N_WEAPONS][N_ARMOURS];
	static int movementTable[N_TILE_TYPES][N_MOVEMENT_TYPES];
	static std::map<char, UnitTemplate> unitTemplates;

	static bool isInitialized() { return _isInitialized; }

	

	static void dispose();

	static Unit* createUnit(char unitType, Tile* position, int owner, int funds);	//If the object pool is full, creates a new unit and returns it. Otherwise returns a unit with the new data assigned.

	static bool handleAttack(Unit* attacker, Unit* defender);	//Handles attacks. Returns true if the defender is alive, false otherwise.

	static int initialize(const char* fileName = "UnitTemplates.txt");	//Initializes unit templates and other unit metadata

	void setAttackableTiles(std::set<Tile*>& attackableTiles) { _attackableTiles.swap(attackableTiles); }
	void setValidTiles(std::set<Tile*>& validTiles) { _validTiles.swap(validTiles); }
	void setTileOrigins(std::map<Tile*, Tile*>& tileOrigins) { _tileOrigins.swap(tileOrigins); }

	bool canMoveTo(Tile* dest) { return _validTiles.find(dest) != _validTiles.end(); }
	bool canTraversePath(std::vector<Tile*>* path)
	{
		if (path && path->size() > 0)
		{
			int length = 0;

			for (auto tile : *path)
			{
				length += getMoveCost(tile);
			}

			return length <= _moveDistance;
		}
		return false;
	}

	std::set<Tile*>& getAttackableTiles();				//Returns the tiles the unit can potentially attack.
	std::set<Tile*>& getValidTiles();				//Returns the tiles the unit may move to.
	std::vector<Tile*>& getVision();					//Returns the tiles the unit can see.
	std::vector<Tile*>* getPathTo(Tile* tile);			//Returns the shortest path from the unit's current position to the tile if there is one. TODO: Remove

	bool isAlive() { return _isAlive; }
	bool hasMoved()	{	return _hasMoved;	}
	bool hasAttacked()	{	return _hasAttacked;	}
	void procTurnEnd(int availableFunds);
	char getType() { return _unitType; }
	Tile* getPosition()	{	return _position;	}
	int captureAmount();
	int getOwner()	{	return _owner;	}
	int getHealth()	{	return _health;	}
	int getMoveDist() { return _moveDistance; }
	int getMoveCost(Tile* tile);			//Determines the move cost of the specified tile
	WeaponType getWeapon() { return _weapon; }
	ArmourType getArmour() { return _armour; }
	int getRepairCost();
	void move(Tile* newPosition);
	void recalcualteVision();	//TODO: Remove and replace with recalculateVision.

	std::string getTextureName()	{	return _texture;	}
};