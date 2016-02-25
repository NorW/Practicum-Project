#include "StdAfx.h"
#include "HUD.h"

HUD* HUD::_gameHUD = new HUD();

HUD::HUD()
{
	_mainMenuOpen = _infoMenuOpen = _isFactorySelected = _unitMenuOpen = false;	//Start with no menus open
	_priceVec.clear();
	
	_mainMenu = std::make_unique<Menu>(glm::vec3(-4, -2, 0), 3);
	_mainMenu->emplaceOption("End Turn");
	_mainMenu->emplaceOption("Back to Level Select");
	_mainMenu->emplaceOption("Cancel");

	_factoryMenu = std::make_unique<Menu>(glm::vec3(-4, -2, 0), 0);

	_unitMenu = std::make_unique<Menu>(glm::vec3(-4, -2, 0), 3);
	_unitMenu->emplaceOption("Attack");
	_unitMenu->emplaceOption("Move");
	_unitMenu->emplaceOption("Cancel");

	_attackMenu = std::make_unique<Menu>(glm::vec3(-4, -2, 0), 2);
	_attackMenu->emplaceOption("Attack");
	_attackMenu->emplaceOption("Cancel");

	_unitCaptureMenu = std::make_unique<Menu>(glm::vec3(-4, -2, 0), 4);
	_unitCaptureMenu->emplaceOption("Capture");
	_unitCaptureMenu->emplaceOption("Attack");
	_unitCaptureMenu->emplaceOption("Move");
	_unitCaptureMenu->emplaceOption("Cancel");

	_attackCaptureMenu = std::make_unique<Menu>(glm::vec3(-4, -2, 0), 3);
	_attackCaptureMenu->emplaceOption("Capture");
	_attackCaptureMenu->emplaceOption("Attack");
	_attackCaptureMenu->emplaceOption("Cancel");

}

void HUD::setUnitTemplates()
{
	_factoryMenu->clear();
	_priceVec.clear();
	_priceVec.reserve(Unit::unitTemplates.size());
	for (auto it : Unit::unitTemplates)
	{
		_factoryMenu->emplaceOption(it.second.name + "\t" + static_cast<std::ostringstream*>(&(std::ostringstream() << it.second.unitCost))->str());
		_priceVec.emplace_back(it.first, it.second.unitCost);
	}
	_factoryMenu->emplaceOption("Cancel");
}

int HUD::handleMainMenu()
{
	int input = _mainMenu->update(16);

	switch (input)
	{
	case MENU_EXIT:
		_mainMenuOpen = false;
		return 0;

	case MENU_NO_ACTION:
		return 0;

	case 0:
		_mainMenuOpen = false;
		return HUD_END_TURN;

	case 1:
		_mainMenuOpen = false;
		return HUD_EXIT_GAME;

	case 2:
		_mainMenuOpen = false;
		return 0;

	default:
		return 0;
	}
}

int HUD::handleFactory()
{
	auto result = _factoryMenu->update(16);

	if (result == _factoryMenu->getSize() - 1 || result == MENU_EXIT)
	{
		_isFactorySelected = false;
	}
	else if (result != MENU_NO_ACTION)
	{
		_toBuild = result;
		_isFactorySelected = false;
		return HUD_BUILD_UNIT;
	}

	return 0;
}

int HUD::handleAttackMenu()
{
	auto type = _unitSelected->getPosition()->getType();
	int result;
	if (_unitSelected->getPosition()->getOwner() != _unitSelected->getOwner() && (type == Factory || type == City))
	{
		result = _attackCaptureMenu->update(16);

		switch (result)
		{
		case 0:
			_attackMenuOpen = false;
			return HUD_CAPTURE_TILE;

		case 1:
			_attackMenuOpen = false;
			return HUD_ATTACK_UNIT;

		case MENU_EXIT:
		case 2:
			_attackMenuOpen = false;
			break;

		default:
			break;
		}
	}
	else
	{
		result = _attackMenu->update(16);

		switch (result)
		{
		case 0:
			_attackMenuOpen = false;
			return HUD_ATTACK_UNIT;

		case MENU_EXIT:
		case 1:
			_attackMenuOpen = false;
			break;

		default:
			break;
		}
	}

	return 0;
}

int HUD::handleUnit()
{
	int result;

	auto type = _unitSelected->getPosition()->getType();

	if (_unitSelected->getPosition()->getOwner() != _unitSelected->getOwner() && (type == Factory || type == City))
	{
		result = _unitCaptureMenu->update(16);
		
		switch (result)
		{
		case 0:
			_unitMenuOpen = false;
			return HUD_CAPTURE_TILE;

		case 1:
			_unitMenuOpen = false;
			return HUD_ATTACK_UNIT;

		case 2:
			_unitMenuOpen = false;
			return HUD_MOVE_UNIT;

		case MENU_EXIT:
		case 3:
			_unitMenuOpen = false;
			break;

		default:
			break;
		}
	}
	else
	{
		result = _unitMenu->update(16);

		switch (result)
		{
		case 0:
			_unitMenuOpen = false;
			return HUD_ATTACK_UNIT;

		case 1:
			_unitMenuOpen = false;
			return HUD_MOVE_UNIT;

		case MENU_EXIT:
		case 2:
			_unitMenuOpen = false;
			break;

		default:
			break;
		}
	}

	return 0;
}

int HUD::update()
{
	int result = 0;
	if (_mainMenuOpen)
	{
		result = handleMainMenu();
	}
	else if (_isFactorySelected)
	{
		result = handleFactory();
	}
	else if (_unitMenuOpen)
	{
		result = handleUnit();
	}
	else if (_attackMenuOpen)
	{
		result = handleAttackMenu();
	}
	return result;
}

void HUD::draw(size_t money)
{
	GLuint shader;

	//Enable vertex and index arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_INDEX_ARRAY);

	//Enable attribute arrays 0 and 1 for vertex and tex coords
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	shader = AssetPool::getShader("Tile Shader");	//Get shader
	glUseProgram(shader);

	//Set up shader info
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, glm::value_ptr(*Camera::getProjection()));	//Set Projection
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, 
		glm::value_ptr(glm::translate(*Camera::getView(),										//Translate camera view by tile position for the modelview matrix
			glm::vec3((*_selectedTile)->getPosition().x, (*_selectedTile)->getPosition().y, -0.1f))));

	glUniform1i(glGetUniformLocation(shader, "isVisible"), true);
	glUniform1i(glGetUniformLocation(shader, "isOwned"), false);

	//Draw selected tile box
	Tile::bindBuffers();												//Selection highlight is the same size as tile, so use the same data as tile
	glBindTexture(GL_TEXTURE_2D, AssetPool::getTexture("Selector"));	//Bind the texture

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	//Draw selector

#ifdef DEBUG_MODE
	if (_targetTile)
	{
		glUniform1i(glGetUniformLocation(shader, "isOwned"), true);
		glUniform4f(glGetUniformLocation(shader, "sideColour"), 1.0f, 1.0f, 0.0f, 0.5f);
		glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false,
			glm::value_ptr(glm::translate(*Camera::getView(),
				glm::vec3(_targetTile->getPosition().x, _targetTile->getPosition().y, -0.1f))));
		glBindTexture(GL_TEXTURE_2D, AssetPool::getTexture(ASP_BLANK_TEXTURE));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
#endif

	if (_highlightType != 0)
	{
		if (_highlightType == 1)
		{
			glUniform4f(glGetUniformLocation(shader, "sideColour"), 0.0f, 0.5f, 0.5f, 0.3f);
		}
		else if (_highlightType == 2)
		{
			glUniform4f(glGetUniformLocation(shader, "sideColour"), 1.0f, 0.0f, 0.0f, 0.3f);
		}
		glUniform1i(glGetUniformLocation(shader, "isOwned"), true);
		glBindTexture(GL_TEXTURE_2D, AssetPool::getTexture(ASP_BLANK_TEXTURE));
		for (auto tile : _highlightedTiles)
		{
			if (tile)
			{
				glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false,
					glm::value_ptr(glm::translate(*Camera::getView(),
						glm::vec3(tile->getPosition().x, tile->getPosition().y, -0.1f))));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//Set texture parameters
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		}
	}

	//Draw money
	shader = AssetPool::getShader("Menu Shader");
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, glm::value_ptr(*Camera::getProjection()));	//Set Projection
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, false, glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 1))));
	glUniform4f(glGetUniformLocation(shader, "colour"), 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform1i(glGetUniformLocation(shader, "isText"), true);
	glUniform1i(glGetUniformLocation(shader, "isEnabled"), true);
	glRasterPos2d(-5.9f, 3.5f);
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)(("Money: " + static_cast<std::ostringstream*>(&(std::ostringstream() << money))->str()).c_str()));
	
	if (_gameOver)
	{
		glRasterPos2d(3.0f, 3.5f);
		glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"GAME OVER");
		glRasterPos2d(2.4f, 3.0f);
		glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"PRESS X TO CONTINUE");
	}

#ifdef DEBUG_MODE
	glRasterPos2d(-5.9f, 3);
	glUniform4f(glGetUniformLocation(shader, "colour"), 1.0f, 1.0f, 1.0f, 1.0f);
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"Press '0' for next turn.");
	glRasterPos2d(-5.9f, 2.5f);
	if (_debugOn)
	{
		glUniform4f(glGetUniformLocation(shader, "colour"), 0.2f, 1.0f, 0.2f, 1.0f);
		glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"Debug Mode On, Press ` to toggle");
		glRasterPos2d(-5.9f, 2);
		if (_view != 2)
		{
			glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)(("Current View: " + static_cast<std::ostringstream*>(&(std::ostringstream() << _view))->str()).c_str()));
		}
		else
		{
			glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"Current View: All");
		}
		glRasterPos2d(-5.9f, 1.5f);
		if (_paused)
		{
			glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"AI Paused, Press 'p' to unpause or 'n' to step through.");
		}
		else
		{
			glUniform4f(glGetUniformLocation(shader, "colour"), 1.0f, 0.2f, 0.2f, 1.0f);
			glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"Press 'p' to pause AI");
		}
	}
	else
	{
		glUniform4f(glGetUniformLocation(shader, "colour"), 1.0f, 0.2f, 0.2f, 1.0f);
		glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"Debug Mode Off, Press ` to toggle");
	}
#endif

	

	//Draw whichever menu is open.
	if (_unitMenuOpen)
	{
		auto type = _unitSelected->getPosition()->getType();
		if (_unitSelected->getPosition()->getOwner() != _unitSelected->getOwner() && (type == Factory || type == City))
		{
			_unitCaptureMenu->draw();
		}
		else
		{
			_unitMenu->draw();
		}
	}
	else if (_isFactorySelected)
	{
		_factoryMenu->draw();
	}
	else if (_mainMenuOpen)
	{
		_mainMenu->draw();
	}
	else if (_attackMenuOpen)
	{
		auto type = _unitSelected->getPosition()->getType();
		if (_unitSelected->getPosition()->getOwner() != _unitSelected->getOwner() && (type == Factory || type == City))
		{
			_attackCaptureMenu->draw();
		}
		else
		{
			_attackMenu->draw();
		}
	}

	//Disable stuff
	glUseProgram(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
}