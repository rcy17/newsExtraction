#include "invertedfile.h"

/************************* methods for InvertedFile class *********************/

InvertedFile::InvertedFile()
{

}


InvertedFile::~InvertedFile()
{

}

// initialize the dict
void InvertedFile::loadDict()
{
	FILE * fp(fopen("./dic/out.dic", "rb"));
	String s;
	fp >> s;
	int pos = 1, posSave = 0,length = s.length();
	int wordCount = 0;
	while (pos < length)
	{
		if (s[pos] == '\n')
		{
			dict.insert(AVLNode(s.substring(posSave, pos - posSave),wordCount++));
			posSave = pos + 1;
		}
		pos++;
	}
	if(posSave!=pos)
		dict.insert(AVLNode(s.substring(posSave, pos - posSave), wordCount++));

}