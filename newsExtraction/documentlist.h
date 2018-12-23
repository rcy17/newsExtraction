#pragma once
#pragma once
#include "string.h"

#define NO_DOCUMENT -1

class News
{
public:
	// index for html
	int index;
	
	String tittle;
	String source;
	String time;
	String content;

	// news total words' square
	int words;
	// square root of words
	double sqrt_words;
};

class Document
{
public:
	int index;
	int times;
	Document(int index = NO_DOCUMENT, int times = 0) :
		index(index), times(times)
	{

	}
};

class DocNode
{
public:
	Document data;
	DocNode * next;
	DocNode() :next(nullptr)
	{

	}
	DocNode(int index, int times) :
		data(index, times), next(nullptr)
	{

	}
};
// exchange two node by exchanging their data
#define swap(p,q) {Document t = p->data;p->data=q->data;q->data=t;}

// document list class, save document in order of occurring times
class DocList
{
private:
	DocNode * head, *tail;
	int size;
public:
	// constructor, new a hear node
	DocList()
	{
		// use head sentinel
		head = new DocNode;
		tail = head;
		head->next = nullptr;
		size = 0;
	}

	// copy constructor, deep copy
	DocList(const DocList & l)
	{
		head = new DocNode;
		DocNode *s = l.head, *p = head, *q;
		while (s->next)
		{
			q = new DocNode;
			q->data = s->data;
			p->next = q;
			p = q;
			s = s->next;
		}
		tail = p;
		p->next = nullptr;
		size = l.size;
	}

	// move constructor, shallow copy
	DocList(DocList && l)
	{
		head = l.head;
		tail = l.tail;
		size = l.size;
		l.head = l.tail = nullptr;
	}

	// destructor, delete each node
	~DocList()
	{
		DocNode * p = head, *q;
		while(p)
		{
			q = p->next;
			delete p;
			p = q;
		}
	}

	// judge if this list has been empty
	bool empty() const
	{
		return head == tail;
	}

	// get the first node in list
	DocNode * first() const
	{
		return head->next;
	}

	// get the last node in list
	DocNode * last() const
	{
		if (tail == head)
			return nullptr;
		return tail;
	}

	// update order in the list
	void update(DocNode * p)
	{
		// if get to tail, stop
		while (p != tail)
		{
			// sort file nodes by occur times
			if (p->data.times < p->next->data.times)
			{
				swap(p, p->next);
				p = p->next;
			}
			else
				break;
		}
	}

	// add a new file(index) in list
	void add(int index, int times)
	{
		// save first node
		DocNode * p = head->next;
		// put new node as first node, then adjust
		head->next = new DocNode(index, times);
		head->next->next = p;
		// in case add a node to empty list
		if (tail == head)
			tail = head->next;
		size++;
		// update the order
		update(head->next);
	}

	// search a file's information by index
	DocNode * search(int index)
	{
		// for list, we have no method but traversal
		DocNode *p = head;
		p = head->next;
		while (p)
		{
			if (p->data.index == index)
				return p;
			p = p->next;
		}
		return nullptr;
	}

	// edit a file's times by index
	bool edit(int index, int times)
	{
		DocNode * p = head, *q;
		while (p->next)
		{
			// first we find the node
			if (p->next->data.index == index)
			{
				if (times <= p->data.times)
				{
					// if the order before is right, just update
					p->next->data.times = times;
					update(p->next);
				}
				else
				{
					// else remove the node and add again
					q = p->next->next;
					delete p->next;
					p->next = q;
					add(index, times);
				}

				return true;
			}
			p = p->next;
		}
		// if this node isn't in list, return false
		return false;
	}

	// edit a file's times by point to node, use it after search
	bool edit(DocNode * pnode, int times)
	{
		if (!pnode)
			return false;
		pnode->data = times;
		return true;
	}

	// remove a file from the list
	bool remove(int index)
	{
		DocNode * p = head, *q;
		while (p->next)
		{
			// find the file and delete related data
			if (p->next->data.index == index)
			{
				q = p->next->next;
				delete p->next;
				p->next = q;
				size--;
				return true;
			}
			p = p->next;
		}
		// if havn't find the file, return false
		return false;
	}
}
;

#undef swap