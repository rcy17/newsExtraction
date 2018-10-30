#include "string.h"

/************************* methods for String class ******************/

String::String()
{
	str = new char[capacity = defaultCapacity];
	size = 0;
}

String::String(int capacity) :capacity(capacity)
{
	str = new char[capacity];
	size = 0;
}

// constructor via a char array
String::String(const char * s)
{
	str = new char[capacity = defaultCapacity];
	size = 0;
	assign(s);
}

// copy constructor
String::String(const String & s)
{
	str = new char[capacity = defaultCapacity];
	size = 0;
	concat(s);
}

// move constructor
String::String(String && s)
{
	capacity = s.capacity;
	size = s.size;
	// important: use this method to improve efficiency
	str = s.str;
	s.str = nullptr;
}

// simple destructor
String::~String()
{
	if (str)
		delete[]str;
}

int String::length() const
{
	return size;
}

void String::enlarge()
{
	char * s = new char[capacity *= 2 + 1];
	for (int i = 0; i < size; i++)
		s[i] = str[i];
	delete[]str;
	str = s;
}

// assign a char array to string
void String::assign(const char * s)
{
	for (size = 0; s[size];)
	{
		str[size++] = s[size];
		if (size == capacity)
			enlarge();
	}
}

bool String::operator ==(const String & s)
{
	if (size != s.size)
		return false;
	for (int i = 0; i < size; i++)
		if (str[i] != s[i])
			return false;
	return true;
}

bool String::operator !=(const String & s)
{
	return !(*this == s);
}

String::operator bool() const
{
	return size > 0;
}

// overload = just as assign
String & String::operator = (const char * s)
{
	assign(s);
	return *this;
}

// overload = for another string as deep copy
String & String::operator = (const String & s)
{
	size = 0;
	concat(s);
	return *this;
}

// overload [] so we can visit char easily
char & String::operator[](int i) const
{
	return str[i];
}

// concat a string and return itself
String & String::concat(const String & s)
{
	int len = s.size;
	int total = size + len;
	while (capacity <= total)
		enlarge();
	for (int i = size; i < total; i++)
		str[i] = s[i - size];
	size = total;
	return *this;
}


// concat a char array and return itself
String & String::concat(const char *s)
{
	int len = size;
	for (int i = 0; s[i]; i++)
	{
		str[i + len] = s[i];
		size++;
		if (size == capacity)
			enlarge();
	}
	return *this;
}

// get a substring by given position and size
String String::substring(int pos, int len)
{
	String s;
	if (!len || len > size - pos)
		len = size - pos;
	for (s.size = 0; s.size < len;)
	{
		s[s.size] = str[s.size + pos];
		s.size++;
		if (s.size == s.capacity)
			s.enlarge();
	}
	return s;
}

String String::operator + (const String & s)
{
	return ::concat(*this, s);
}

// append a char to tail
void String::add(char c)
{
	str[size++] = c;
	if (size == capacity)
		enlarge();
}

// remove a char by rank
bool String::remove(int rank)
{
	if (rank >= size)
		return false;
	for (int i = rank; i < size - 1; i++)
		str[i] = str[i + 1];
	size--;
	return true;
}

//swap two string's data ,mainly to simplify some assign operation
void String::swap(String & s)
{
	int temCapacity = capacity, temSize = size;
	char *temStr = str;
	capacity = s.capacity;
	size = s.size;
	str = s.str;
	s.capacity = temCapacity;
	s.size = temSize;
	s.str = temStr;
}

// clear the string (lazy deletion)
void String::clear()
{
	size = 0;
}

// match for the given range
bool String::match(const String & s, int start, int over)
{
	if (!over)
		if (s.size > size - start)
			return false;
		else
			over = s.size + start;
	for (int i = start; i < over; i++)
	{
		if (str[i] != s[i - start])
			return false;
	}
	return true;
}

int * String::buildNext(const String & s)
{
	int m = s.size, j = 0;
	int * next = new int[m];
	// use a sentinel -1 for next[0]
	int t = next[0] = -1;
	while (j < m - 1)
		if (t < 0 || s[j] == s[t])
			next[++j] = ++t;
		else
			t = next[t];
	return next;
}

// use kmp to get the position of a sub string, it will after the given position
int String::indexOf(const String & s, int position)
{
	// awating to finish
	int * next = buildNext(s);
	int n = size, i = position;
	int m = s.size, j = 0;
	while (j < m&&i < n)
		// for j<0, it's as an asterisk wildcard
		if (j < 0 || str[i] == s[j])
		{
			i++;
			j++;
		}
		else
			j = next[j];
	delete[]next;
	// if can't find, return ERROR_INDEX, else position
	return i - j == n ? ERROR_INDEX : i - j;
}

// try to use its char array directly
const char * String::getString()
{
	str[size] = '\0';
	return str;
}


