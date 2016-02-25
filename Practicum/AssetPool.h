#pragma once
#include "StdAfx.h"
#include "SOIL.h"
#include "Shader.h"
#define ASP_ERROR_TEXTURE "ASP_ERROR_TEXTURE"
#define ASP_BLANK_TEXTURE "ASP_BLANK_TEXTURE"

#pragma comment(lib, "SOIL.lib")

/*
Asset pool used to load and store images and audio data.
Designed to allow the reuse of image and audio data over all classes as opposed to loading and storing multiple copies of the same files.
*/
class AssetPool
{
private:
	static std::map<std::string, GLuint> _textureMap;

	static std::map<std::string, std::shared_ptr<Shader>> _shaderMap;

	static GLuint _errorTex, _blankTex;

public:
	static void initialize();

	//load texture
	static GLuint loadTexture(std::string fileName, std::string name);

	static GLuint getTexture(std::string name);

	//load audio


	//Load Shader
	static GLuint loadShader(std::string name, const char* vertexShaderFile, const char* fragShaderFile);

	static GLuint getShader(std::string name);

	static void deleteAssets();
};