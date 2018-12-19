#pragma once
#include "string.h"
#include "invertedfile.h"

class Configuration
{
private:
	String configDate;
	bool autoScan, applyStopWord,
		invalidTagReport, segmentWord,
		countTime;

	int start;

	InvertedFile files;

	int recommended_max;
	
public:
	
	
	Configuration();

	// load config information
	void load();
	
	// segment word if needed
	void segment();

	// query as expected
	void query();

	//  process data according to configuration
	void process();
};