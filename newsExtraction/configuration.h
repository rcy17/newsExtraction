#pragma once
#include <fstream>
#include "string.h"
using std::ifstream;

class Configuration
{
private:
	String configDate;
public:
	Configuration():autoScan(false),applyStopWord(false),invalidTagReport(false)
	{
		ifstream file("config.ini");
		file >> configDate;
	}

	// load config information
	void load()
	{
		int pos = 0;
		int len = configDate.length();
		while (pos < len)
			if (configDate[pos] == '#')
			{
				// when meeting '#', skip this line
				while (configDate[++pos] != '\n');
			}
			else if (configDate.substring(pos, 9) == "auto_scan")
			{
				pos += 9;
				while (++pos, configDate[pos] == ' ' || configDate[pos] == '\t');
				if (configDate[pos] == '1')
					autoScan = true;
			}
			else if (configDate.substring(pos, 15) == "apply_stop_word")
			{
				pos += 15;
				while (++pos, configDate[pos] == ' ' || configDate[pos] == '\t');
				if (configDate[pos] == '1')
					applyStopWord = true;
			}
			else if (configDate.substring(pos, 18) == "invalid_tag_report")
			{
				pos += 18;
				while (++pos, configDate[pos] == ' ' || configDate[pos] == '\t');
				if (configDate[pos] == '1')
					invalidTagReport = true;
			}
			else
				pos++;
	}

	bool autoScan, applyStopWord, invalidTagReport;
};