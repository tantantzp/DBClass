#pragma once
#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <map>
#include <list>


using namespace std;

template <typename _IDType, typename _SimType>
struct JoinResult
{
	_IDType id1;
	_IDType id2;
	_SimType s;
};

typedef JoinResult<unsigned, double> JaccardJoinResult;
typedef JoinResult<unsigned, unsigned> EDJoinResult;

const int SUCCESS = 0;
const int FAILURE = 1;

static unordered_map< string, int > gram_map;
static unordered_map< string, int >::iterator gram_iter;


class SimJoiner
{
public:
    vector<string> file1_str;
    vector< vector<string> > file1_grams;
    vector<string> file2_str;
    vector< vector<string> > file2_grams;

    unordered_map< string, vector<int> > file2_idx;

    int** distance;
    int gramlen;
    int edthre;
    double jaccardSim;

	SimJoiner();
	~SimJoiner();

    int createIndex(vector<string>& file_str, vector< vector<string> >& file_grams);
    int readFile(const char *filename1, const char *filename2);
    void showgrams(vector< vector<string> >& file_grams, vector<string>& file_str);
    void showidx (unordered_map< string, vector<int> >& file_idx);
    void sortgrams(vector< vector<string> >& file_grams);
    void quick_sort(vector<string>& vecgram, int left, int right);
    void createPrefixId(vector< vector<string> >& file_grams, vector<string>& file_str, unordered_map< string, vector<int> >& file_idx);
    void createEdPrefixId(vector< vector<string> >& file_grams, vector<string>& file_str, unordered_map< string, vector<int> >& file_idx);
    int intersection(vector<string>& grams1, vector<string>& grams2);
    void cleanfile();

    int editDistance(string& source, string& target);
	int joinJaccard(const char *filename1, const char *filename2, unsigned q, double threshold, std::vector<JaccardJoinResult> &result);
	int joinED(const char *filename1, const char *filename2, unsigned q, unsigned threshold, std::vector<EDJoinResult> &result);

    int ditDistance(string source, string target);

};

