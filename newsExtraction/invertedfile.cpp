#include "invertedfile.h"
#include <memory>
#include <cmath>

#define RECOMMEND_LOWEST_WEIGHT (1e-3)

/************************* methods for InvertedFile class *********************/

// use some macros to simplify transformation from AVL tree to hash
#if TREE_VS_HASH
#define DICT_PAIR(a,b) AVLNode(a,b)
#define RESULT_ERROR(result) (!result)
#define RESULT_INDEX(result) result->data
#else
#define DICT_PAIR(a,b) a , b
#define RESULT_ERROR(result) (result == DICTIONARY_WRONG_RETURN)
#define RESULT_INDEX(result) result
#endif

InvertedFile::InvertedFile() :recommendMax(5),
recommendIndex(nullptr), recommendWeight(nullptr)
{

}

InvertedFile::~InvertedFile()
{
	if (recommendIndex)
		delete recommendIndex;
	if (recommendWeight)
		delete recommendWeight;
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

			dict.insert(DICT_PAIR(s.substring(posSave, pos - posSave), wordCount++));
			posSave = pos + 1;
		}
		pos++;
	}
	if (posSave != pos)
		dict.insert(DICT_PAIR(s.substring(posSave, pos - posSave), wordCount++));

}

// load segmentation result
void InvertedFile::loadSegmentation()
{
	for (int index = 0; index < FILE_AMOUNT; index++)
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

inline void tailCheck(String & s)
{
	if (s[s.length() - 1] == '\r')
		s.remove(s.length() - 1);
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
	tailCheck(allNews[index].tittle = info.substring(posSave, pos - posSave));
	posSave = pos + 1;

	while (info[++pos] != '\n');
	tailCheck(allNews[index].source = info.substring(posSave, pos - posSave));
	posSave = pos + 1;

	while (info[++pos] != '\n');
	tailCheck(allNews[index].time = info.substring(posSave, pos - posSave));
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
	allNews[index].words = 0;

	// start to read words and times
	String word;
	int number;
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
			number = txt.substring(posSave, pos - posSave).toInt();
			posSave = pos + 1;
			addWord(word, number, index);
			allNews[index].words += number * number;
			break;
		case '\r':
			// get number (in windows)
			number = txt.substring(posSave, pos - posSave).toInt();
			// skip '\n'
			pos++;
			posSave = pos + 1;
			addWord(word, number, index);
			allNews[index].words += number * number;
			break;
		default:
			break;
		}
	}
	allNews[index].sqrt_words = sqrt(double(allNews[index].words));
}

// set recommended upper
void InvertedFile::setUpper(int k)
{
	assert(k > 0);
	if (recommendIndex)
		delete recommendIndex;
	if (recommendWeight)
		delete recommendWeight;
	recommendMax = k;
	recommendIndex = new int[listMax];
	recommendWeight = new double[listMax];
}

// add word and times in doclist
void InvertedFile::addWord(String & word, int number, int index)
{
	int times = number;

	auto result = dict.search(word);
	if (RESULT_ERROR(result))
	{
		result = dict.insert(DICT_PAIR(word, dict.getSize()));
		assert(result);
	}
	docList[RESULT_INDEX(result)].add(index, times);
}

// do all query operation
void InvertedFile::query()
{
	String data;
	String sentence;
	int length;
	int pos = 0, posSave = 0;
	FILE * fp(fopen("query1.txt", "rb"));
	if (!fp)
		printf("open query1.txt failed!\n");
	else
	{
		fseek(fp, 0, SEEK_END);
		length = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		data.resize(length + 5);
		fp >> data;
		fclose(fp);
		memset(queryIndex, 0, sizeof(queryIndex));
		memset(occurTimes, 0, sizeof(occurTimes));
		pos = posSave = 0;

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
	}

	fp = fopen("query2.txt", "rb");
	if (!fp)
		printf("open query2.txt failed!");
	{
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
}

// swap two index-value pairs for sort
void swap(int * allValue, int * allIndex, int * refer, int rankA, int rankB)
{
	int s = allIndex[rankA];
	allIndex[rankA] = allIndex[rankB];
	allIndex[rankB] = s;


	s = allValue[rankA];
	allValue[rankA] = allValue[rankB];
	allValue[rankB] = s;

	s = refer[rankA];
	refer[rankA] = refer[rankB];
	refer[rankB] = s;
}

// the child function for merge sort to merge two branches
void merge(int * allValue, int * allIndex, int * refer, int lo, int mid, int hi)
{
	int *A = allValue + lo;
	int *Ar = refer + lo;
	int *As = allIndex + lo;
	int lb = mid - lo;
	int *B = new int[lb];
	int *Br = new int[lb];
	int *Bs = new int[lb];
	for (int i = 0; i < lb; i++)
	{
		B[i] = A[i];
		Br[i] = Ar[i];
		Bs[i] = As[i];
	}
	int lc = hi - mid;
	int *C = allValue + mid;
	int *Cr = refer + mid;
	int *Cs = allIndex + mid;
	for (int i = 0, j = 0, k = 0; j < lb;)
	{
		if (k < lc && (C[k] > B[j] || (C[k] == B[j] && Cr[k] > Br[j])))
		{
			A[i] = C[k];
			Ar[i] = Cr[k];
			As[i++] = Cs[k++];
		}
		if (lc <= k || B[j] > C[k] || (B[j] == C[k] && Br[j] >= Cr[k]))
		{
			As[i] = Bs[j];
			Ar[i] = Br[j];
			A[i++] = B[j++];
		}
	}
	delete[] B;
	delete[] Bs;
	delete[] Br;
}

// use merge sort to sort it
void mergesort(int * allValue, int * allIndex, int * refer, int lo, int hi)
{
	if (hi - lo == 2)
	{
		if (allValue[lo] < allValue[lo + 1])
			swap(allValue, allIndex, refer, lo, lo + 1);
		return;
	}
	if (hi - lo < 2)
		return;
	int mid = (lo + hi) >> 1;
	mergesort(allValue, allIndex, refer, lo, mid);
	mergesort(allValue, allIndex, refer, mid, hi);
	merge(allValue, allIndex, refer, lo, mid, hi);
}

// sort for the occur times
int InvertedFile::sortRecorder()
{
	for (int i = 0; i < FILE_AMOUNT; i++)
		queryIndex[i] = i;

	mergesort(occurTimes, queryIndex, diffentWords, 0, FILE_AMOUNT);
	int cnt = -1;
	while (occurTimes[++cnt]);

	return cnt;
}

// finish news query by key word
void InvertedFile::queryNews(const String & sentence)
{
	if (!sentence.length())
		return;
	// first segmentation
	int sentenceLength = sentence.length();
	String keyTry;
	int index;
	FILE *fp(fopen("result1.txt", "ab"));

	memset(queryIndex, 0, sizeof(queryIndex));
	memset(occurTimes, 0, sizeof(occurTimes));
	memset(diffentWords, 0, sizeof(diffentWords));
	// as the shortesti word has 4 byte
	for (int pos = 0; pos < sentenceLength - 3; pos++)
	{
		// try to scan 8 character, but if not enough, try as long as possible
		for (int length = sentenceLength - pos > 15 ? 16 : (sentenceLength - pos); length >= 2; length--)
		{
			keyTry = sentence.substring(pos, length);
			if ((index = dict.getValue(keyTry)) != DICTIONARY_WRONG_RETURN)
			{
				auto p = docList[index].first();
				while (p)
				{
					// count occur times
					occurTimes[p->data.index] += p->data.times;
					diffentWords[p->data.index]++;
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
		fp << String("数据库中未找到合适新闻，请去掉不必要的空格并避免使用停用词");
	fp << String('\n');
	fclose(fp);
}

// finish news recommend by tittle
void InvertedFile::recommend(const String & tittle)
{
	if (tittle.length() < 2)
		return;
	FILE *fp(fopen("result2.txt", "ab"));
	bool founded = false;
	String pureTittle = tittle.removeSpace();
	for (auto & news : allNews)
	{
		if (pureTittle.tittleMatch(news.tittle))
		{
			recommendNews(news.index);
			if (recommendIndex[0] == -1 || recommendWeight[0] < RECOMMEND_LOWEST_WEIGHT)
				fp << String("未找到合适的推荐新闻，试试换个标题");
			else
				for (int i = 0; i < recommendMax; i++)
				{
					// if not get enough recommendation or weight is too light, ig
					if (recommendIndex[i] == -1 || recommendWeight[i] < RECOMMEND_LOWEST_WEIGHT)
						break;
					if (i)
						fp << String(',');
					fp << String('(') + String(allNews[recommendIndex[i]].index) + String(',') + allNews[recommendIndex[i]].tittle + String(')');
					// debug
					printf("%d %f\n", recommendIndex[i], recommendWeight[i]);
				}
			founded = true;
			break;
		}
	}
	if (!founded)
		fp << String("该新闻不在数据库中，无法推荐");
	fp << String('\n');
	fclose(fp);

}

// it's not worthy to write a new function, use a temporary macro is comvenient
#define swap(i,j) \
{\
	int t = recommendIndex[i];\
	double f = recommendWeight[i];\
	recommendIndex[i] = recommendIndex[j];recommendIndex[j] = t;\
	recommendWeight[i] = recommendWeight[j];recommendWeight[j] = f;\
}

// try to insert a recommendation by index and weight
void InvertedFile::insertRecommand(int index, double weight)
{
	for (int i = 0; i < listMax; i++)
		// if this news has already been recommended, just merge its weight
		if (recommendIndex[i] == index)
		{
			recommendWeight[i] += weight;
			// update order
			for (int j = i; i > 0; i--)
			{
				if (recommendWeight[i - 1] < recommendWeight[i])
					swap(i - 1, i);
			}
			return;
		}
	// if its weight is not enough, ignore it
	if (weight < recommendWeight[listMax - 1])
		return;
	// else we should add it into the recommendation
	recommendWeight[listMax - 1] = weight;
	recommendIndex[listMax - 1] = index;
	// this is a kind of insertion sort, because it's useful and listMax should be in 10
	for (int i = listMax - 1; i > 0; i--)
	{
		if (recommendWeight[i - 1] < recommendWeight[i])
			swap(i - 1, i);
	}
}

#undef swap

// recommend news from index
void InvertedFile::recommendNews(int index)
{
	assert(index >= 0 && index < FILE_AMOUNT);
	memset(recommendIndex, -1, sizeof(recommendIndex[0]) * listMax);
	memset(recommendWeight, 0, sizeof(recommendWeight[0]) * listMax);

	FILE * txt(fopen((String("./output/") + String(index) + String(".txt")).getString(), "rb"));
	String data;
	txt >> data;
	fclose(txt);

	// we use this news' word frequency to recommend
	int pos = data.indexOf("词频统计:");
	while (data[++pos] != '\n');
	int posSave = ++pos;
	int length = data.length();
	// count how many recommendations we got
	int successCount = 0;

	// save the max frequency of words in given news
	int maxFre = 0;

	while (pos < length)
	{
		// get word
		while (data[++pos] != '\t');
		auto result = dict.getValue(data.substring(posSave, pos - posSave));

		// get frequency
		pos += 2;
		int t = 0;
		while (data[pos] >= '0' && data[pos] <= '9')
		{
			t *= 10;
			t += data[pos++] - '0';
		}
		if (data[pos] == '\r')
			pos++;
		posSave = ++pos;

		auto pnode = docList[result].first();
		auto itself = pnode;
		assert(pnode);
		// first we find this news this word's frequency to calculate factor
		while (itself->data.index != index)
		{
			itself = itself->next;
		}
		// use influency calculate the importancy of this word in two news
		// then multiple them to get a weight for recommend
		double factor = itself->data.times / allNews[index].sqrt_words;
		double factor2, weight;

		while (pnode)
		{
			// we should ignore itself while scanning
			if (pnode != itself)
			{
				factor2 = pnode->data.times / allNews[pnode->data.index].sqrt_words;
				// important: use factor's product to present weight
				weight = factor * factor2;
				insertRecommand(pnode->data.index, weight);
			}
			pnode = pnode->next;
		}
	}

}
