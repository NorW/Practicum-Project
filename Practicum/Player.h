#pragma once
#include "StdAfx.h"
#include <set>
#include "Tile.h"
#include "Unit.h"

class Player
{
protected:
	bool _isHuman;
	int _playerID;
	int _money;
	
	

public:
	explicit Player(bool isHuman);

	int requestFunds(int amount);

	inline int getFunds()	{	return _money;	}

	inline void setID(int id)	{	_playerID = id;	}

	inline int getID()	{	return _playerID;	}

	inline void setFunds(int amount)
	{
		_money = amount;
	}

	inline void addFunds(int amount)
	{
		_money += amount;
	}

	virtual void reset();
};