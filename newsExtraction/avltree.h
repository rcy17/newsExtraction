#pragma once
#include "string.h"

typedef int AVLData;

// subclass for tree
class AVLNode
{
public:
	String key;
	AVLData data;
	AVLNode *lchild, *rchild, *parent;
	int height;

	AVLNode();

	AVLNode(String key,const AVLData data);

	// copy constructor
	AVLNode(const AVLNode & node);

	~AVLNode();

	// clear all sub childs for this subtree
	void clear();

	// judge if this subtree is balenced in avl sense
	bool isBalenced();

	// update this node's height, assuming its two childs' height have updated
	void update();
};

class AVLTree
{
private:
	
	AVLNode * root;
	int size;

public:

	AVLTree();

	~AVLTree();

	// clear the whole tree
	void clearTree();

	// get the number of nodes in this tree
	int getSize() const;

	// insert a new node in the tree
	AVLNode * insert(const AVLNode & node);

	// insert a new node by key and data
	AVLNode * insert(const String & key, AVLData data);

	// find a node by data
	AVLNode * search(const String & key);

	// adjust the tree to balance
	AVLNode * adjust(AVLNode * pnode);

	// remove a node by data
	bool remove(const String & data);

	// edit a node's information by key, return true if node exits
	bool edit(const String & key,const AVLData & data);

	// here are some api to be compatible with hash
	bool inDict(const String & word);

	// return index of a word
	int getValue(const String & word);

};