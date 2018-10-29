#pragma once
#include "string.h"
#include "stack.h"


#define DICTIONARY_WRONG_RETURN -1

class Data;

class Dictionary
{
private:
	String * key;
	int *value, capacity;
	// a 4k+3 prime as capacity
	const static int mod = 500111;

	// get a long long number by given String
	unsigned long long hash(const String &);

	// get a vacancy for new key
	int getVacancy(int);

	// get the rank of value for the given key
	int getRank(const String &);

	// save how many keys in this dictionary
	int size;
public:
	Dictionary();

	~Dictionary();

	// insert a string in dictionary, return -1 if dictionary is full
	bool insert(const String &, const int n = 0);

	// judge if a string has already been in dictionary
	bool inDict(const String &);

	// overload [] to get value directly
	int & operator[](const String &);

	// get value for the given key
	int getValue(const String &);

	// clear values, but keys are still here
	void clear();

	// get all nonzero value key and value
	Data getAll();
};



// use Data class to deal with segmentation result
class Data
{
private:
	String * allKey;
	int * allValue;
	int capacity;
	int size;
	const static int DEFAULT_CAPACITY = 100;
public:
	Data() :capacity(DEFAULT_CAPACITY), size(0)
	{
		allValue = new int[capacity];
		allKey = new String[capacity];
	}

	int length()
	{
		return size;
	}

	String operator[](int k)
	{
		return allKey[k];
	}

	int getValue(int k)
	{
		return allValue[k];
	}

	// move constructor to deal with return
	Data(Data && p)
	{
		allValue = p.allValue;
		allKey = p.allKey;
		size = p.size;
		capacity = p.capacity;
		p.allValue = nullptr;
		p.allKey = nullptr;
	}

	~Data()
	{
		delete[] allValue;
		delete[] allKey;
	}

	// if necessary, enlarge capacity twice
	void enlarge()
	{
		String * newKey = new String[capacity *= 2];
		int * newValue = new int[capacity];
		for (int i = 0; i < size; i++)
		{
			newKey[i] = allKey[i];
			newValue[i] = allValue[i];
		}
		delete[] allKey;
		delete[] allValue;
		allValue = newValue;
		allKey = newKey;
	}

	// push new key and value
	void push(const String & s, int t)
	{
		allKey[size] = s;
		allValue[size] = t;
		if (++size == capacity)
			enlarge();
	}

	// swap two key-value pairs for sort
	void swap(int rankA, int rankB)
	{
		String s = allKey[rankA];
		int i = allValue[rankB];

		allKey[rankA] = allKey[rankB];
		allValue[rankA] = allValue[rankB];

		allKey[rankB] = s;
		allValue[rankB] = i;
	}

	// use merge sort to sort it
	void mergesort(int lo, int hi)
	{
		if (lo + 2 == hi)
		{
			if (allValue[lo] < allValue[lo + 1])
				swap(lo, lo + 1);
			return;
		}
		if (lo + 1 == hi)
			return;
		int mid = (lo + hi) >> 1;
		mergesort(lo, mid);
		mergesort(mid, hi);
		merge(lo, mid, hi);
	}

	// the child function for merge sort to merge two branches
	void merge(int lo, int mid, int hi)
	{
		int *A = allValue + lo;
		String *As = allKey + lo;
		int lb = mid - lo;
		int *B = new int[lb];
		String *Bs = new String[lb];
		for (int i = 0; i < lb; i++)
		{
			B[i] = A[i];
			Bs[i] = As[i];
		}
		int lc = hi - mid;
		int *C = allValue + mid;
		String *Cs = allKey + mid;
		for (int i = 0, j = 0, k = 0; j < lb;)
		{
			if (k < lc&&C[k] > B[j])
			{
				A[i] = C[k];
				As[i++] = Cs[k++];
			}
			if (lc <= k || (B[j] >= C[k]))
			{
				As[i] = Bs[j];
				A[i++] = B[j++];
			}
		}
		delete[] B;
		delete[] Bs;
	}
};