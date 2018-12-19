#pragma once
#pragma once
#include "string.h"

#define NO_DOCUMENT -1

class News
{
public:
	int index;
	String tittle;
	String source;
	String time;
	String content;
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

#define swap(p,q) {Document t = p->data;p->data=q->data;q->data=t;}

// document list class
class DocList
{
private:
	DocNode * head, *tail;
	int size;
public:
	// constructor, new a hear node
	DocList()
	{
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
		do
		{
			q = p->next;
			delete p;
			p = q;
		} while (p);
	}

	bool empty() const
	{
		return head == tail;
	}

	// get the first node in list
	DocNode * first() const
	{
		return head->next;
	}

	// update order in the list
	void update(DocNode * p)
	{
		while (p != tail)
		{
			if (p->data.times > p->next->data.times)
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
		DocNode * p = head->next;

		head->next = new DocNode(index, times);
		head->next->next = p;
		if (tail == head)
			tail = head->next;
		size++;
		update(head->next);
	}

	// search a file's information by index
	Document search(int index)
	{
		DocNode *p = head;
		p = head->next;
		while (p)
		{
			if (p->data.index == index)
				return p->data;
			p = p->next;
		}
		return Document();
	}

	// edit a file's tiems
	bool edit(int index, int times)
	{
		DocNode * p = head, *q;
		while (p->next)
		{
			if (p->next->data.index == index)
			{
				if (times >= p->data.times)
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
		return false;
	}

	// remove a file from the list
	bool remove(int index)
	{
		DocNode * p = head, *q;
		while (p->next)
		{
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
		return false;
	}
}
;

#undef swap