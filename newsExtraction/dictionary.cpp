#include "dictionary.h"

/************************* methods for Dictionary class *********************/

Dictionary::Dictionary() :capacity(mod), size(0)
{
	value = new int[capacity + 5];
	key = new String[capacity + 5];
}

Dictionary::~Dictionary()
{
	delete[] key;
	delete[] value;
}


bool Dictionary::insert(const String & s, const int n)
{
	if (!s.length())
		// return false if input wrong string
		return false;
	int k = getVacancy(hash(s) % mod);
	if (k == DICTIONARY_WRONG_RETURN)
		// return false if it's already full
		return false;
	// otherwise insert it
	key[k] = s;
	value[k] = n;
	size++;
	return true;
}

bool Dictionary::setValue(const String & s, const int n)
{
	int rank = getRank(s);
	if (rank == DICTIONARY_WRONG_RETURN)
		return false;
	value[rank] = n;
	return true;
}

bool Dictionary::inDict(const String &s)
{
	return getRank(s) != DICTIONARY_WRONG_RETURN;
}

int & Dictionary::operator[](const String & s)
{
	int rank = getRank(s);
	if (rank == DICTIONARY_WRONG_RETURN)
		// if use [] with a key not existing, it's embarassing
		return value[capacity];
	else
		return value[rank];
}

unsigned long long Dictionary::hash(const String & s)
{
	// use a ull to save number
	unsigned long long number = 0;

	// divide s into 4-byte parts
	int divideLength = s.length() / 4 + bool(s.length() % 4);
	unsigned int sum = 0;
	int mul = 1;

	// for all parts except last one, sum up
	for (int i = 0; i < divideLength - 1; i++)
	{
		mul = 1;
		sum = 0;
		// convert a 4-byte part into 32-bit number
		for (int j = 0; j < 4; j++)
		{
			sum += unsigned char(s[i * 4 + j]) * mul;
			mul *= 256;
		}
		number += sum;
	}
	mul = 1;
	sum = 0;
	// for the last one, sometimes must deal exceptionally
	for (int i = divideLength * 4 - 4; i < s.length(); i++)
	{

		sum += unsigned char(s[i]) * mul;
		mul *= 256;
	}
	number += sum;

	// return the total number
	return number;
}

// get a vacancy for new key
int Dictionary::getVacancy(int k)
{
	if (!key[k])
		return k;
	int n = 0;
	int rank;
	// use square-double-trial method
	for (int i = 1; i < (capacity + 1) / 2; i++)
	{
		n = i * i;
		rank = k + n;
		if (rank >= capacity)
			rank -= capacity;
		if (!key[rank])
			return rank;
		rank = (k - n);
		if (rank < 0)
			rank += capacity;
		if (!key[rank])
			return rank;
	}
	// after trial, return DICTIONARY_WRONG_RETURN means dictionary is full
	return DICTIONARY_WRONG_RETURN;
}

// get the rank of value for the given key
int Dictionary::getRank(const String & s)
{
	if (!s.length())
		// specially, blank input string is forbidden
		return DICTIONARY_WRONG_RETURN;
	int k = hash(s) % mod;
	// if trial gets a vacancy, then this key not in key grid
	if (!key[k])
		return DICTIONARY_WRONG_RETURN;
	// if this key is just as the trial, success
	if (key[k] == s)
		return k;
	int n = 0;
	int rank;
	// use square-double-trial method to repeat above operation
	for (unsigned long long i = 1; i < (capacity + 1) / 2; i++)
	{
		n = i * i % capacity;
		rank = k + n;
		if (rank >= capacity)
			rank -= capacity;
		if (!key[rank])
			return DICTIONARY_WRONG_RETURN;
		if (key[rank] == s)
			return rank;
		rank = (k - n);
		if (rank < 0)
			rank += capacity;
		if (!key[rank])
			return DICTIONARY_WRONG_RETURN;
		if (key[rank] == s)
			return rank;
	}
	// in this case, the dictionary is full and can't find needed key
	return DICTIONARY_WRONG_RETURN;
}

// get value for the given key
int Dictionary::getValue(const String & s)
{
	int rank = getRank(s);
	if (rank == DICTIONARY_WRONG_RETURN)
		return DICTIONARY_WRONG_RETURN;
	else
		return value[rank];
}

void Dictionary::clear()
{
	for (int i = 0; i < capacity; i++)
	{
		value[i] = 0;
	}
}

Data Dictionary::getAll()
{
	Data tem;
	// scan all things in hash table
	for (int i = 0; i < capacity; i++)
		// if this key appear, value would be positive
		// but for stop words, ignore them
		if (value[i] > 0)
			// push key and value to return
			tem.push(key[i], value[i]);
	return tem;
}