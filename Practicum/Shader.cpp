#include "StdAfx.h"
#include "Shader.h"

Shader::Shader(const char* vSFName, const char* fSFName): _programID(0), _vSID(0), _fSID(0)
{
	char* vShader;
	char* fShader;

	if(readFile(vSFName, &vShader) && readFile(fSFName, &fShader))
	{
		//TODO: more error checking
		_vSID = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(_vSID, 1, (const GLchar**) &vShader, NULL);
		glCompileShader(_vSID);

		errCheck(_vSID);

		_fSID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(_fSID, 1, (const GLchar**) &fShader, NULL);
		glCompileShader(_fSID);

		errCheck(_fSID);

		free(vShader);
		free(fShader);

		_programID = glCreateProgram();
		glAttachShader(_programID, _vSID);
		glAttachShader(_programID, _fSID);
		glBindAttribLocation(_programID, 0, "in_position");
		glBindAttribLocation(_programID, 1, "in_texcoord");
		glLinkProgram(_programID);
	}
	else
	{
		::MessageBoxA(0, "Unable to read shader files", "error", MB_OK);
	}
}

Shader::~Shader()
{
	if (_programID)
	{
		if (_vSID)
		{
			glDetachShader(_programID, _vSID);
			glDeleteShader(_vSID);
		}

		if (_fSID)
		{
			glDetachShader(_programID, _fSID);
			glDeleteShader(_fSID);
		}

		glDeleteProgram(_programID);
	}
}

bool Shader::readFile(const char* file, char** dest)
{
	std::ifstream ifs;
	ifs.open(file, std::ios_base::ate | std::ios_base::binary);

	if(!ifs.is_open())	//If the file can not be read, return false;
	{
		return false;
	}

	int size = ifs.tellg();				//Get size of file in bytes

	char* data = new char[size + 1];	//Allocate space for data + 1 for null
	memset(data, 0, size + 1);			//Fill with 0 in case things go wrong
	ifs.close();
	ifs.clear();

	ifs.open(file);						//Open file at the beginning
	
	if(!ifs.is_open())	//If the file can not be read somehow, return false.
	{
		return false;
	}
	
	ifs.read(data, size);				//Read the entire file into data

	if(ifs.fail() && !ifs.eof())		//If reading fails, return false.
	{
		return false;
	}

	data[size] = 0;						//Set the last char to null

	(*dest) = data;						//Set the destination to point to data

	return true;
}

void Shader::errCheck(GLuint shader)
{
	GLint compileStatus, msgLen;
	char* err;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == FALSE)
	{
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &msgLen);

		err = (char*)malloc(msgLen);

		glGetShaderInfoLog(shader, msgLen, &msgLen, err);
		::MessageBoxA(0, err, "Shader Error", MB_OK);

		free(err);
	}
}