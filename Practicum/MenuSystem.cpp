#include "StdAfx.h"
#include "MenuSystem.h"



MenuSystem::MenuSystem()
{
	_mapNames.push_back("Maps/testmap.txt");
	_mapNames.push_back("Maps/testmap2.txt");
	_mapNames.push_back("Maps/testmap3.txt");
	_mapNames.push_back("Maps/testmap4.txt");
	_mapNames.push_back("Maps/testmap5.txt");
	_mapNames.push_back("Maps/testmap6.txt");
}

std::string MenuSystem::getSelectedMap()
{
	if (_curOption < 0 || _curOption >= _mapNames.size())
	{
		logError("Internal Logic Error: Invalid Map Index");
		return std::string("testmap.txt");
	}
	
	return _mapNames[_curOption];
}

void MenuSystem::setup()
{
	GLuint shader = AssetPool::getShader("Menu Shader");
	AssetPool::loadTexture("Assets/StartBtn.png", "StartBtn");
	AssetPool::loadTexture("Assets/OptionsBtn.png", "OptionBtn");
	AssetPool::loadTexture("Assets/Exit.png", "Exit");

	if(shader)
	{
		glUseProgram(shader);
	}

	_curMenu = Main;
	_curOption = 0;

	_menus[Main] = new Menu(glm::vec3(0, 0, 0), 3);
	_menus[Main]->emplaceOption("Levels", "StartBtn");
	_menus[Main]->emplaceOption("Options", "OptionBtn");
	_menus[Main]->emplaceOption("Exit", "Exit");
	_menus[Main]->toggleOption(1, false);

	_menus[Options] = new Menu(glm::vec3(), 1);



	_menus[Level] = new Menu(glm::vec3(0.2f, -3.5f, 0), 6);
	_menus[Level]->emplaceOption("Level 1");
	_menus[Level]->emplaceOption("Level 2");
	_menus[Level]->emplaceOption("Level 3");
	_menus[Level]->emplaceOption("Level 4");
	_menus[Level]->emplaceOption("Level 5");
	_menus[Level]->emplaceOption("Level 6");
	_menus[Level]->emplaceOption("Exit");
}

int MenuSystem::update(int ms)
{
	//TODO: Update depending on current menu
	int result = 0;

	switch (_curMenu)
	{
	case Main:
		result = _menus[Main]->update(ms);
		if (result == MENU_EXIT)
		{
			_menus[Main]->setSelectedOption(2);
		}
		else if (result == 0)
		{
			_curMenu = Level;
		}
		else if (result == 2)
		{
			return MSSTATUS_EXIT;
		}
		break;

	case Options:
		result = _menus[Options]->update(ms);
		break;

	case Level:
		result = _menus[Level]->update(ms);
		if (result == MENU_EXIT)
		{
			_curMenu = Main;
			_menus[Level]->setSelectedOption(0);
		}
		else if (result == _menus[Level]->getSize() - 1)
		{
			_curMenu = Main;
			_menus[Level]->setSelectedOption(0);
		}
		else if (result != MENU_NO_ACTION)
		{
			_curOption = result;
			return MSSTATUS_GAME;
		}
		break;
	}

	return MSSTATUS_CONTINUE;
}

void MenuSystem::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_INDEX_ARRAY);

	//Enable attribute arrays 0 and 1 for vertex and tex coords
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//TODO: Draw appropriate menu
	_menus[_curMenu]->draw();

	glUseProgram(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);

	glFlush();
	glutSwapBuffers();
}
