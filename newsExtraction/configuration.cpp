#include <ctime>
#include "configuration.h"
#include "string.h"
#include "extractor.h"
#include "filescanner.h"
#include "invertedfile.h"
using namespace std;

/************************* methods for Configuration class ******************/

Configuration::Configuration() :start(clock()), autoScan(false), applyStopWord(false),
invalidTagReport(false), segmentWord(false), countTime(false), recommended_max(0)
{
	FILE * file(fopen("config.ini", "r"));
	file >> configDate;
}

// load config information
void Configuration::load()
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
		else if (configDate.substring(pos, 15) == "recommended_max")
		{
			pos += 15;
			while (++pos, configDate[pos] == ' ' || configDate[pos] == '\t');
			int t = 0;
			while (configDate[pos] >= '0' && configDate[pos] <= '9')
			{
				t *= 10;
				t += configDate[pos] - '0';
				pos++;
			}
			recommended_max = t;
		}
		else
			pos++;
}

// segment word if needed
void Configuration::segment()
{
	// if needed, segmentWord again
	String fileName;
	Extractor extractor;
	extractor.set(applyStopWord, invalidTagReport);
	if (autoScan)
	{
		// in auto scanning, this program will scan all html files in ./input/
		HTMLScanner scanner;
		while (!scanner.file_name.empty())
		{
			fileName = scanner.file_name.del(0);
			cout << "now start to extract from " << fileName << ", please wating..." << endl;
			extractor.processOperation(fileName);
		}
		// now give some message before exiting
		cout << "all done!" << endl;
	}
	else
	{
		// else, user inputs file names one by one
		cout << "input filename or input exit or ctrl + z to exit:";
		// get ctrl + z or loop continue
		while (cin >> fileName)
		{
			// after getting input "exit" loop will break
			if (fileName == "exit")
				break;
			extractor.setName(fileName);
			extractor.getData();
			if (!extractor.good())
			{
				cout << "can't open the file, please check and input again or input exit to exit:";
				continue;
			}
			extractor.extractInfo();
			extractor.output();

			cout << "input filename or input exit or ctrl + z to exit:";
		}
	}
}

// query as expected
void Configuration::query()
{
	if (recommended_max > 0)
		files.setUpper(recommended_max);
	files.initialize();
	files.query();
}

//  process data according to configuration
void Configuration::process()
{
	if (segmentWord)
		segment();

	query();

	if (countTime)
		printf("total used time: %d ms", clock() - start);
}