#pragma once
#include "StdAfx.h"
#include <fstream>
#include <type_traits>

class Shader
{
private:
	GLuint _programID,	//Program ID
		_fSID,			//Fragment Shader ID
		_vSID;			//Vertex Shader ID
	
	bool readFile(const char* file, char** dest);

	void errCheck(GLuint shader);

public:
	Shader(const char* vertexShaderFileName, const char* fragmentShaderFileName);

	~Shader();

	inline GLuint getProgramID() {	return _programID;	}
};