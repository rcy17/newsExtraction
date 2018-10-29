#include <iostream>
#include "string.h"
#include "extractor.h"
using namespace std;

int main()
{
	String fileName;
	Extractor extractor;
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
	return 0;
}