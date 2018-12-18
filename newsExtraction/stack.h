#pragma once
#include <cassert>

template <typename T>

class Stack
{
	friend class Sort;
private:
	T * data;
	T * pTop;
	int size;
	int capacity;
	const int default_capacity = 30;
public:
	// constrcutor without anything
	Stack()
	{
		data = new T[capacity = default_capacity];
		pTop = data;
		size = 0;
	}

	// copy constructor
	Stack(const Stack & s)
	{
		size = s.size;
		capacity = s.capacity;
		data = new  T[capacity];
		for (int i = 0; i < s.size; i++)
		{
			data[i] = s.data[i];
		}
		pTop = data + size;
	}

	// move constructor
	Stack(Stack && s)
	{
		size = s.size;
		capacity = s.capacity;
		data = s.data;
		pTop = s.pTop;
		s.data = nullptr;
	}

	// destructor
	~Stack()
	{
		delete[]data;
	}

	// push a element to the top
	void push(const T & e)
	{
		*(pTop++) = e;
		if (++size == capacity)
		{
			enlarge();
		}
	}

	// enlarge the array twice the capacity
	void enlarge()
	{
		T * new_data = new T[capacity *= 2];
		for (int i = 0; i < size; i++)
		{
			new_data[i] = data[i];
		}
		delete[]data;
		data = new_data;
		pTop = data + size;
	}

	// pop the top element and return it
	T & pop()
	{
		assert(size);
		size--;
		return *(--pTop);
	}

	// just get the top element
	T & top()
	{
		return *(pTop - 1);
	}

	// judge if the stack is empty
	bool empty()
	{
		return !size;
	}

	// get the number of elements in this stack
	int length()
	{
		return size;
	}

	// swap two stacks' data
	void swap(Stack<T> s)
	{
		int temCapacity = capacity, temSize = size;
		T * temData = data, *pTop = pTop;
		capacity = s.capacity;
		size = s.size;
		data = s.data;
		tempTop = s.pTop;
		s.capacity = temCapacity;
		s.size = temSize;
		s.data = temData;
		s.pTop = tempTop;
	}

	// clear the stack (lazy deletion)
	void clear()
	{
		pTop = data;
		size = 0;
	}
};
