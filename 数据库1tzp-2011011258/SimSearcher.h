#pragma once
#include <vector>
#include <string>
#include <utility>
#include <map>
#include <list>

using namespace std;

#define MAXLEN  2050

const int SUCCESS = 0;
const int FAILURE = 1;


class SimSearcher
{
public:
    vector<string> vecstr;
    vector<int> gramcounts;
    int smin;
    int gramlen;

    map< string, vector<pair<int, int> > > mapidx;
    map< string, vector<pair<int, int> > >::iterator mapiter;
    map< string, vector<int> > mapidxEd;
    map< string, vector<int> >::iterator editer;
    int* table;
    int** distance;
    int edthre;
    //map< string, list<int>* > jaccardidx;
    //map< string, list<int>* >::iterator jaccarditer;

	SimSearcher();
	~SimSearcher();

    void addIndex(string data, int index);
    void showmap();
    int editDistance(string source, string target);
    map<string, int> splitgram(string data);

	int createIndex(const char *filename, unsigned q);
	int searchJaccard(const char *query, double threshold, std::vector<std::pair<unsigned, double> > &result);
	int searchED(const char *query, unsigned threshold, std::vector<std::pair<unsigned, unsigned> > &result);
};

