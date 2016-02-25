#include "StdAfx.h"
#include "Menu.h"

GLuint Menu::_menuTopBG = 0;
GLuint Menu::_menuMidBG = 0;
GLuint Menu::_menuBotBG = 0;

void Menu::initialize()
{
	AssetPool::loadShader("Menu Shader", "Shaders/Menu Shader.vsh", "Shaders/Menu Shader.fsh");
	AssetPool::loadTexture("Assets/Menu BG.png", "DefaultMenuBG");
	_menuTopBG = AssetPool::loadTexture("Assets/Menu BG.png", "MenuTopBG");
	_menuMidBG = AssetPool::loadTexture("Assets/MenuMidBG.png", "MenuMidBG");
	_menuBotBG = AssetPool::loadTexture("Assets/Menu BG.png", "MenuBotBG");
}


Menu::Menu(glm::vec3 position, size_t size)// : _position(position.x, position.y)
{
	_modelMatrix = glm::rotate(glm::mat4(1.0f), 3.14f, glm::vec3(1, 0, 0));
	_modelMatrix = glm::scale(glm::translate(_modelMatrix, position), glm::vec3(MENU_OPTION_WIDTH, MENU_OPTION_HEIGHT, 1.0f));
	_menuOptions.reserve(size);

	_position = Vector2(_modelMatrix[0][3] + 0.2f, _modelMatrix[1][3] + 0.5f);
	_selectionOption = 0;
}

void Menu::emplaceOption(std::string text, std::string texture)
{
	_menuOptions.emplace_back(text, texture);
}

int Menu::update(int ms)
{
	if (InputManager::isPressed(Select))
	{
		return _selectionOption;
	}
	else if (InputManager::isPressed(Cancel))
	{
		return MENU_EXIT;
	}
	else if (InputManager::isPressed(Up))
	{
		if (_selectionOption == 0)
		{
			_selectionOption = _menuOptions.size() - 1;
		}
		else
		{
			--_selectionOption;
		}
	}
	else if (InputManager::isPressed(Down))
	{
		if (++_selectionOption == _menuOptions.size())
		{
			_selectionOption = 0;
		}
	}

	return MENU_NO_ACTION;
}

//TODO
void Menu::draw()
{
	//Set default parameters
	auto menuShader = AssetPool::getShader("Menu Shader");

	glUseProgram(menuShader);
	glUniformMatrix4fv(glGetUniformLocation(menuShader, "projection"), 1, false, glm::value_ptr(*Camera::getProjection()));	//Set Projection
	glUniformMatrix4fv(glGetUniformLocation(menuShader, "model"), 1, false, glm::value_ptr(_modelMatrix));

	glm::mat4 model = _modelMatrix;

	GLint shaderModelVariable = glGetUniformLocation(menuShader, "model");
	GLint shaderTextVariable = glGetUniformLocation(menuShader, "isText");
	GLint shaderEnabledVariable = glGetUniformLocation(menuShader, "isEnabled");
	GLint shaderSelectedVariable = glGetUniformLocation(menuShader, "isSelected");

	Tile::bindBuffers();

	glUniform1i(shaderTextVariable, false);
	glUniform1i(shaderEnabledVariable, true);

	auto curPosition = _position;
	glRasterPos2f(_position.x, _position.y);
	auto drawTexture = [](GLint tex)
	{
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	};

	//Draw Menu Options
	
	//No menu options to draw.
	if (_menuOptions.size() == 0)
	{
		return;
	}
	else
	{

		for (size_t i = 0; i < _menuOptions.size(); i++)
		{
			if (_selectionOption == i)
			{
				glUniform4f(glGetUniformLocation(menuShader, "colour"), 0.0f, 1.0f, 1.0f, 1.0f);
			}
			else
			{
				glUniform4f(glGetUniformLocation(menuShader, "colour"), 1.0f, 1.0f, 1.0f, 1.0f);
			}

			glUniform1i(shaderEnabledVariable, _menuOptions[i].isEnabled());

			model = glm::translate(model, glm::vec3(0, 0, 0.1f));
			glUniformMatrix4fv(shaderModelVariable, 1, false, glm::value_ptr(model));

			if (_menuOptions[i].hasTexture())
			{
				glUniform1i(shaderTextVariable, false);
				drawTexture(_menuMidBG);
				model = glm::translate(model, glm::vec3(0, 0, -0.1f));
				glUniformMatrix4fv(shaderModelVariable, 1, false, glm::value_ptr(model));
				drawTexture(AssetPool::getTexture(_menuOptions[i].getName()));
			}
			else
			{
				glUniform1i(shaderTextVariable, false);
				drawTexture(_menuMidBG);
				glUniform1i(shaderTextVariable, true);
				model = glm::translate(model, glm::vec3(0, 0, -0.1f));
				glUniformMatrix4fv(shaderModelVariable, 1, false, glm::value_ptr(model));

				glUniform4f(glGetUniformLocation(menuShader, "colour"), 0.0f, 0.0f, 0.0f, 1.0f);
				glRasterPos2f(_position.x, _position.y);
				glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)_menuOptions[i].getText().c_str());
			}
			model = glm::translate(model, glm::vec3(0, MENU_OPTION_HEIGHT, 0));
			glUniformMatrix4fv(shaderModelVariable, 1, false, glm::value_ptr(model));
			//curPosition.y += 0.001f;
		}
	}
}
#pragma endregion



MenuOption::MenuOption(std::string text, std::string texture) : _text(text), _texture(texture)
{
	_hasTexture = texture != "";
	_isEnabled = true;
}