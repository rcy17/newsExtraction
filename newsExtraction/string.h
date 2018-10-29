#pragma once
#include <iostream>
#include <fstream>
#define ERROR_INDEX -1
class String;
String concat(const String & a, const String & b);

class String
{
private:
	char *str;
	int size;
	int capacity;
	const int defaultCapacity = 10;
public:
	// default constructor
	String();

	String(int capacity);

	// constructor via a char array
	String(const char * s);

	// copy constructor
	String(const String & s);

	// move constructor
	String(String && s);

	// simple destructor
	~String();

	int length() const;

	void enlarge();

	// assign a char array to string
	void assign(const char * s);

	bool operator ==(const String & s);

	bool operator !=(const String & s);

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
	String substring(int pos = 0, int len = 0);

	String operator + (const String & s);

	// append a char to tail
	void add(char c);

	// remove a char by rank
	bool remove(int rank);

	//swap two string's data ,mainly to simplify some assign operation
	void swap(String & s);

	// clear the string (lazy deletion)
	void clear();

	// match for the given range
	bool match(const String & s, int start = 0, int over = 0);

	int * buildNext(const String & s);

	// use kmp to get the position of a sub string, it will after the given position
	int indexOf(const String & s, int position = 0);

	// try to use its char array directly
	const char * getString();

	// now we can stream out a string easily
	friend std::ostream & operator << (std::ostream &, const String &);

	// also, we can stream in, but it maybe not so safe
	friend std::istream & operator >> (std::istream &, String &);

	// and file operation is also important
	friend std::ifstream & operator >> (std::ifstream &, String &);

	// however, overload ofstream << is not easy, and istream << is already enough
	// friend std::ofstream & operator << (std::ofstream &, const String &);

};

class String;



/****************** overload some operator for String ***********************/

// overload ostream << string, use inline to avoid mutiple definition
inline std::ostream & operator << (std::ostream & stream, const  String & s)
{
	s[s.size] = 0;
	int i;
	for (i = 0; i < s.size; i++)
		if (s[i] != '\n')
			break;
	stream << s.str + i;
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

// overload ifstream >> string to read data from file until EOF
inline std::ifstream & operator >> (std::ifstream & stream, String & s)
{
	char c;
	while (c = stream.peek())
	{
		if (c == EOF)
			break;
		// important, stream will ignore some char, which we need for html tag
		else if (c == ' ' || c == '\n')
			s.add(c);
		stream >> c;
		s.add(c);
	}
	return stream;
}


// concat two string to get a new string
inline String concat(const String & a, const String & b)
{
	String s(a);
	s.concat(b);
	// here move constructor will work
	return s;
}
