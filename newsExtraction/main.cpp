#include <iostream>
#include "string.h"
#include "extractor.h"
using namespace std;

#define AUTO_SCAN

#ifdef AUTO_SCAN
#include "filescanner.h"
#endif

int main()
{
	String fileName;
	Extractor extractor;
#ifdef AUTO_SCAN
	// in auto scanning, this program will scan all html files in ./input/
	HTMLScanner scanner;
	for (int i = 0; i < scanner.length(); i++)
	{
		cout << "now start to extract from " << scanner[i] << ", please wating..." << endl;
		extractor.processOperation(scanner[i]);
	}
	// now give some message before exiting
	cout << "all done!" << endl;
	for (int i = 0; i < 3; i++)
	{
		cout <<3 - i << " second later will it exit..." << endl;
		Sleep(1000);
	}
	cout << "exit!";
#else
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
		extractor.unwrap();
		extractor.output();

		cout << "input filename or input exit or ctrl + z to exit:";
	}
#endif
	return 0;
}