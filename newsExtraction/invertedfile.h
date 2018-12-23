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
	static const int FILE_AMOUNT = 781;

	// aount of max recommend news
	int recommendMax;

	// max list length(technologically), with increase of it, slower but exacter
	const int listMax = 100;

#if TREE_VS_HASH
	AVLTree dict;
#else
	Dictionary dict;
#endif
	// save recommended files' index
	int *recommendIndex;

	// save recommended files' influence weight
	double *recommendWeight;

	// doclists for all words in dict(including avltree or hash)
	DocList docList[Dictionary::mod];

	// save all news information
	News allNews[FILE_AMOUNT];

	// save the index for occur times after sort
	int queryIndex[FILE_AMOUNT];

	// count for occur times
	int occurTimes[FILE_AMOUNT];

	// count how many diffent words occur
	int diffentWords[FILE_AMOUNT];

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
	void addWord(String & word, int number, int index);
	
	// sort for the occur times and return amount of nonzero number
	int sortRecorder();

	// try to insert a recommendation by index and weight
	void insertRecommand(int index, double weight);
public:
	InvertedFile();

	~InvertedFile();
	
	// initialize some data, mainly load files
	void initialize();

	// set recommended upper
	void setUpper(int k);

	// do all query operation
	void query();

	// finish news query by key word
	void queryNews(const String & sentence);

	// finish news recommend by tittle
	void recommend(const String & tittle);

	// recommend news from index
	void recommendNews(int index);
};