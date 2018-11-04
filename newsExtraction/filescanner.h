#pragma once
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
#include "string.h"
#include "list.h"

class HTMLScanner
{
private:
	String filePath;
public:
	List<String> file_name;

	// initialize drectory
	HTMLScanner(const String & path = "./input/") :filePath(path)
	{
		getFile();
	}

	~HTMLScanner()
	{
		
	}

	// get all html file names in the given folder
	void getFile()
	{
		// use windows.h API to scan
		WIN32_FIND_DATA findFileData;
		// get first file
		HANDLE result = FindFirstFile((filePath + String("*.html")).getString(), &findFileData);
		// if there is no html file, return 
		if (result == INVALID_HANDLE_VALUE)
			return;
		do
		{
			// save this file name
			file_name.insert(file_name.length(), findFileData.cFileName);
			// if there is no next file, stop
		} while (FindNextFile(result, &findFileData));
	}

	int size() const
	{
		return file_name.length();
	}
};
