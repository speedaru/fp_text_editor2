#pragma once
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <stdarg.h>
#include <format>

#include "macros.h"


namespace logging {
	// Log Levels
	typedef enum {
		LOG_TRACE,
		LOG_DEBUG,
		LOG_INFO,
		LOG_WARN,
		LOG_ERROR
	} LogLevel;

	// Function to initialize the logger (opens the file)
	void LoggerInit(const char* filename);
	// Function to close the logger
	void LoggerShutdown();
	// Core logging function (don't call this directly, use macros)
	void LogOutput(LogLevel level, const char* file, int line, const char* func, const char* fmt, ...);

	#define LOG_T(...) LogOutput(logging::LOG_TRACE, __RELATIVE_FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
	#define LOG_D(...) LogOutput(logging::LOG_DEBUG, __RELATIVE_FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
	#define LOG_I(...) LogOutput(logging::LOG_INFO,  __RELATIVE_FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
	#define LOG_W(...) LogOutput(logging::LOG_WARN,  __RELATIVE_FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
	#define LOG_E(...) LogOutput(logging::LOG_ERROR, __RELATIVE_FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
}

