#include "StdAfx.h"
#include <stdlib.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "InputManager.h"
#include "GameController.h"
#include "AIManager.h"
#include "Map.h"


void setup(int argc, char** argv);
void draw();
void idle();
void destroy();

GameController* gameController;

int main(int argc, char** argv)
{
	clearLogFile();
	setup(argc, argv);

	int prevTime, curTime, elapsedTime;
	bool running = true;

	prevTime = glutGet(GLUT_ELAPSED_TIME);

	while(running && gameController)
	{
		curTime = glutGet(GLUT_ELAPSED_TIME);				//Get the current time
		elapsedTime = curTime - prevTime;					//Calculate elapsed time
		prevTime = curTime;

		running = gameController->update(elapsedTime);		//Update the game
		InputManager::procUpdate();							//Update the input manager
		glutMainLoopEvent();								//Glut event loop
		glutPostRedisplay();								//Redraw
	}
	

	destroy();
}

void setup(int argc, char** argv)
{
	InputManager::initialize();		//Set up input manager

	glutInit(&argc, argv);															//Initialize glut
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);			//Set display mode
	glutInitWindowSize(1200, 800);													//Set window size
	glutCreateWindow("Practicum");													//Create window
	glClearColor(0.2f, 0.2f, 0.45f, 1);												//Set background colour
	glewInit();

	AssetPool::initialize();		//Initilize asset pool
	Unit::initialize();				//Set up unit templates
	AIManager::initialize();		//Set up AI manager
	Menu::initialize();

	//Set up input functions
	glutKeyboardFunc(InputManager::keyDown);
	glutKeyboardUpFunc(InputManager::keyUp);
	glutSpecialFunc(InputManager::spKeyDown);
	glutSpecialUpFunc(InputManager::spKeyUp);

	Tile::generateVertexArrays();	//Set up tile draw data
	
	//Set up OpenGL stuff
	glShadeModel(GL_SMOOTH);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);	//Perspective reaaally doesnt matter for this project
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//Transparency blend function

	auto orthoProjectionMatrix = std::make_shared<glm::mat4>(glm::mat4(200.0f/1200.0f, 0, 0, 0,
		0, 0.25f, 0, 0,
		0, 0, -0.5f, 0,
		0, 0, 0, 1));
	Camera::setProjection(orthoProjectionMatrix);

	auto viewMatrix = std::make_shared<glm::mat4>(glm::lookAt(glm::vec3(0.5f, 0.5f, -1.0f), glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	Camera::setView(viewMatrix);

	glutIdleFunc(idle);

	glutDisplayFunc(draw);

	Map::initialize();
	gameController = new GameController();
}


void draw()
{
	if(gameController)
	{
		gameController->draw();
	}
}

void idle()
{
	glutPostRedisplay();
}

void destroy()
{
	if(gameController)
	{
		delete gameController;
	}

	Map::destroyMaps();
	Unit::dispose();
	AssetPool::deleteAssets();
	Tile::destroyVertexArrays();
}