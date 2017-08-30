#pragma once

#include <tchar.h>
#include <stdarg.h>
#include <string>
#include "Singleton.h"

#define LOG_INFO(format, ...)  Logger::GetPtr()->LogInfo(format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)  Logger::GetPtr()->LogWarning(format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) Logger::GetPtr()->LogError(format, ##__VA_ARGS__)

class Logger : public Singleton<Logger>
{
public:

	virtual void LogInfo(const char *format, ...) = 0;
	virtual void LogWarning(const char *format, ...) = 0;
	virtual void LogError(const char *format, ...) = 0;
};

class StdLogger : public Logger
{
public:

	virtual void LogInfo(const char *format, ...) override;
	virtual void LogWarning(const char *format, ...) override;
	virtual void LogError(const char *format, ...) override;
};

class FileLogger : public Logger
{
private:

	std::string		mFileName;
	FILE			*mFile;

public:

	FileLogger(const std::string &fileName);
	~FileLogger();

	bool Open();
	void Close();

	virtual void LogInfo(const char *format, ...) override;
	virtual void LogWarning(const char *format, ...) override;
	virtual void LogError(const char *format, ...) override;
};