#pragma once
#include "documentlist.h"
#include "avltree.h"
#include "dictionary.h"

// use avl tree or use hash, 1->avl tree / 0->hash
#define TREE_VS_HASH 1

class InvertedFile
{
private:
	// amount of htmls
	static const int FILEAMOUNT = 781;

#if TREE_VS_HASH
	AVLTree dict;
#else
	Dictionary dict;
#endif
	// doclists for all words in dict(including avltree or hash)
	DocList docList[Dictionary::mod];
	News allNews[FILEAMOUNT];

	// save the index for occur times after sort
	int queryIndex[FILEAMOUNT];

	// count for occur times
	int occurTimes[FILEAMOUNT];

	// initialize the dict
	void loadDict();

	// initialize output file
	void initFile();

	// load segmentation result
	void loadSegmentation();

	// load news information
	void loadInfo(int index);

	// load words in the news
	void loadTxt(int index);

	// add word and times in doclist
	void addWord(String & word, String & number, int index);
	
	// sort for the occur times and return amount of nonzero number
	int sortRecorder();
public:
	InvertedFile();

	~InvertedFile();
	
	// initialize some data, mainly load files
	void initialize();


	// do all query operation
	void query();

	// finish news query by key word
	void queryNews(const String & sentence);

	// finish news recommend by tittle
	void recommend(const String & tittle);
};