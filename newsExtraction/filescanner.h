#pragma once
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
#include "string.h"

class HTMLScanner
{
private:
	int size;
	String * fileName;
	int capacity;
	const static int DEFAULT_CAPACITY = 30;
	String filePath;

	// enlarge capacity by twice
	void enlarge()
	{
		String * new_string = new String[capacity *= 2];
		for (int i = 0; i < size; i++)
			new_string[i] = fileName[i];
		delete[] fileName;
		fileName = new_string;
	}
public:
	// initialize drectory
	HTMLScanner(const String & path = "./input/", int capacity = DEFAULT_CAPACITY) :filePath(path), capacity(capacity)
	{
		fileName = new String[capacity];
		size = 0;
		getFile();
	}

	~HTMLScanner()
	{
		delete[] fileName;
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
			fileName[size++] = findFileData.cFileName;
			// if there is no next file, stop
		} while (FindNextFile(result, &findFileData));
	}

	int length()
	{
		return size;
	}

	String & operator[](int k) const
	{
		return fileName[k];
	}
};
