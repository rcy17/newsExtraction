#include <iostream>
#include "string.h"
#include "extractor.h"
#include "configuration.h"
#include "filescanner.h"
#include "invertedfile.h"
using namespace std;


Configuration config;
void segmentWord();
InvertedFile files;

#include <ctime>

int main()
{
	auto start = clock();

	// first load all configuration and use them
	config.load();
	if (config.segmentWord)
		segmentWord();

	files.initialize();
	files.query();


	auto finish = clock();
	if (config.countTime)
		printf("time used in main function: %d ms", finish - start);
	return 0;
}

void segmentWord()
{
	// if needed, segmentWord again
	String fileName;
	Extractor extractor;
	extractor.set(config.applyStopWord, config.invalidTagReport);
	if (config.autoScan)
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