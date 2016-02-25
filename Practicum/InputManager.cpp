#include "StdAfx.h"
#include "InputManager.h"

int InputManager::_commandStatus[N_COMMANDS];
int InputManager::_prevStatus[N_COMMANDS];

std::map<int, bool> InputManager::_keyStatus;
std::map<int, InputCommands> InputManager::_inputMap;	

void InputManager::initialize()
{
	for(int i = 0; i < N_COMMANDS; i++)
	{
		_prevStatus[i] = _commandStatus[i] = 0;
	}

	_inputMap['w'] = Up;
	_inputMap['a'] = Left;
	_inputMap['s'] = Down;
	_inputMap['d'] = Right;

	_inputMap['\n'] = Select;
	_inputMap['x'] = Select;

	_inputMap['\b'] = Cancel;
	_inputMap['c'] = Cancel;
	_inputMap[27] = Cancel;		//Escape key

	_inputMap[GLUT_KEY_LEFT + IM_SPKEYOFFSET] = Left;
	_inputMap[GLUT_KEY_RIGHT + IM_SPKEYOFFSET] = Right;
	_inputMap[GLUT_KEY_UP + IM_SPKEYOFFSET] = Up;
	_inputMap[GLUT_KEY_DOWN + IM_SPKEYOFFSET] = Down;

#ifdef DEBUG_MODE
	_inputMap['`'] = DebugToggle;
	_inputMap['1'] = DebugSwitchView1;
	_inputMap['2'] = DebugSwitchView2;
	_inputMap['3'] = DebugSwitchViewAll;
	_inputMap['p'] = DebugToggleAIPause;
	_inputMap['n'] = DebugAINextStep;
	_inputMap['0'] = TEMPENDTURN;
#endif

	for(std::map<int, InputCommands>::iterator it = _inputMap.begin(); it != _inputMap.end(); ++it)
	{
		_keyStatus[(*it).first] = false;
	}
}

/*
	Handles key presses. If a registered key is pressed and has not already been pressed, it increments the status of the associated command by 1.
	The status of each command is stored as an integer rather than a boolean to keep track of multiple key presses.
	A command is only registered as released if and only if none of its associated keys are pressed.
	A command is registered as pressed if one or more of its associated keys are pressed.
*/
void InputManager::keyDown(unsigned char key, int x, int y)
{
	//If the key is registered and not already down
	if(_inputMap.find(key) != _inputMap.end() && !_keyStatus[key])
	{
		++_commandStatus[_inputMap[key]];	//Increment key
		_keyStatus[key] = true;				//Set status of key to true.
	}
}

void InputManager::keyUp(unsigned char key, int x, int y)
{
	if(_inputMap.find(key) != _inputMap.end() && _keyStatus[key])
	{
		_keyStatus[key] = false;
		--_commandStatus[_inputMap[key]];
	}
}

void InputManager::spKeyDown(int key, int x, int y)
{
	if(_inputMap.find(key + IM_SPKEYOFFSET) != _inputMap.end() && !_keyStatus[key + IM_SPKEYOFFSET])
	{
		++_commandStatus[_inputMap[key + IM_SPKEYOFFSET]];
		_keyStatus[key + IM_SPKEYOFFSET] = true;
	}
}

void InputManager::spKeyUp(int key, int x, int y)
{
	if(_inputMap.find(key + IM_SPKEYOFFSET) != _inputMap.end() && _keyStatus[key + IM_SPKEYOFFSET])
	{
		_keyStatus[key + IM_SPKEYOFFSET] = false;
		--_commandStatus[_inputMap[key + IM_SPKEYOFFSET]];
	}
}

void InputManager::procUpdate()
{
	memcpy(_prevStatus, _commandStatus, sizeof(int) * N_COMMANDS);
}

bool InputManager::isUp(InputCommands cmd)
{
	if(cmd >= 0 && cmd < N_COMMANDS)
	{
		return _commandStatus[cmd] == 0;
	}

	return false;
}

bool InputManager::isDown(InputCommands cmd)
{
	if(cmd >= 0 && cmd < N_COMMANDS)
	{
		return _commandStatus[cmd] != 0;
	}

	return false;
}

bool InputManager::isPressed(InputCommands cmd)
{
	if(cmd >= 0 && cmd < N_COMMANDS)
	{
		return _commandStatus[cmd] == 0 && _prevStatus[cmd] != 0;
	}

	return false;
}