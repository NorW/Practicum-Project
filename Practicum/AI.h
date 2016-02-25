#pragma once
#include "StdAfx.h"
#include <mutex>
#include <condition_variable>
#include "Player.h"
#include "Map.h"

struct AIProfile
{
	int ownBuildingThreatNumber;
	int otherBuildingThreatNumber;
	int unitThreatNumber;
	int capturingUnitThreatNumber;
};

class AI : public Player
{
private:
	Map* _map;

	int _ownBuildingThreatNumber;
	int _otherBuildingThreatNumber;
	int _unitThreatNumber;
	int _capturingUnitThreatNumber;
	bool _isRunning;

#ifdef DEBUG_MODE
	std::mutex _pauseLock;
	std::condition_variable _cv;
	bool _debugMode;
	bool _isPaused;
	Tile* _debugTargetTile;
#endif

	std::map<char, char> _counterLookup;

	void setupCounters();

	std::shared_ptr<std::vector<Tile*>> findShortestPath(Unit* unit, Tile* from, Tile* to);

	void buildCapturingUnits(std::set<Tile*>& ownBuildings, std::map<Tile*, int>& interestRating);

	char getOpposingType(char unitType)
	{
		if (_counterLookup.find(unitType) != _counterLookup.end())
		{
			return _counterLookup[unitType];
		}
		return ' ';
	}
	

	//Inserts the unit into threatVector based on rating in decending order.
	void insertThreat(Unit* unit, int rating, std::vector<std::pair<Unit*, int>>& threatVector)
	{
		//Insert threat in order of threat number in decending order.
		for (auto it = threatVector.begin(); it != threatVector.end(); ++it)
		{
			if (rating > it->second)
			{
				threatVector.emplace(it, unit, rating);
				return;
			}
		}

		//If the rating is the lowest in the vector (or the vector is empty) insert it at the end.
		threatVector.emplace_back(unit, rating);
	}

public:
	AI();


#ifdef DEBUG_MODE
	void toggleDebugMode(bool on) { _debugMode = on; if (!_debugMode) { _isPaused = false; _cv.notify_all(); } }
	void debugNextStep() { _cv.notify_all(); }
	bool debugIsPaused() { return _isPaused; }
	void togglePause(bool paused) { if (_debugMode) { _isPaused = paused; } }
	Tile* debugGetTargetTile() 
	{
		if (_debugMode) 
		{
			return _debugTargetTile;
		}
		return 0;
	}

#endif

	void setProfile(AIProfile& profile)
	{
		_ownBuildingThreatNumber = profile.ownBuildingThreatNumber;
		_otherBuildingThreatNumber = profile.otherBuildingThreatNumber;
		_unitThreatNumber = profile.unitThreatNumber;
		_capturingUnitThreatNumber = profile.capturingUnitThreatNumber;
	}

	void setMap(Map* map) { _map = map; setupCounters(); }

	bool isRunning() { return _isRunning; }

	int update();
};