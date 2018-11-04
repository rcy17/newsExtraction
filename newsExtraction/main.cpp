#include <iostream>
#include "string.h"
#include "extractor.h"
#include "configuration.h"
using namespace std;

#define AUTO_SCAN

#include "filescanner.h"

int main()
{
	String fileName;
	Extractor extractor;
	Configuration config;
	// first load all configuration and use them
	config.load();
	extractor.set(config.applyStopWord, config.invalidTagReport);
	if (config.autoScan)
	{
		// in auto scanning, this program will scan all html files in ./input/
		HTMLScanner scanner;
		while(fileName = scanner.file_name.del(0))
		{
			cout << "now start to extract from " << fileName << ", please wating..." << endl;
			extractor.processOperation(fileName);
		}
		// now give some message before exiting
		cout << "all done!" << endl;
		for (int i = 0; i < 3; i++)
		{
			cout << 3 - i << " second later will it exit..." << endl;
			Sleep(1000);
		}
		cout << "exit!";
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

	return 0;
}