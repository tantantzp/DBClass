#pragma once
#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <map>
#include <list>
#include "memory.h"

using namespace std;
template <typename _IDType, typename _PosType, typename _LenType, typename _SimType>
struct ExtractResult
{
	_IDType id;
	_PosType pos;
	_LenType len;
	_SimType sim;
};

typedef ExtractResult<unsigned, unsigned, unsigned,  unsigned> EDExtractResult;
typedef ExtractResult<unsigned, unsigned, unsigned,  double> JaccardExtractResult;

const int SUCCESS = 0;
const int FAILURE = 1;

class AEE {
public:

	int gramlen;
    vector<string> dictionary;
    unordered_map< string, vector<int> > dic_idx;
    //map< string, vector<int> > dic_idx;
    int** distance;
    int edthre;
	AEE();
	~AEE();
	int createIndex(const char *filename, unsigned q);
	int aeeJaccard(const char *doc, double threshold, std::vector<JaccardExtractResult> &result);
	int aeeED(const char *doc, unsigned threshold, std::vector<EDExtractResult> &result);
    void array_count(int dic_num, int doc_num, string& doc_str, vector<string>& doc_gram, int* table,vector<EDExtractResult> &result);
    void do_aeeED(string doc, int doc_num, vector<EDExtractResult> &result);

    void showidx (unordered_map< string, vector<int> >& file_idx);
    int readFile(const char *filename,vector<string>& vecstr);
    int createfileIDX(vector<string>& file_str, unordered_map< string, vector<int> >& file_idx);
    int editDistance(string& source, string& target);
};
