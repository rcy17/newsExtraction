#pragma once
#include "string.h"
#include "stack.h"
#include "list.h"
#include "dictionary.h"

#include <fstream>
using std::ofstream;
using std::ifstream;

// an important class to manage xml data
class Extractor
{
	
private:
	enum SearchState
	{
		// normal state
		FREE,
		// met a < but haven't get a full tag
		INCOMPLETE,
		// in a tag (wait for >0
		INTAG,
		// in ""
		INSTRING,
		// in an unimportant tag
		UNIMPORTANT,
		// in news' tittle
		INTITTLE,
		// in news' content
		INCONTENT,
		// in <h1>
		INTITTLETAG,
		// in time stamp
		INTIME,
		// in source
		INSOURCE,

	};
	ifstream html;
	ofstream info,txt;
	// for file name
	String inputPath,outputPath,mainName,allData;
	// for news
	String content, tittle, source, timeStamp;
	// for news content extractor
	int contentTagPos,sourceTagPos;
	// for news extractor to save state
	SearchState state, statePast;

	// save all stop word, maybe won't use in future
	List<String> stopWord;

	// judge if user input right file name
	bool inputGood;

	// intialize some variables and open the file
	void initialize();

	// check if tags are match
	void checkTag(Stack<String> & s, Stack<String> & mis, String & tag);

	// use a dictionary to divide words
	Dictionary dict;

	// output segmentation's result
	void outputTxt();

	// output supposed information
	void outputInfo();

	// use stop word by setting -1 flag
	void useStopWord();
public:
	Extractor();

	~Extractor();

	// get input status
	bool good();


	// set file name
	void setName(String & s);

	// open the file and get data from html
	void getData();
	
	// unwrap the html data
	void extractInfo();

	// output all thing needed
	void output();

	// do the things this assignment need doing
	void processOperation(String &);

	// load dictionary for segmentation
	void initDictionary();

	// segmentation for words in dictionary to content
	void divideWords();
};