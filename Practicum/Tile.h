#pragma once
#include "StdAfx.h"
#include <array>

#define TILE_N_NEIGHBORS 4
#define TILE_NORTH 0
#define TILE_EAST 1
#define TILE_SOUTH 2
#define TILE_WEST 3
#define TILE_UNOWNED -1

enum TerrainType;
std::string TerrainToString(TerrainType t);

//Valid terrain types for Tiles (excluding N_TILE_TYPES)
enum TerrainType
{
	City = 0,
	Factory,
	Road,
	Grass,
	Hill,
	Mountain,
	Swamp,
	N_TILE_TYPES
};



/*
Stores data for a tile.
Stores neighboring tiles, the tile's type, the tile's owner (if applicable), and the tile's capture status (if applicable)
*/
class Tile
{
private:

	static GLfloat vertices[8];

	static GLfloat texCoords[8];

	static GLuint indices[6];

	static GLuint _vao, _vbo[3];

	std::array<Tile*, TILE_N_NEIGHBORS> _neighbors;		//The neighboring tiles.
	TerrainType _type;									//The tile's type
	int _owner;											//The tile's owner (if applicable)
	int _originalOwner;									//The tile's original owner (if applicable)
	int _capturer;										//The current side attempting to capture the tile (if applicable)
	int _captureProgress;								//The current progress of capturing the tile (%) (if applicable)
	Vector2 _position;									//Position of the tile

public:

	static void generateVertexArrays();

	static void destroyVertexArrays();

	static void bindBuffers();

	static int getDistance(Tile* a, Tile* b)
	{
		return Vector2::manhattanDistance(a->_position, b->_position);
	}

	//Constructor for tile. Creates a tile of terrain type t owned by the side specified by owner.
	Tile(TerrainType t, int x, int y, int owner = TILE_UNOWNED);

	//Returns the current owner of the tile
	int getOwner()	{	return _owner;	}

	//Returns the type of the tile.
	TerrainType getType()	{	return _type;	}

	//Sets the neighbor specified by nNeighbor to neighbor.
	void setNeighbor(int nNeighbor, Tile* neighbor);

	//Returns the neighbor specified by nNeighbor. Returns 0 if the neighbor does not exists or if nNeighbor is an invalid neighbor.
	Tile* getNeighbor(int nNeighbor);

	//Returns the side currently attempting to capture the tile
	int getCapturingSide()	{	return _capturer;	}

	//Progresses the tile's capture status by the capturing side by captureAmount. Returns true if the tile has been successfully captured, false otherwise.
	bool capture(int capturingSide, int captureAmount);

	//Resets the tile's capture status
	void resetCaptureStatus();

	//Changes the tile's owner to TILE_UNOWNED
	void unAlly();

	void reset();

	Vector2& getPosition()	{	return _position;	}
};