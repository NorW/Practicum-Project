#pragma once
#include "StdAfx.h"
#include <thread>
#include "AI.h"

enum AIErrorCodes
{
	OK = 0,
	NullAI,
	MultipleAI
};

class AIManager
{
private:
	static AIErrorCodes _error;
	static bool _isRunning;
	
	static std::map<std::string, std::string> _unitMetaAnalysis;

	static void runAI(AI* ai);

public:
	static void initialize();

	static bool run(AI* ai);

	static AIErrorCodes getError()	{	return _error;	}

	static void resetError()	{	_error = OK;	}

	static bool isRunning()	{	return _isRunning;	}
};

