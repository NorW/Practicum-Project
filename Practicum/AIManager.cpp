#include "StdAfx.h"
#include "AIManager.h"

AIErrorCodes AIManager::_error = OK;
bool AIManager::_isRunning = false;
	
std::map<std::string, std::string> AIManager::_unitMetaAnalysis;

void AIManager::initialize()
{

}

bool AIManager::run(AI* ai)
{
	if(!ai)
	{
		_error = NullAI;
		return false;
	}

	if(_isRunning)
	{
		_error = MultipleAI;
		return false;
	}

	std::thread* aiThread = new std::thread(AIManager::runAI, ai);

	return true;
}

void AIManager::runAI(AI* ai)
{
	if(!ai || _isRunning)
	{
		return;
	}

	_isRunning = true;
	ai->update();
	_isRunning = false;
}
