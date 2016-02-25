#pragma once
#include "StdAfx.h"
#define IM_SPKEYOFFSET 256


#ifdef DEBUG_MODE
enum InputCommands
{
	Up,
	Down,
	Left,
	Right,
	Select,
	Cancel,
	DebugToggle,
	DebugSwitchView1,
	DebugSwitchView2,
	DebugSwitchViewAll,
	DebugToggleAIPause,
	DebugAINextStep,
	TEMPENDTURN,
	N_COMMANDS
};
#else
enum InputCommands
{
	Up,
	Down,
	Left,
	Right,
	Select,
	Cancel,
	N_COMMANDS
};
#endif

/*
Handles the status of input by associating input with commands and ignoring all non registered keys.
The status of each command is stored as an int, representing how many key associated to the command are down at the time.
A command is considered down if one or more associated keys is down.
A command is considered up if and only if all of the associated keys are up.
A command is considered pressed if, in the status of the previous update, one or more of the associated keys are down, and in the current update, none of the associated keys are down.
*/
class InputManager
{
private:
	static int _commandStatus[N_COMMANDS];							//Current state of commands. Keeps track of the number of keys that are down for their associated commands.
	static int _prevStatus[N_COMMANDS];								//Previous state of commands.
	static std::map<int, bool> _keyStatus;							//Current state of keys
	static std::map<int, InputCommands> _inputMap;					//Map of input to commands

public:
	static void initialize();

	static void keyDown(unsigned char key, int x, int y);			//Handles key presses
	static void keyUp(unsigned char key, int x, int y);				//Handles key releases
	static void spKeyDown(int key, int x, int y);					//Handles special key presses
	static void spKeyUp(int key, int x, int y);						//Handle special key releases
	
	static void procUpdate();

	static bool isUp(InputCommands cmd);
	static bool isDown(InputCommands cmd);
	static bool isPressed(InputCommands cmd);
};