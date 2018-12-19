#pragma once
#include <fstream>
#include "string.h"

class Configuration
{
private:
	String configDate;
public:
	bool autoScan, applyStopWord, 
		invalidTagReport, segmentWord,
		countTime;
	
	Configuration():autoScan(false),applyStopWord(false),
		invalidTagReport(false),segmentWord(false),
		countTime(false)
	{
		FILE * file(fopen("config.ini", "r"));
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
			else if (configDate.substring(pos, 13) == "segment_words")
			{
				pos += 13;
				while (++pos, configDate[pos] == ' ' || configDate[pos] == '\t');
				if (configDate[pos] == '1')
					segmentWord = true;
			}
			else if (configDate.substring(pos, 10) == "count_time")
			{
				pos += 10;
				while (++pos, configDate[pos] == ' ' || configDate[pos] == '\t');
				if (configDate[pos] == '1')
					countTime = true;
			}
			else
				pos++;
	}

};