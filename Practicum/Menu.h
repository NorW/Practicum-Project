#pragma once
#include "StdAfx.h"
#include <glm\gtc\bitfield.hpp>
#include "Tile.h"	//Temporarily include tile because im too lazy to set up buffers
#include "Camera.h"
#include "AssetPool.h"
#include "InputManager.h"
#define MS_INVALID_MENU_OPTION "!A@sd#$"
#define MENU_NO_ACTION -1
#define MENU_EXIT -2
#define MENU_OPTION_HEIGHT 1
#define MENU_OPTION_WIDTH 3

class MenuOption;
class Menu;


class MenuOption
{
private:
	bool _isEnabled, _hasTexture;
	std::string _texture, _text;

public:
	//Create a MenuOption object with the specified text and an optional texture.
	explicit MenuOption(std::string text, std::string texture = "");

	inline const std::string& getName()
	{
		return _texture;
	}

	inline void setEnabled(bool enabled)
	{
		_isEnabled = enabled;
	}

	inline bool isEnabled()
	{
		return _isEnabled;
	}

	inline bool hasTexture()
	{
		return _hasTexture;
	}

	inline const std::string& getText()
	{
		return _text;
	}
};

class Menu
{
private:
	static GLuint _menuTopBG, _menuBotBG, _menuMidBG;	//Textures for the menu background for the top, bottom, and background textures to draw in between.

	std::vector<MenuOption> _menuOptions;
	glm::mat4 _modelMatrix;		//Model matrix
	size_t _selectionOption;
	std::string _texture;
	Vector2 _position;

public:
	static void initialize();

	//Create a menu with the starting position specified by position. The size of the menu is determined by the number of options. Size specifys how many menuoptions to start with.
	explicit Menu(glm::vec3 position, size_t size = 0);

	void emplaceOption(std::string text, std::string texture = "");

	void clear()
	{
		_selectionOption = 0;
		_menuOptions.clear();
	}

	inline const std::string& getOption(size_t option)
	{
		if (option < _menuOptions.size())
		{
			return _menuOptions[option].getName();
		}
		return MS_INVALID_MENU_OPTION;
	}

	inline const size_t getSize()
	{
		return _menuOptions.size();
	}

	inline void toggleOption(size_t option, bool enabledStatus)
	{
		if (option < _menuOptions.size())
		{
			_menuOptions[option].setEnabled(enabledStatus);
		}
	}

	inline void toggleAllOptions(bool enabledStatus)
	{
		for (auto option : _menuOptions)
		{
			option.setEnabled(enabledStatus);
		}
	}

	inline void setSelectedOption(size_t option)
	{
		_selectionOption = option;
	}

	inline size_t getSelectedOption()
	{
		return _selectionOption;
	}

	int update(int ms);

	void draw();
};

