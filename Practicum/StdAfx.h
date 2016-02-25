#pragma once
//OpenGL headers/libraries
#include <gl\glew.h>	//Glew must come first
#pragma comment(lib, "glew32.lib")
#include <GL/freeglut.h>
#pragma comment(lib, "freeglut.lib")

//Misc Std Lib headers
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <memory>

//GL Math headers
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Vector.h"

//Debug/Error logging headers
#include "Logger.h"

#define DEBUG_MODE

#ifdef DEBUG_MODE

#include <fstream>

#endif