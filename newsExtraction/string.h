#pragma once
#include <iostream>
#include <cassert>

#define ERROR_INDEX -1

#pragma warning (disable:4996)
class String;
String concat(const String & a, const String & b);

class String
{
private:
	char *str;
	int size;
	int capacity;
	const int defaultCapacity = 10;

	// build next array for kmp
	int * buildNext(const String & s);

	// enlarge capacity if needed
	void enlarge();
public:
	// default constructor
	String();

	String(const char c);

	String(const int k);

	// constructor via a char array
	String(const char * s);

	// copy constructor
	String(const String & s);

	// move constructor
	String(String && s);

	// simple destructor
	~String();

	int length() const;

	// assign a char array to string
	void assign(const char * s);

	// change capacity of string, which will clear the string
	void resize(int size);

	bool operator ==(const String & s);

	bool operator !=(const String & s);

	bool operator < (const String & s);

	bool operator > (const String & s);

	// return true if String isn't empty
	operator bool() const;

	// overload = just as assign
	String & operator = (const char * s);

	// overload = for another string as deep copy
	String & operator = (const String & s);

	// overload [] so we can visit char easily
	char & operator[](int i) const;

	// concat a string and return itself
	String & concat(const String & s);

	// concat a char array and return itself
	String & concat(const char *s);

	// get a substring by given position and size
	String substring(int pos = 0, int len = 0) const;

	String operator + (const String & s);

	// append a char to tail
	void add(char c);

	// remove a char by rank
	bool remove(int rank);

	//swap two string's data ,mainly to simplify some assign operation
	void swap(String & s);

	// clear the string (lazy deletion)
	void clear();

	// return a string without space
	String removeSpace() const;

	// match for the given range
	bool match(const String & s, int start = 0, int over = 0);

	// judge if this is a English word or english word with number, but pure number will return false
	bool isWord()
	{
		// use flag to save if there is at least one English character
		bool flag = false;
		for (int i = 0; i < size; i++)
			if ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z'))
			{
				flag = true;
				continue;
			}
			else if ((str[i] >= '0' && str[i] <= '9') || str[i] == '\'')
				continue;
			else
				return false;
		return flag;
	}

	// match for news tittle, has fault-tolerance
	bool tittleMatch(const String & s) const;

	// use kmp to get the position of a sub string, it will after the given position
	int indexOf(const String & s, int position = 0);

	// try to use its char array directly
	const char * getString() const;

	// get data without any modification
	const char * getData() const;

	// now we can stream out a string easily
	friend std::ostream & operator << (std::ostream &, const String &);

	// also, we can stream in, but it maybe not so safe
	friend std::istream & operator >> (std::istream &, String &);

	//friend FILE * operator<<(FILE * fp, String & s);
	friend FILE * operator>>(FILE *fp, String & s);
};

class String;



/****************** overload some operator for String ***********************/

// overload ostream << string, use inline to avoid mutiple definition
inline std::ostream & operator << (std::ostream & stream, const  String & s)
{
	char c = s[s.size];
	s[s.size] = 0;

	int i;
	for (i = 0; i < s.size; i++)
		if (s[i] != '\n')
			break;
	stream << s.str + i;
	s[s.size] = c;
	return stream;
}

// overload istream >> string, use inline again 
inline std::istream & operator >> (std::istream & stream, String & s)
{
	s.clear();
	bool flag = false;
	char c, t;
	while (t = stream.peek())
	{
		if ((t == '\r' || t == ' ' || t == '\n' || t == EOF) && flag)
			break;
		flag = true;
		stream >> c;
		s.add(c);
	}
	return stream;
}
// given that stream input/output is too slow, i decide to use some basic api
inline FILE * operator<<(FILE * fp,const String & s)
{
	fwrite(s.getData(), 1, s.length(), fp);
	return fp;
}


// also, use some better method to read data
inline FILE * operator>>(FILE *fp, String & s)
{
	// first get size of file
	fseek(fp, 0, SEEK_END);
	int length = ftell(fp);
	// then read all data
	fseek(fp, 0, SEEK_SET);
	s.resize(length + 5);
	fread(s.str, 1, length, fp);
	s.size = length;
	return fp;
}


// concat two string to get a new string
inline String concat(const String & a, const String & b)
{
	String s(a);
	s.concat(b);
	// here move constructor will work
	return s;
}
