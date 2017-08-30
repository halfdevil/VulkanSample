#include "Logger.h"
#include <stdio.h>
#include <stdarg.h>

void StdLogger::LogInfo(const char *format, ...)
{
	va_list argList;	
	va_start(argList, format);

	printf_s("[INFO]: ");	
	vprintf_s(format, argList);
	printf_s("\r\n");

	va_end(argList);
}

void StdLogger::LogWarning(const char *format, ...)
{
	va_list argList;
	va_start(argList, format);

	printf_s("[WARN]: ");
	vprintf_s(format, argList);
	printf_s("\r\n");

	va_end(argList);
}

void StdLogger::LogError(const char *format, ...)
{
	va_list argList;
	va_start(argList, format);

	printf_s("[ERROR]: ");
	vprintf_s(format, argList);
	printf_s("\r\n");

	va_end(argList);
}

FileLogger::FileLogger(const std::string &fileName)
	: mFileName(fileName), mFile(nullptr)
{	
}

FileLogger::~FileLogger()
{
	Close();
}

bool FileLogger::Open()
{
	if (mFile)
		Close();

	errno_t err = fopen_s(&mFile, mFileName.c_str(), "w+");
	if (err != 0 || mFile == nullptr)
	{
		_ftprintf_s(stderr, _T("Unable to open log: %d"), err);
		return false;
	}

	return true;
}

void FileLogger::Close()
{
	if (mFile)	
		fclose(mFile);
	
	mFile = nullptr;
}

void FileLogger::LogInfo(const char *format, ...)
{
	va_list argList;
	va_start(argList, format);

	fprintf_s(mFile, "[INFO]: ");
	vfprintf_s(mFile, format, argList);
	fprintf_s(mFile, "\r\n");

	va_end(argList);
	fflush(mFile);
}

void FileLogger::LogWarning(const char *format, ...)
{
	va_list argList;
	va_start(argList, format);

	fprintf_s(mFile, "[WARN]: ");
	vfprintf_s(mFile, format, argList);
	fprintf_s(mFile, "\r\n");

	va_end(argList);

	fflush(mFile);
}

void FileLogger::LogError(const char *format, ...)
{
	va_list argList;
	va_start(argList, format);

	fprintf_s(mFile, "[ERROR]: ");
	vfprintf_s(mFile, format, argList);
	fprintf_s(mFile, "\r\n");

	va_end(argList);
	fflush(mFile);
}