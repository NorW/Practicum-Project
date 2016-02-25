#pragma once
#include "StdAfx.h"
#include <fstream>
#include <iostream>
#define ERROR_LOG_FILE "Error Log.txt"

static void logError(std::string msg);
static void logError(char* msg);
static void clearLogFile();

void logError(std::string msg)
{
	auto writer = std::ofstream(ERROR_LOG_FILE, std::ios_base::out | std::ios_base::app);

	writer << msg << std::endl;

	writer.flush();
	writer.close();
}

void logError(char* msg)
{
	auto writer = std::ofstream(ERROR_LOG_FILE, std::ios_base::out | std::ios_base::app);

	writer << msg << std::endl;

	writer.flush();
	writer.close();
}

void clearLogFile()
{
	auto writer = std::ofstream(ERROR_LOG_FILE, std::ios_base::trunc);
}