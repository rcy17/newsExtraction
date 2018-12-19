#include "invertedfile.h"
#include <memory>

/************************* methods for InvertedFile class *********************/

InvertedFile::InvertedFile()
{

}


InvertedFile::~InvertedFile()
{

}

// initialize some data, mainly load files
void InvertedFile::initialize()
{
	loadDict();
	loadSegmentation();
	initFile();
}

// load the dict
void InvertedFile::loadDict()
{
	FILE * fp(fopen("./dic/out.dic", "rb"));
	String s;
	fp >> s;
	int pos = 1, posSave = 0, length = s.length();
	int wordCount = 0;
	while (pos < length)
	{
		if (s[pos] == '\n')
		{
			dict.insert(AVLNode(s.substring(posSave, pos - posSave), wordCount++));
			posSave = pos + 1;
		}
		pos++;
	}
	if (posSave != pos)
		dict.insert(AVLNode(s.substring(posSave, pos - posSave), wordCount++));

}

// load segmentation result
void InvertedFile::loadSegmentation()
{
	for (int index = 0; index < FILEAMOUNT; index++)
	{
		allNews[index].index = index;
		loadInfo(index);
		loadTxt(index);
	}
}

// initialize output file
void InvertedFile::initFile()
{
	// here clear existing result files
	FILE *fp;
	fp = fopen("result1.txt", "w");
	fclose(fp);
	fp = fopen("result2.txt", "w");
	fclose(fp);
}


// load segmentation result
void InvertedFile::loadInfo(int index)
{
	FILE *fp(fopen((String("./output/") + String(index) + String(".info")).getString(), "rb"));
	if (!fp)
		return;
	String info;
	fp >> info;
	fclose(fp);

	int pos = 0, posSave = 0;
	while (info[++pos] != '\n');
	allNews[index].tittle = info.substring(posSave, pos - posSave);
	posSave = pos + 1;
	while (info[++pos] != '\n');
	allNews[index].source = info.substring(posSave, pos - posSave);
	posSave = pos + 1;
	while (info[++pos] != '\n');
	allNews[index].time = info.substring(posSave, pos - posSave);
	posSave = pos + 1;
	allNews[index].content = info.substring(posSave);
}

// load words in the news
void InvertedFile::loadTxt(int index)
{
	FILE *fp(fopen((String("./output/") + String(index) + String(".txt")).getString(), "rb"));
	if (!fp)
		return;
	String txt;
	fp >> txt;
	fclose(fp);

	int pos = txt.indexOf("词频统计:");
	int posSave = pos;
	while (txt[++pos] != '\n');
	posSave = pos + 1;

	// start to read words and times
	String word, number;
	for (pos++; pos < txt.length(); pos++)
	{
		switch (txt[pos])
		{
		case '\t':
			// get word
			word = txt.substring(posSave, pos - posSave);
			// skip next '\t'
			pos++;
			posSave = pos + 1;
			break;
		case '\n':
			// get number (in linux)
			number = txt.substring(posSave, pos - posSave);
			posSave = pos + 1;
			addWord(word, number, index);
			break;
		case '\r':
			// get number (in windows)
			number = txt.substring(posSave, pos - posSave);
			// skip '\n'
			pos++;
			posSave = pos + 1;
			addWord(word, number, index);
			break;
		default:
			break;
		}
	}
}

// add word and times in doclist
void InvertedFile::addWord(String & word, String & number, int index)
{
	int times = 0;
	for (int i = 0; i <number.length(); i++)
	{
		times *= 10;
		times += number[i] - '0';
	}
	auto result = dict.search(word);
	if (!result)
	{
		result = dict.insert(AVLNode(word, dict.getSize()));
		assert(result);
	}
	docList[result->data].add(index, times);
}

// do all query operation
void InvertedFile::query()
{
	FILE * fp(fopen("query1.txt", "rb"));
	String data;
	String sentence;
	fseek(fp, 0, SEEK_END);
	int length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	data.resize(length + 5);
	fp >> data;
	fclose(fp);
	memset(queryIndex, 0, sizeof(queryIndex));
	memset(occurTimes, 0, sizeof(occurTimes));
	int pos = 0, posSave = 0;

	while (pos < length)
	{
		while (data[++pos] != '\n');
		// segmentation and query
		queryNews(data.substring(posSave, pos - posSave));
		posSave = pos + 1;
		pos++;
	}
	if (pos != posSave)
	{
		// in case of lack of line feed in the end
		queryNews(data.substring(posSave, pos - posSave));
	}


	fp = fopen("query2.txt", "rb");
	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	data.resize(length + 5);
	fp >> data;
	fclose(fp);

	pos = posSave = 0;
	while (pos < length)
	{
		while (data[++pos] != '\n');
		// recommend
		recommend(data.substring(posSave, pos - posSave));
		posSave = pos + 1;
		pos++;
	}
	if (pos != posSave)
	{
		// in case of lack of line feed in the end
		recommend(data.substring(posSave, pos - posSave));
	}
}

// swap two index-value pairs for sort
void swap(int * allValue, int * allIndex, int rankA, int rankB)
{
	int s = allIndex[rankA];
	int i = allValue[rankB];

	allIndex[rankA] = allIndex[rankB];
	allValue[rankA] = allValue[rankB];

	allIndex[rankB] = s;
	allValue[rankB] = i;
}

// the child function for merge sort to merge two branches
void merge(int * allValue, int * allIndex, int lo, int mid, int hi)
{
	int *A = allValue + lo;
	int *As = allIndex + lo;
	int lb = mid - lo;
	int *B = new int[lb];
	int *Bs = new int[lb];
	for (int i = 0; i < lb; i++)
	{
		B[i] = A[i];
		Bs[i] = As[i];
	}
	int lc = hi - mid;
	int *C = allValue + mid;
	int *Cs = allIndex + mid;
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

// use merge sort to sort it
void mergesort(int * allValue, int * allIndex, int lo, int hi)
{
	if (hi - lo == 2)
	{
		if (allValue[lo] < allValue[lo + 1])
			swap(allValue, allIndex, lo, lo + 1);
		return;
	}
	if (hi - lo < 2)
		return;
	int mid = (lo + hi) >> 1;
	mergesort(allValue, allIndex, lo, mid);
	mergesort(allValue, allIndex, mid, hi);
	merge(allValue, allIndex, lo, mid, hi);
}



// sort for the occur times
int InvertedFile::sortRecorder()
{
	for (int i = 0; i < FILEAMOUNT; i++)
		queryIndex[i] = i;

	mergesort(occurTimes, queryIndex, 0, FILEAMOUNT);
	int cnt = -1;
	while (occurTimes[++cnt]);

	return cnt;
}

// finish news query by key word
void InvertedFile::queryNews(const String & sentence)
{
	// first segmentation
	int sentenceLength = sentence.length();
	String keyTry;
	int index;
	FILE *fp(fopen("result1.txt", "ab"));

	// as the shortesti word has 4 byte
	for (int pos = 0; pos < sentenceLength - 4; pos++)
	{
		// try to scan 8 character, but if not enough, try as long as possible
		for (int length = sentenceLength - pos > 15 ? 16 : (sentenceLength - pos); length >= 2; length--)
		{
			keyTry = sentence.substring(pos, length);
			if ((index = dict.getValue(keyTry)) != DICTIONARY_WRONG_RETURN)
			{
				//debug
				fp << keyTry + String("\n");
				auto p = docList[index].first();
				while (p)
				{
					// count occur times
					occurTimes[p->data.index] += p->data.times;
					p = p->next;
				}
				// with a successful try, skip these bytes
				pos += length - 1;
				break;
			}
		}
	}

	int records = sortRecorder();
	if (records)
		for (int i = 0; i < records; i++)
		{
			if (i)
				fp << String(' ');
			fp << String('(') + String(queryIndex[i]) + String(',') +
				String(occurTimes[i]) + String(')');
		}
	else
		fp << String("数据库中未找到合适新闻，请重新输入或去掉不必要的空格");
	fp << String('\n');
	fclose(fp);
	memset(queryIndex, 0, sizeof(queryIndex));
	memset(occurTimes, 0, sizeof(occurTimes));
}


// finish news recommend by tittle
void InvertedFile::recommend(const String & tittle)
{
	if (tittle.length() < 2)
		return;
	FILE *fp(fopen("result2.txt", "ab"));
	bool founded = false;
	for (auto & news : allNews)
	{
		if (tittle.tittleMatch(news.tittle))
		{
			//debug
			fp << String("查找成功！编号") + String(news.index) + String("\n");
			founded = true;
			break;
		}
	}
	if (!founded)
		fp << String("该新闻不在数据库中，无法推荐\n");
	fclose(fp);

}

