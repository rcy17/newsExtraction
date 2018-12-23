#include "avltree.h"
#include <cassert>


/************************* methods for AVLNode class ******************/

AVLNode::AVLNode() :lchild(nullptr), rchild(nullptr),
	parent(nullptr), height(0)
{

}

AVLNode::AVLNode(String key, const AVLData data):key(key),data(data),
	lchild(nullptr), rchild(nullptr), parent(nullptr), height(0)
{

}


// copy constructor
AVLNode::AVLNode(const AVLNode & node) : key(node.key), data(node.data),
height(0), lchild(nullptr), rchild(nullptr),parent(nullptr)
{

}

AVLNode::~AVLNode()
{

}

// clear all sub childs for this subtree
void AVLNode::clear()
{
	// use recursion to clear the tree
	if (lchild)
	{
		lchild->clear();
		delete lchild;
		lchild = nullptr;
	}
	if (rchild)
	{
		rchild->clear();
		delete rchild;
		rchild = nullptr;
	}
}


// judge if this subtree is balenced in avl sense
bool AVLNode::isBalenced()
{
	return abs((lchild ? lchild->height : -1) - (rchild ? rchild->height : -1)) < 2;
}

// update this node's height, assuming its two childs' height have updated
void AVLNode::update()
{
	if (lchild || rchild)
	{
		height = lchild ? lchild->height + 1 : 0;
		if (rchild && rchild->height >= height)
			height = rchild->height + 1;
	}
	else
		height = 0;
}

/************************* methods for AVLTree class ******************/

AVLTree::AVLTree() :root(nullptr),size(0)
{

}

AVLTree::~AVLTree()
{
	clearTree();
}

// clear the whole tree
void AVLTree::clearTree()
{
	if (root)
	{
		// root->clear will use recursion to clear the tree
		root->clear();
		delete root;
		root = nullptr;
	}
}

// get the number of nodes in this tree
int AVLTree::getSize() const
{
	return size;
}

// insert a new node in the tree
AVLNode * AVLTree::insert(const AVLNode & node)
{
	AVLNode * current = root;
	AVLNode * pnode, * save;
	// if root is nullptr, just insert the node here
	if (!root)
	{
		root = pnode = new AVLNode(node);
	}
	else
	{
		// now find the insert position by key
		while (1)
		{
			if (current->key < node.key)
			{
				if (current->rchild)
					current = current->rchild;
				else
				{
					// if a "smaller" node has no rchild, insert here
					pnode = new AVLNode(node);
					current->rchild = pnode;
					pnode->parent = current;
					break;
				}
			}
			else if (current->key > node.key)
			{
				if (current->lchild)
					current = current->lchild;
				else
				{
					// if a "bigger" node has no lchild, insert here
					pnode = new AVLNode(node);
					current->lchild = pnode;
					pnode->parent = current;
					break;
				}
			}
			else
				// if this node has been in the tree, insert failed
				return nullptr;
		}
	}
	// now make sure height is right and no unbalaence
	save = pnode;
	while (pnode)
	{
		// if it's unbalenced, adjust it and break the loop
		if (!pnode->isBalenced())
		{
			if (pnode == root)
			{
				// if get to root, we need a little special treatment for no parent
				root = adjust(pnode);
			}
			else
			{
				// else adjust the subtree and let its parent point to the new child
				AVLNode * parent = pnode->parent;
				// of course we need to know if pnode is its parent's lchild or rchild
				(parent->lchild == pnode ? parent->lchild : parent->rchild) = adjust(pnode);
			}
			// notice that after we solve unbalence, there is no need to back track
			break;
		}
		// otherwise just update height
		pnode->update();
		pnode = pnode->parent;
	}
	size++;
	return save;
}

// insert a new node by key and data
AVLNode * AVLTree::insert(const String & key, AVLData data)
{
	AVLNode node(key, data);
	return insert(node);
}

// find a node by key
AVLNode * AVLTree::search(const String & key)
{
	AVLNode * current = root;
	// search is simple, nothing special in contrast to BST
	while (current)
	{
		if (current->key < key)
			current = current->rchild;
		else if (current->key > key)
			current = current->lchild;
		else
			return current;
	}
	return nullptr;
}

// we need to know taller child to know unbalenced part's shape
inline AVLNode * tallerChild(AVLNode * pnode)
{
	// in this case pnode must have child, or it won't be adjusted node
	assert(pnode->lchild || pnode->rchild);
	if (!pnode->lchild)
		return pnode->rchild;
	else if (!pnode->rchild)
		return pnode->lchild;
	else
		return pnode->lchild->height < pnode->rchild->height ?
		pnode->rchild : pnode->lchild;
}

// here i use a simple shorthand to avoid some boring repeats
#define connect(p,t) if(t) t->parent = p;

// use 3 + 4 algorithm to rotate unbalanced part
inline void rotate(AVLNode * p1, AVLNode * p2, AVLNode * p3,
	AVLNode * t1, AVLNode * t2, AVLNode * t3, AVLNode * t4)
{
	// in this algorithm, we think the correct shape should be:
	//                 p2
	//                /  \
	//              p1    p3
	//             / \    / \
	//            t1 t2  t3  t3
	// what we should do is input right order in actual parameters
	p1->lchild = t1,p1->rchild = t2;
	p2->lchild = p1,p2->rchild = p3;
	p3->lchild = t3,p3->rchild = t4;
	p1->parent = p3->parent = p2;
	p1->update();
	p3->update();
	p2->update();
	connect(p1, t1);
	connect(p1, t2);
	connect(p3, t3);
	connect(p3, t4);
}

#undef connect

// adjust the tree to balance
AVLNode * AVLTree::adjust(AVLNode * pnode)
{
	// first find its child and grandchild
	assert(!(pnode->lchild && pnode->rchild) || pnode->lchild->height != pnode->rchild->height);
	AVLNode * child = tallerChild(pnode);
	assert(!(child->lchild && child->rchild) || child->lchild->height != child->rchild->height);
	AVLNode * grandchild = tallerChild(child);
	AVLNode * newParent = nullptr;
	// use 4 + 3 algrithm to rotate the tree
	// we should judge the kind of adjust and input right order to use 3 + 4 rotate
	if (pnode->lchild == child)
	{
		//zig
		if (child->lchild == grandchild)
		{
			// zig-zig
			child->parent = pnode->parent;
			rotate(grandchild, child, pnode, grandchild->lchild,
				grandchild->rchild, child->rchild, pnode->rchild);
			newParent = child;
		}
		else
		{
			// zig-zag
			grandchild->parent = pnode->parent;
			rotate(child, grandchild, pnode, child->lchild,
				grandchild->lchild, grandchild->rchild, pnode->rchild);
			newParent = grandchild;
		}
	}
	else
	{
		//zag
		if (child->lchild == grandchild)
		{
			// zag-zig
			grandchild->parent = pnode->parent;
			rotate(pnode, grandchild, child, pnode->lchild,
				grandchild->lchild, grandchild->rchild, child->rchild);
			newParent = grandchild;
		}
		else
		{
			// zag-zag
			child->parent = pnode->parent;
			rotate(pnode, child, grandchild, pnode->lchild,
				child->lchild, grandchild->lchild, grandchild->rchild);
			newParent = child;
		}
	}
	// return the new parent node in above three nodes to connect to main tree
	return newParent;
}

// remove a node by key
bool AVLTree::remove(const String & key)
{
	AVLNode * current = search(key);
	if (!current)
		// return false if this key isn't in the tree
		return false;
	AVLNode * & parentToChild = (current->parent->lchild == current ?
		current->parent->lchild : current->parent->rchild);
	// save the real removed node
	AVLNode * pnode = current;
	// save current's successor
	AVLNode * successor = nullptr;
	if (!current->lchild)
		// if current node doesn't have lchild, just use rchild to succeed
		successor = parentToChild = current->rchild;
	else if (!current->rchild)
		// if current node doesn't have rchild, do the same
		successor = parentToChild = current->lchild;
	else
	{
		// else find pnode's successor by logic
		pnode = pnode->rchild;
		// we find it along lchild
		while (pnode->lchild)
			pnode = pnode->lchild;
		// we exchange it with pnode, but just data ,to improve effcienct
		current->data = pnode->data;
		current->key = pnode->key;
		AVLNode * parent = pnode->parent;
		// we should judge if pnode's parent is current node
		(parent == current ? parent->rchild : parent->lchild) = successor = pnode->rchild;
	}
	// if deleted node really has rchild, adjust parent relationship
	if (successor)
		successor->parent = pnode->parent;
	delete pnode;

	pnode = successor;

	// adjust the whole tree
	while (pnode)
	{
		// if it's unbalenced, adjust it and break the loop
		if (!pnode->isBalenced())
		{

			if (pnode == root)
			{
				root = adjust(pnode);
			}
			else
			{
				AVLNode * parent = pnode->parent;
				(parent->lchild == pnode ? parent->lchild : parent->rchild) = adjust(pnode);
			}
			break;
		}
		// otherwise just update height
		pnode->update();
		pnode = pnode->parent;
	}
	size--;
	return true;
}

// edit a node's information
bool AVLTree::edit(const String & key, const AVLData & data)
{
	AVLNode * pnode = search(key);
	// if this key isn't in the tree, return false
	if (!pnode)
		return false;
	// else edit its data
	pnode->data = data;
	return true;
}

// here are some api to be compatible with hash
bool AVLTree::inDict(const String & word)
{
	return search(word);
}

// return index of a word
int AVLTree::getValue(const String & word)
{
	auto result = search(word);
	// return false if search failed
	if (!result)
		return -1;
	return result->data;
}
