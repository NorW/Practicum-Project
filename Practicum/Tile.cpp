#include "StdAfx.h"
#include "Tile.h"

GLfloat Tile::vertices[8] = { 0.0f, 0.0f,
						1.0f, 0.0f,
						1.0f, 1.0f,
						0.0f, 1.0f };


GLfloat Tile::texCoords[8] = {0.0f, 0.0f,
					1.0f, 0.0f,
					1.0f, 1.0f,
					0.0f, 1.0f};

GLuint Tile::indices[6] = {0, 1, 2,
					2, 3, 0};

GLuint Tile::_vao = 0;

GLuint Tile::_vbo[3] = { 0, 0, 0 };

std::string TerrainToString(TerrainType t)
{
	std::string returnVal;
	switch(t)
	{
	case City:
		returnVal = "City";
		break;
	case Factory:
		returnVal = "Factory";
		break;
	case Road:
		returnVal = "Road";
		break;
	case Grass:
		returnVal = "Grass";
		break;
	case Hill:
		returnVal = "Hill";
		break;
	case Mountain:
		returnVal = "Mountain";
		break;
	case Swamp:
		returnVal = "Swamp";
		break;
	default:
		returnVal = "INVALID";
	}
	return returnVal;
}

void Tile::generateVertexArrays()
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	
	glGenBuffers(3, _vbo);
}

void Tile::destroyVertexArrays()
{
	glDeleteBuffers(3, _vbo);
	glDeleteVertexArrays(1, &_vao);
}

void Tile::bindBuffers()
{
	glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);
}

//Creates a tile based on t and owner.
Tile::Tile(TerrainType t, int x, int y, int owner): _position(Vector2(x, y))
{
	if(owner != TILE_UNOWNED && (t == City || t == Factory))	//If the tile is owned, and is a city or factory, then the tile can have an owner
	{
		_owner = owner;
	}
	else
	{
		_owner = TILE_UNOWNED;
	}

	_originalOwner = _owner;

	if(t >= N_TILE_TYPES || t < 0)
	{
		_type = Mountain;
	}
	else
	{
		_type = t;
	}

	_neighbors.fill(0);
}

//Sets the neighbor at nNeighbor to neighbor
void Tile::setNeighbor(int nNeighbor, Tile* neighbor)
{
	//Assignes neighbor to neighbor at nNeighbor if nNeighbor is a valid index
	if(nNeighbor >= 0 && nNeighbor < TILE_N_NEIGHBORS)
	{
		_neighbors[nNeighbor] = neighbor;
	}
}

//Returns the neighboring tile at the specified index.
Tile* Tile::getNeighbor(int nNeighbor)
{
	//If nNeighbor is a valid index, return the neighbor at that index. Else return 0.
	if(nNeighbor >= 0 && nNeighbor < TILE_N_NEIGHBORS)
	{
		return _neighbors[nNeighbor];
	}
	else
	{
		return 0;
	}
}

//Captures the tile by captureAmount for the side numbered capturingSide
bool Tile::capture(int capturingSide, int captureAmount)
{
	//Only cities and factories can be captured.
	if(_type != City && _type != Factory)
	{
		return false;
	}

	//Tiles cannot be captured by the owner of the tile or no owner
	if(capturingSide == _owner || capturingSide == TILE_UNOWNED)
	{
		return false;
	}

	//If the tile is being captured by another side, reset the progress.
	if(capturingSide != _capturer)
	{
		_capturer = capturingSide;
		_captureProgress = 0;
	}

	_captureProgress += captureAmount;

	//If the tile has been captured, set the owner
	if(_captureProgress >= 100)
	{
		_owner = capturingSide;
		resetCaptureStatus();
		return true;
	}
	return false;
}

//Resets the tile's captured status
void Tile::resetCaptureStatus()
{
	_capturer = TILE_UNOWNED;
	_captureProgress = 0;
}

//Tile is no longer owned
void Tile::unAlly()
{
	_owner = TILE_UNOWNED;
}

void Tile::reset()
{
	_owner = _originalOwner;
	resetCaptureStatus();
}