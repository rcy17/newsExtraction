#pragma once


template <typename T>
class Node
{
public:
	T		data;
	Node	*next;
};

template <typename T>
class List
{
private:
	int size;
	Node<T> * head, *tail;
public:
	// constructor, new a hear node
	List()
	{
		head = new Node<T>;
		tail = head;
		head->next = nullptr;
		size = 0;
	}

	// copy constructor, deep copy
	List(const List & l)
	{
		head = new Node<T>;
		Node<T> *s = l.head, *p = head, *q;
		while (s->next)
		{
			q = new Node<T>;
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
	List(List && l)
	{
		head = l.head;
		tail = l.tail;
		size = l.size;
		l.head = l.tail = nullptr;
	}

	// destructor, delete each node
	~List()
	{
		Node<T>*p = head, *q;
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

	int length() const
	{
		return size;
	}

	// get a node pointer by data
	Node<T> * getNode(int k)
	{
		Node<T> *p = head->next;
		while (k-- && p)
		{
			p = p->next;
		}
		return p;
	}

	// search a node's position by data
	int search(const T &e)
	{
		Node<T>*p = head;
		int i = 0;
		while (p && p->data != e)
		{
			p = p->next;
			i++;
		}
		if (!p)
			return -1;
		return i;
	}

	// add a node to the tail
	void add(const T &e)
	{
		Node<T> *s = new Node<T>;
		s->data = e;
		tail->next = s;
		tail = s;
		size++;
		return;
	}

	// insert a node to the given position
	void insert(int i, const T &e)
	{
		if (i == size)
		{
			add(e);
			return;
		}
		Node<T> *p = head, *s = new Node<T>;
		s->data = e;
		while (i--)
			p = p->next;
		s->next = p->next;
		p->next = s;
		size++;
	}

	// delete the node by the give position
	T del(int i)
	{
		if (i >= size)
			return 0;
		T e;
		Node<T> *p = head, *q;
		int j = 0;
		while (i--)
			p = p->next;
		q = p->next;
		e = q->data;
		p->next = q->next;
		// change tail node when trying to delete it
		if (q == tail)
			tail = p;
		delete q;
		q = nullptr;
		size--;
		return e;
	}

	// remove a node by given data
	bool remove(const T & e)
	{
		Node<T> *p = head, *q;
		while (q = (p = p->next)->next)
		{
			if (q->data == e)
			{
				// change tail node when trying to delete it
				if (q == tail)
					tail = p;
				p->next = q->next;
				delete q;
				size--;
				return true;
			}
		}
		return false;
	}

	// swap two lists' nodes
	void swap(List<T> l)
	{
		Node<T> *temHead = head, *temTail = tail;
		int temSize = size;
		head = l.head;
		tail = l.tail;
		size = l.size;
		l.head = temHead;
		l.tail = temTail;
		l.size = temSize;
	}

	// reserve the list by operate the head steadily
	void reserveList()
	{
		Node<T>*p, *q, *r;
		p = head->next;
		q = head->next->next;
		tail = p;
		while (q)
		{
			r = q->next;
			q->next = p;
			p = q;
			q = r;
		}
		tail->next = nullptr;
		head->next = p;
		return true;
	}

	// clear each node of this list(besides head node)
	void clear()
	{
		Node<T> *p = head, *q;
		while (p->next)
		{
			q = p->next->next;
			delete p->next;
			p->next = q;
		}
		tail = head;
		size = 0;
	}

};

