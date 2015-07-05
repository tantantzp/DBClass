#include "SimSearcher.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <memory.h>

using namespace std;

#define EDMAX 100000


SimSearcher::SimSearcher()
{
    gramlen = 3;
    smin = 100000;
    distance = new int*[MAXLEN];
    for(int i = 0; i < MAXLEN; i++)
    {
        distance[i] = new int[MAXLEN];
    }
    for(int i = 0; i < MAXLEN; i++)
         distance[i][0] = i;
    for(int j = 1; j < MAXLEN; j++)
         distance[0][j] = j;
    for(int i = 1; i < MAXLEN; i++)
        for(int j = 1; j < MAXLEN; j++)
        {
            distance[i][j] = 0;
        }

}

SimSearcher::~SimSearcher()
{
    for(int i = 0; i <MAXLEN;i++)
    {
        delete[] distance[i];
    }
    delete distance;
    delete[] table;
}

void SimSearcher::showmap()
{
    vector<pair<int, int> >* tvector;
    for(mapiter = mapidx.begin();mapiter != mapidx.end(); mapiter++)
    {
        cout << mapiter->first <<" : ";
        tvector = &mapiter->second;
        vector<pair<int, int> >::iterator tvecit;
        for(tvecit = tvector->begin(); tvecit != tvector->end(); tvecit++)
        {
            cout <<'('<< tvecit->first<<',' << tvecit->second<< ')';
        }
        cout << endl;
    }
}

void SimSearcher::addIndex(string data, int index)
{
    map<string, int> grams = splitgram(data);
    map<string, int>::iterator gramiter;

    for(gramiter = grams.begin(); gramiter != grams.end(); gramiter++)
    {
        string gram = gramiter->first;
        int repeat = gramiter->second;

        mapiter = mapidx.find(gram);
        if(mapiter == mapidx.end())
        {
            vector<pair<int, int> > item;
            item.push_back(pair<int, int>(index, repeat));
            mapidx.insert(pair<string, vector<pair<int, int> > >(gram, item));
        }
        else
        {
            mapidx[gram].push_back(pair<int, int>(index, repeat));
        }
    }
    int gramcount = data.length() - gramlen + 1;
    gramcounts.push_back(gramcount);
    if(gramcount < smin) smin = gramcount;

    string gram;
    for(int i = 0; i < (int)(data.length() - gramlen + 1); i++)
    {
        gram = data.substr(i, gramlen);
        editer = mapidxEd.find(gram);
        if(editer == mapidxEd.end())
        {
            vector<int> item;
            item.push_back(index);
            mapidxEd.insert(pair<string, vector<int> >(gram, item));
        }
        else
        {
            mapidxEd[gram].push_back(index);
        }
    }

}

int SimSearcher::createIndex(const char *filename, unsigned q)
{
    gramlen = (int)q;
    string line;
    ifstream fin(filename);
    if(!fin)
    {
        cout << "Can't read file :";
        return FAILURE;
    }
    while (!fin.eof())
    {
        getline(fin, line);
        //line = line.substr(0, line.length()-1);
        vecstr.push_back(line);
    }
    fin.close();

    mapidx.clear();

    for(int i = 0; i < vecstr.size(); i++)
    {
        addIndex(vecstr[i], i);
    }
    table = new int[vecstr.size()];
    //showmap();
	return SUCCESS;
}

int SimSearcher::searchJaccard(const char *query, double threshold, vector<pair<unsigned, double> > &result)
{
	result.clear();
    string querystr(query);

    int gramcount = querystr.length()-gramlen + 1;
    //|GQ∩GS| ≥ max (τ *|GQ|, (|GQ|+|GSmin|)*τ/(1+τ))
    double t1 = threshold * gramcount;
    double t2 = (gramcount + smin) * threshold / (1 + threshold);
    int T = (int)max(t1, t2);
	if(T < 0 ) T = 0;

    memset(table, 0, sizeof(int) * vecstr.size());

    map<string, int> querygram = splitgram(querystr);
    map<string, int>::iterator gramit;
    for(gramit = querygram.begin(); gramit != querygram.end(); gramit++)
    {
        string gram = gramit->first;
        int repeat = gramit->second;
        mapiter = mapidx.find(gram);
        if(mapiter == mapidx.end()) { }
        else
        {
            vector<pair<int, int> >* tempvec = &mapidx[gram];
            vector<pair<int, int> >::iterator vecit;
            for(vecit = tempvec->begin(); vecit != tempvec->end(); vecit++)
            {
                int index = vecit->first;
                int irepeat =vecit->second;
                table[index]+= min(repeat, irepeat);
            }
        }
    }

    for(int i = 0; i < vecstr.size(); i++)
    {
        if(table[i] >= T)
        {
            string candidate = vecstr[i];
            double tjac = (double)(table[i]) / (gramcount + gramcounts[i] - table[i]);
            if(tjac >= threshold)
            {
                result.push_back(pair<unsigned, double>(i, tjac));
            }
        }

    }

    /*cout <<"show result"<<endl;
    for(int i = 0; i < result.size(); i++)
    {
        cout <<"result:"<<endl;
        int index = result[i].first;
        cout <<querystr<<endl;
        cout <<vecstr[index]<<endl;
        cout << index << ", "<< result[i].second << endl;
    }*/
	return SUCCESS;
}

int SimSearcher::searchED(const char *query, unsigned threshold, vector<pair<unsigned, unsigned> > &result)
{
	result.clear();
	string querystr(query);
    edthre = threshold;

	int T = querystr.length() - gramlen + 1 - gramlen * threshold;
	if(T < 0 ) T = 0;
    memset(table, 0, sizeof(int) * vecstr.size());

    if(T > 0)
    {
        string gram;
        for(int i = 0; i < (int)(querystr.length() - gramlen + 1); i++)
        {
            gram = querystr.substr(i, gramlen);
            editer = mapidxEd.find(gram);
            if(editer == mapidxEd.end()) { }
            else
            {
                vector<int>* tempvec = &mapidxEd[gram];
                vector<int>::iterator vecit;
                for(vecit = tempvec->begin(); vecit != tempvec->end(); vecit++)
                {
                    int index = (*vecit);
                    table[index]++;
                }
            }
        }

        for(int i = 0; i < vecstr.size(); i++)
        {
            if(table[i] >= T)
            {
                string canditate = vecstr[i];
                unsigned editdis = editDistance(querystr, canditate);
                if(editdis <= threshold)
                {
                    result.push_back(pair<unsigned, unsigned>(i, editdis));
                }

            }

        }
    }
    else
    {
        for(int i = 0; i < vecstr.size(); i++)
        {
            string canditate = vecstr[i];
            unsigned editdis = editDistance(querystr, canditate);
            if(editdis <= threshold)
            {
                result.push_back(pair<unsigned, unsigned>(i, editdis));
            }
        }
    }

    /*cout <<"show result"<<endl;
    for(int i = 0; i < result.size(); i++)
    {
        cout <<"result:"<<endl;
        int index = result[i].first;
        cout <<querystr<<endl;
        cout <<vecstr[index]<<endl;
        cout << index << ", "<< result[i].second << endl;
    }*/

	return SUCCESS;
}


int SimSearcher::editDistance(string source, string target)
{
     int lenthdif = (int)source.length() - (int)target.length();
     if(lenthdif < 0) lenthdif = -lenthdif;
     if(lenthdif > edthre) return edthre + 1;
     int i, j;

     for(i = edthre + 2; i <= source.length(); i++)
     {
         j = i - edthre - 1;
         distance[i][j] = EDMAX;
     }
     for(j = edthre + 2; j <= target.length(); j++)
     {
         i = j - edthre - 1;
         distance[i][j] = EDMAX;
     }

     for(i = 1; i <= source.length(); i++)
     {
         int jbegin = i - edthre;
         int jend = i + edthre;
         if(jbegin < 1) jbegin = 1;
         if(jend >target.length()) jend = target.length();
         bool toobig = true;
         for(j = jbegin; j <= jend; j++)
         {
             int tmp = 1;
             if((source[i - 1] == target[j - 1]))
             {
                 tmp = 0;
             }
             int edIns = distance[i][j - 1] + 1;
             int edDel = distance[i - 1][j] + 1;
             int edRep = distance[i - 1][j - 1] + tmp;
             distance[i][j] = min(min(edIns, edDel), edRep);
             if(distance[i][j] <= edthre) toobig = false;
         }
         if(toobig) return edthre + 1;

     }
     return distance[source.length()][target.length()];
}

map<string, int>  SimSearcher::splitgram(string data)
{
    map<string, int> grams;
    map<string, int>::iterator gramiter;
    for(int i = 0; i < (int)(data.length() - gramlen + 1); i++)
    {
        string gram = data.substr(i, gramlen);
        gramiter = grams.find(gram);
        if(gramiter == grams.end())
        {
            grams.insert(pair<string, int>(gram, 1));
        }
        else
        {
            grams[gram]++;
        }
    }
    return grams;
}

