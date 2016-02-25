#include "StdAfx.h"
#include "AssetPool.h"

std::map<std::string, GLuint> AssetPool::_textureMap;
std::map<std::string, std::shared_ptr<Shader>> AssetPool::_shaderMap;

GLuint AssetPool::_errorTex = 0;
GLuint AssetPool::_blankTex = 0;

void AssetPool::initialize()
{
	_errorTex = loadTexture("Assets/Error.png", ASP_ERROR_TEXTURE);
	_blankTex = loadTexture("Assets/Blank.png", ASP_BLANK_TEXTURE);
	loadTexture("Assets/Selector.png", "Selector");
}

GLuint AssetPool::loadTexture(std::string fileName, std::string name)
{
	FILE* fp = fopen(fileName.c_str(), "r");
	if(!fp)
	{
		logError(std::string("AP: Failed to Read Tex File: ") + fileName);
		return _errorTex;
	}
	fclose(fp);

	int width, height;
	unsigned char* data = SOIL_load_image(fileName.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
	GLuint newTexture;
	glGenTextures(1, &newTexture);
	glBindTexture(GL_TEXTURE_2D, newTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	SOIL_free_image_data(data);

	if(newTexture != 0)
	{
		_textureMap[name] = newTexture;
	}
	else
	{
		logError(std::string("AP: Failed to Load Texture: ") + name);
		newTexture = _errorTex;
	}

	return newTexture;
}

GLuint AssetPool::getTexture(std::string name)
{
	if(_textureMap.find(name) != _textureMap.end())
	{
		return _textureMap[name];
	}
	else
	{
		return _errorTex;
	}
}


GLuint AssetPool::loadShader(std::string name, const char* vertexShaderFile, const char* fragShaderFile)
{
	if(_shaderMap.find(name) == _shaderMap.end())
	{
		std::shared_ptr<Shader> shader = std::make_shared<Shader>(vertexShaderFile, fragShaderFile);

		if(shader && shader->getProgramID())
		{
			_shaderMap[name] = shader;
			return shader->getProgramID();
		}
	}

	return 0;
}

GLuint AssetPool::getShader(std::string name)
{
	if(_shaderMap.find(name) != _shaderMap.end())
	{
		return _shaderMap[name]->getProgramID();
	}

	return 0;
}

void AssetPool::deleteAssets()
{
	_shaderMap.clear();
}