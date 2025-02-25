#include "extractor.h"
#include "list.h"
#include <ostream>
using std::cout;

#define STOP_WORD_FLAG -1
#define SCAN_ENGLISH_WORD 1

/************************* methods for Extractor class *********************/


Extractor::Extractor()
{
	inputPath.assign("./input/");
	outputPath.assign("./output/");
	cout << R"(loading "in.dic", please waiting......)";
	initDictionary();
	cout << "\nloading finished!\n";
	initialize();
}

Extractor::~Extractor()
{

}

bool Extractor::good()
{
	return inputGood;
}

void Extractor::initialize()
{
	inputGood = true;
	allData.clear();
	tittle.clear();
	content.clear();
	timeStamp.clear();
	source.clear();
	dict.clear();
}

void Extractor::setName(String & s)
{
	if (s.substring(s.length() - 5, 5) != String(".html"))
	{
		inputGood = false;
		return;
	}
	mainName = s.substring(0, s.length() - 5);
	initialize();
}

void Extractor::getData()
{
	if (!inputGood)
		return;
	html = fopen((inputPath + mainName + String(".html")).getString(), "rb");

	if (!html)
	{
		inputGood = false;
		return;
	}
	html >> allData;
	fclose(html);
}

void Extractor::checkTag(Stack<String> & s, Stack<String> & mis, String & tag)
{
	Stack<String> tem;
	bool match = false;
	// if s is empty, stop
	while (!s.empty())
	{
		// tyr to match
		if (s.top() == tag)
		{
			match = true;
			break;
		}
		// after mismatching, I try to consider it as nesting error
		tem.push(s.pop());
	}
	if (match)
	{
		// after matching, remember to pop the true tag
		// however, before I must deal with content tag
		if (s.length() <= contentTagPos + sourceTagPos)
		{
			// remember: s.length() > 0
			if (s.length() == contentTagPos && tag == "div")
			{
				// if this is the end of content
				contentTagPos = 0;
				state = INTAG;
			}
			else if (s.length() == sourceTagPos)
			{
				// it's the end of source
				sourceTagPos = 0;
				state = INTAG;
			}
			else
			{
				// or mismatch happened, deal with pos
				contentTagPos--;
			}

		}

		s.pop();
	}
	else
	{
		// else, remember this special tag
		mis.push(tag);
	}
	// now put all ignored tags in the stack again
	while (!tem.empty())
	{
		s.push(tem.pop());
	}
}

void Extractor::extractInfo()
{
	// use a stack to save tags as requested
	Stack<String> tagSave;
	// save mismatching tags
	Stack<String> misTag;
	// try to read a tag
	String tag;
	// save the position of a string's start
	int posSave;
	// the number of current pos
	int pos = 0;

	state = FREE;

	for (; pos < allData.length(); pos++)
	{
		switch (state)
		{
		case FREE:
			switch (allData[pos])
			{
			case '<':
				// now try to get the whole tag
				state = INCOMPLETE;
				posSave = pos + 1;
				break;
			default:
				break;
			}
			break;
		case INCOMPLETE:
			switch (allData[pos])
			{
			case '!':
				if (allData.substring(pos, 3) != "!--")
				{
					//ignore <!DOCTYPE HTML>
					state = UNIMPORTANT;
					break;
				}
				// in which case we meet comments, find its end
				posSave = allData.indexOf("-->", pos);
				if (posSave == ERROR_INDEX)
				{
					printf("error! <!-- without -->! ");
				}
				// ignore comments
				posSave += 3;
				pos = posSave - 1;
				state = FREE;
				break;
			case '\n':
				// \n can't be the right break char, but sometimes we meet it
			case ' ':
				// now we find the whole tag but didn't get its attributes
				tag = allData.substring(posSave, pos - posSave);
				if (tag == "meta")
				{
					state = UNIMPORTANT;
					break;
				}
				else if (tag == "img" || tag == "input")
				{
					// ignore img, input
					state = UNIMPORTANT;
					break;
				}
				else if (tag == "link")
				{
					// ignore link url
					state = UNIMPORTANT;
					break;
				}
				else if (tag == "script")
				{
					// try to find its end
					posSave = allData.indexOf("</script>", pos);
					if (posSave == ERROR_INDEX)
					{
						// when mismatch, it's really wired, I can just ignore it
						posSave = pos;
					}
					else
					{
						// after find the first </script>, ignore anything between them
						pos = (posSave += 8);
						state = FREE;
					}
				}
				else if (tag == "style")
				{
					// try to find its end
					posSave = allData.indexOf("</style>", pos);
					if (posSave == ERROR_INDEX)
					{
						// when mismatch, I can just ignore it
						posSave = pos;
					}
					else
					{
						// after find the first </style>, ignore anything between them
						pos = posSave + 7;
						if (contentTagPos)
							state = INCONTENT;
						else
							state = FREE;
					}
				}
				else if (tag == "h1")
				{
					// here deal with tittle in news
					state = INTITTLETAG;
					tagSave.push(tag);
				}
				else
				{
					tagSave.push(tag);
					state = INTAG;
				}
				posSave = pos + 1;
				break;
			case '>':
				// now we find the whole tag and finish it
				tag = allData.substring(posSave, pos - posSave);
				if (tag == "script")
				{
					// try to find its end
					posSave = allData.indexOf("</script>", pos);
					if (posSave == ERROR_INDEX)
					{
						// when mismatch, I can just ignore it
						posSave = pos;
					}
					else
					{
						// after find the first </script>, ignore anything between them
						pos = posSave + 8;
					}
				}
				else if (tag == "style")
				{
					// try to find its end
					posSave = allData.indexOf("</style>", pos);
					if (posSave == ERROR_INDEX)
					{
						// when mismatch, I can just ignore it
						posSave = pos;
					}
					else
					{
						// after find the first </style>, ignore anything between them
						pos = posSave + 7;
					}
				}
				else if (tag[0] == '/')
				{
					// when meet end tag, match it with stack's top
					tag = tag.substring(1);
					checkTag(tagSave, misTag, tag);
				}
				else if (tag[tag.length() - 1] == '/' || tag == "br")
				{
					// most time we meet <br/> and need this function to deal with
					// and deal with tag together
				}
				else if (tag == "h1")
				{
					tagSave.push(tag);
					state = INTITTLE;
					posSave = pos + 1;
					break;
				}
				else
				{
					tagSave.push(tag);

				}
				posSave = pos + 1;
				if (contentTagPos)
					state = INCONTENT;
				else if (sourceTagPos)
					state = INSOURCE;
				else
					state = FREE;
				break;
			default:
				break;
			}
			break;
		case INTAG:
			switch (allData[pos])
			{
			case '>':
				posSave = pos + 1;
				if (contentTagPos)
					state = INCONTENT;
				else if (sourceTagPos)
					state = INSOURCE;
				else
					state = FREE;
				break;
			case '/':
				if (allData[pos + 1] == '>')
				{
					state = FREE;
					tagSave.pop();
					pos++;
					posSave = pos + 1;
					if (contentTagPos)
						state = INCONTENT;
					else if (sourceTagPos)
						state = INSOURCE;
				}
				break;
			case '"':
				posSave = pos + 1;
				state = INSTRING;
				statePast = INTAG;
				break;
			default:
				break;
			}
			break;
		case UNIMPORTANT:
			// for those unimportant tag like img, link, input, just ignore
			if (allData[pos] == '>')
			{
				state = FREE;
				posSave = pos + 1;
				if (contentTagPos)
					state = INCONTENT;
				else if (sourceTagPos)
					state = INSOURCE;
			}
			break;
		case INSTRING:
			if (allData[pos] == '"')
			{
				if (statePast == INTAG)
				{
					String str = allData.substring(posSave, pos - posSave);
					if (str == "post_time_source" || str == "ptime" || str == "ep-time-soure cDGray" || str== "article-source")
					{
						// if it's time stamp, ready to get
						pos = allData.indexOf(">", pos);
						posSave = pos + 1;
						state = INTIME;
						contentTagPos = tagSave.length();
						break;

					}
					if (str == "ne_article_source" || str == "author" || str=="article-author")
					{
						// if it's source, ready to get
						pos = allData.indexOf(">", pos);
						posSave = pos + 1;
						state = INSOURCE;
						sourceTagPos = tagSave.length();
						break;
					}
					if (str == "post_text" || str == "endText" ||str.match("end-text"))
					{
						// if it's content text, ready to extracte
						pos = allData.indexOf(">", pos);
						posSave = pos + 1;
						state = INCONTENT;
						contentTagPos = tagSave.length();
						break;
					}
				}
				posSave = pos + 1;
				state = statePast;
			}
			break;
		case INTITTLETAG:
			if (allData[pos] == '"')
			{
				statePast = INTITTLETAG;
				state = INSTRING;
				posSave = pos + 1;
			}
			else if (allData[pos] == '>')
			{
				state = INTITTLE;
				posSave = pos + 1;
			}
			break;
		case INTITTLE:
			if (allData[pos] == '<' && allData[pos + 1] == '/')
			{
				tittle = allData.substring(posSave, pos - posSave);
				state = INCOMPLETE;
				posSave = pos + 1;
			}
			break;
		case INTIME:
			if (allData[pos] == '<')
			{
				if (pos != posSave)
					timeStamp.concat(allData.substring(posSave, pos - posSave));
				state = INCOMPLETE;
				posSave = pos + 1;
			}
			break;
		case INSOURCE:
			if (allData[pos] == '<')
			{
				if (pos != posSave)
					source.concat(allData.substring(posSave, pos - posSave));
				state = INCOMPLETE;
				posSave = pos + 1;
			}
			break;
		case INCONTENT:
			if (allData[pos] == '<')
			{
				if (pos != posSave)
					content.concat(allData.substring(posSave, pos - posSave));
				posSave = pos + 1;
				state = INCOMPLETE;
			}
			break;
		default:
			break;
		}
	}
	if (invalidTagReport)
	{
		// in this case, report all mismatch tags
		if (tagSave.length())
		{
			cout << "here are all the tags without end tags:";
			while (!tagSave.empty())
			{
				cout << tagSave.pop() << " ";
			}
			cout << "\n";
		}
		if (misTag.length())
		{
			cout << "here are all the tags without start tags:";
			while (!misTag.empty())
			{
				cout << misTag.pop() << " ";
			}
			cout << "\n";
		}
	}
	else
	{
		// else, just release these stacks
		while (!tagSave.empty())
			tagSave.pop();
		while (!misTag.empty())
			misTag.pop();
	}
}

// judge if a char is a number
inline bool isdigit(char c)
{
	return c >= '0'&& c <= '9';
}

// get standard format time from a string
inline String & getTime(String & s)
{
	String tem;
	if (s.length())
	{
		for (int i = 0; i < s.length() - 9; i++)
		{
			// get date as 2018-10-30
			if (isdigit(s[i]) && isdigit(s[i + 1]) && isdigit(s[i + 2]) && isdigit(s[i + 3])
				&& s[i + 4] == '-' && isdigit(s[i + 5]) && isdigit(s[i + 6]) && s[i + 7] == '-'
				&& isdigit(s[i + 8]) && isdigit(s[i + 9]))
			{
				tem.concat(s.substring(i, 10));
				tem.add(' ');
				break;
			}
		}
		for (int i = 0; i < s.length() - 7; i++)
		{
			// get time as 18:05
			if (isdigit(s[i]) && isdigit(s[i + 1]) && s[i + 2] == ':'
				&& isdigit(s[i + 3]) && isdigit(s[i + 4]) && s[i + 5] == ':'
				&& isdigit(s[i + 6]) && isdigit(s[i + 7]))
			{
				tem.concat(s.substring(i, 8));
				break;
			}
		}
	}
	return s = tem;
}

inline String & reorganiseSource(String & s)
{
	for (int i = 0; i < s.length() - 1; i++)
	{
		// use ' ' replace ' ' 
		if (s[i] == '\n')
			s[i] = ' ';
		if (s[i] == ' ')
		{
			// remove continuous ' '
			while (i + 1 < s.length() && (s[i + 1] == ' ' || s[i + 1] == '\n'))
				s.remove(i + 1);
			i++;
		}
	}
	return s;
}

inline String & reorganiseContent(String & s)
{
	// find source information at the end
	int tail = s.indexOf("本文来源：");
	for (int i = 0; i < s.length() - 1; i++)
		if (s[i] != ' '&&s[i] != '\n')
		{
			s = s.substring(i, tail > i ? tail - i : 0);
			break;
		}
	for (int i = 0; i < s.length() - 1; i++)
		if (s[i] == '\n')
			while (i + 1 < s.length() && (s[i + 1] == '\n'))
				s.remove(i + 1);
		else if(s[i]==' ')
			while (i + 5 < s.length() && (s[i + 5] == ' '))
				s.remove(i + 5);
		/*else if (s[i] == '\t')
			while (i + 1 < s.length() && (s[i + 1] == '\t'))
				s.remove(i + 1);*/
	return s;
}

void Extractor::outputInfo()
{
	// put out tittle, source, time and content
	info = fopen((outputPath + mainName + String(".info")).getString(), "w");

	assert(info);

	info << tittle << String("\n");
	info << reorganiseSource(source) << String("\n");
	info << getTime(timeStamp) << String("\n");
	info << reorganiseContent(content) << String("\n");

	fclose(info);
}

void Extractor::initDictionary()
{
	ifstream dictFile("./dic/out.dic");
	String s;
	char c = 0;
	do
	{
		c = dictFile.get();
		if (c == '\n' || c == '\r' || c == EOF)
		{
			dict.insert(s);
			s.clear();
		}
		else
			s.add(c);
	} while (c != EOF);
	// load stop word table
	dictFile.clear();
	dictFile.close();
	dictFile.open("./dic/stop.dic");
	c = 0;
	do
	{
		c = dictFile.get();
		if (c == '\n' || c == EOF)
		{
			// for stop-word, assign value -1
			if (s[0] != '\n')
				stopWord.add(s);
			s.clear();
		}
		else
			s.add(c);
	} while (c != EOF);
}

void Extractor::divideWords()
{
	int contentLength = content.length();
	String keyTry;
	// output segementation result
	txt = fopen((outputPath + mainName + String(".txt")).getString(),"w");

	txt << String("分词结果:") + String("\n");
	// as the shortesti word has 3 byte
	for (int pos = 0; pos < contentLength - 2; pos++)
	{
		// try to scan 8 character, but if not enough, try as long as possible
#if SCAN_ENGLISH_WORD
		for (int length = contentLength - pos > 15 ? 16 : (contentLength - pos); length >= 2; length--)
#else
		for (int length = contentLength - pos > 15 ? 16 : (contentLength - pos); length >= 4; length-=2)
#endif
		{
			keyTry = content.substring(pos, length);
			if (dict.inDict(keyTry))
			{
				txt << keyTry << String("\n");
				// if it's stop word, ignore it
				if (dict[keyTry] != STOP_WORD_FLAG)
					dict[keyTry]++;
				// with a successful try, skip these bytes
				pos += length - 1;
				break;
			}
#if SCAN_ENGLISH_WORD
			if (keyTry.isWord())
			{
				// if this is a English word, add it in dic
				txt << keyTry << String("\n");
				dict.insert(keyTry, 1);
				pos += length - 1;
				break;
			}
#endif
		}
	}
	fclose(txt);
}

void Extractor::outputTxt()
{
	if (applyStopWord)
		useStopWord();
	// get dict's segmentation result
	Data result = dict.getAll();
	int len = result.length();
	// sort according to times
	result.mergesort(0, len);
	txt = fopen((outputPath + mainName + String(".txt")).getString(), "a");
	txt << String("\n词频统计:") + String("\n");
	// output words
	for (int i = 0; i < len; i++)
	{
		/*if (result.getValue(i) < 2)
			break;*/
		txt << result[i] << String("\t\t") + String(result.getValue(i)) + String("\n");

	}
	fclose(txt);
}

void Extractor::output()
{
	outputInfo();
	divideWords();
	outputTxt();
}

void Extractor::useStopWord()
{
	auto node = stopWord.getNode(0);
	while (node)
	{
		dict[node->data] = STOP_WORD_FLAG;
		node = node->next;
	}
}

void Extractor::processOperation(String & fileName)
{
	setName(fileName);
	getData();
	extractInfo();
	output();
}

void Extractor::set(bool applyStopWordSet, bool invalidTagReportSet)
{
	applyStopWord = applyStopWordSet;
	invalidTagReport = invalidTagReportSet;
}