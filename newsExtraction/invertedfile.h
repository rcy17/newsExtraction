#pragma once
#include "documentlist.h"
#include "avltree.h"
#include "dictionary.h"

// use avl tree or use hash
#define USE_AVL_TREE 1

class InvertedFile
{
private:
#ifdef USE_AVL_TREE
	AVLTree dict;
#else

#endif
	// doclists for all words in dict(including avltree or hash)
	DocList docList[Dictionary::mod];
public:
	InvertedFile();

	~InvertedFile();

	// initialize the dict
	void loadDict();

};