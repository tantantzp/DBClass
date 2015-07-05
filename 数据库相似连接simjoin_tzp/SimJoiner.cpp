#include "SimJoiner.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <functional>
#include <map>

using namespace std;


#define MAXLEN 2050
#define EDMAX 1000000

SimJoiner::SimJoiner()
{
    gramlen = 3;
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

SimJoiner::~SimJoiner()
{
    cleanfile();
    for(int i = 0; i <MAXLEN;i++)
    {
        delete[] distance[i];
    }
    delete distance;
}
void SimJoiner::cleanfile()
{
    file1_str.clear();
    file2_str.clear();
    file1_grams.clear();
    file2_grams.clear();
    file2_idx.clear();
    gram_map.clear();
}

int SimJoiner::readFile(const char *filename1, const char *filename2)
{
    string line;
    ifstream fin(filename1);
    if(!fin)
    {
        cout << "Can't read file1 :";
        return FAILURE;
    }
    //while (!fin.eof())
    while(getline(fin, line))
    {
        //line = line.substr(0, line.length()-1);
        file1_str.push_back(line);
    }
    fin.close();
    ifstream fin2(filename2);
    if(!fin2)
    {
        cout << "Can't read file2 :";
        return FAILURE;
    }
    while (getline(fin2, line))
    {
        //line = line.substr(0, line.length()-1);
        file2_str.push_back(line);
    }
    fin2.close();
    return SUCCESS;
}


void SimJoiner::showgrams(vector< vector<string> >& file_grams, vector<string>& file_str)
{
    cout <<"showGrams"<<endl;
    for(int i = 0; i < file_grams.size(); i ++)
    {
        cout <<"id = " << i << " - " << file_str[i]<<" : ";
        vector<string>& gclass = file_grams[i];
        for(int j = 0; j < gclass.size(); j ++)
        {
            cout << gclass[j] <<": "<<gram_map[ gclass[j] ]<<" ";
        }
        cout << endl;
    }
}

void SimJoiner::showidx (unordered_map< string, vector<int> >& file_idx)
{
    cout <<"show fileidx"<<endl;
    unordered_map< string, vector<int> >::iterator mapiter;
    for(mapiter = file_idx.begin(); mapiter != file_idx.end(); mapiter++)
    {
        cout << mapiter->first <<" : ";
        vector<int>& vint = mapiter->second;
        for(int i = 0; i < vint.size(); i ++)
        {
            cout <<vint[i] << ", ";
        }
        cout << endl;
    }
   // cout <<"end show" <<endl;
}

int SimJoiner::createIndex(vector<string>& file_str, vector< vector<string> >& file_grams)
{
    for(int i = 0; i < file_str.size(); i++)
    {
        string data = file_str[i];
        vector<string> tgrams;
        for(int i = 0; i < (int)(data.length() - gramlen + 1); i++)
        {
            string gram = data.substr(i, gramlen);
            gram_map[gram]++;
            tgrams.push_back(gram);
        }
        file_grams.push_back(tgrams);
    }

	return SUCCESS;
}

void SimJoiner::quick_sort(vector<string>& vecgram, int left, int right)
{
    if(left < right)
    {
        int l = left, r = right;
        string sgram = vecgram[l];
        int x = gram_map[ vecgram[l] ];
        while(l < r)
        {
            while(l < r && gram_map[ vecgram[r] ] >= x) r--;
            if(l < r)
                vecgram[l++] = vecgram[r];
            while(l < r && gram_map[ vecgram[l] ] <= x) l++;
            if(l < r)
                vecgram[r--] = vecgram[l];
        }
        vecgram[l] = sgram;
        quick_sort(vecgram, left, l - 1);
        quick_sort(vecgram, l + 1, right);
    }
}

void SimJoiner::sortgrams(vector< vector<string> >& file_grams)
{
    for(int i = 0; i < file_grams.size(); i ++)
    {
        vector<string>& gclass = file_grams[i];
        quick_sort(gclass, 0, gclass.size() - 1);
    }
}

void SimJoiner::createPrefixId(vector< vector<string> >& file_grams, vector<string>& file_str, unordered_map< string, vector<int> >& file_idx)
{
    double prefix_len = 0.0;
    int pre_gramnum;
    for(int i = 0; i < file_grams.size(); i ++)
    {
         int gramnum1 = file_str[i].length() - gramlen + 1;
         prefix_len = (double)(1 - jaccardSim) * gramnum1 + 1.0;
         pre_gramnum = floor(prefix_len);
         if(pre_gramnum <= 0) pre_gramnum = 1;
         vector<string>& vgram = file_grams[i];
         for(int j = 0; j < pre_gramnum ; j ++)
         {
             string gram = vgram[j];
             //file_idx[gram].push_back(i);
             vector<int>& tvec = file_idx[gram];
             if(tvec.empty() || tvec[tvec.size() - 1] != i)
                 tvec.push_back(i);
         }
    }
}
void SimJoiner::createEdPrefixId(vector< vector<string> >& file_grams, vector<string>& file_str, unordered_map< string, vector<int> >& file_idx)
{
    int pre_gramnum = edthre * gramlen + 1;
    for(int i = 0; i < file_grams.size(); i ++)
    {
         int gramnum = file_str[i].length() - gramlen + 1;
         int num = min(pre_gramnum, gramnum);
         //cout <<"i " << i <<"--" <<prefix_len<<" gramnum:"<<gramnum <<endl;
         vector<string>& vgram = file_grams[i];
         for(int j = 0; j < num ; j ++)
         {
             string gram = vgram[j];
             vector<int>& tvec = file_idx[gram];
             if(tvec.empty() || tvec[tvec.size() - 1] != i)
                 tvec.push_back(i);
         }
    }
}

int SimJoiner::joinJaccard(const char *filename1, const char *filename2, unsigned q, double threshold, vector<JaccardJoinResult> &result)
{
	result.clear();
    gramlen = (int)q;
    jaccardSim = threshold;

    cleanfile();
    readFile(filename1, filename2);

    createIndex(file1_str, file1_grams);
    createIndex(file2_str, file2_grams);

    sortgrams(file1_grams);
    sortgrams(file2_grams);

    //cout <<"after sorting"<<endl;
    //showgrams(file1_grams, file1_str);
    //cout <<"file2_grams)"<<endl;
    //showgrams(file2_grams, file2_str);
    createPrefixId(file2_grams, file2_str, file2_idx);
    //showidx(file2_idx);
    int prefix_len = 0;

    for(int i = 0; i < file1_str.size(); i++)
    {
        int gramnum = file1_grams[i].size();
        prefix_len = floor((1.0 - jaccardSim) * gramnum + 1.0);
        vector<int> candidates ;
        vector<string>& vecgram = file1_grams[i];

        if(prefix_len <= gramnum)
        {
            map<int, int> can_map;

            for(int j = 0; j < prefix_len; j ++)
            {
                string gram = vecgram[j];
                vector<int>& veci = file2_idx[gram];   //search inverted table
                if(!veci.empty())
                {
                    for(int k = 0; k < veci.size(); k ++)
                    {
                        can_map[ veci[k] ] ++;
                    }
                }
            }
            for(map<int, int>::iterator can_it = can_map.begin(); can_it != can_map.end(); can_it++)
            {
                candidates.push_back(can_it->first);
            }
        }
        else
        {
            for(int j = 0; j < file2_str.size(); j ++)
                candidates.push_back(j);
        }

        //cout <<"number " << i <<" : ";

        //for(int j = 0; j < candidates.size(); j++)
        //{
        //   cout <<"("<<i<<"-"<<candidates[j]<<")";
        //}
        //cout << endl;
        for(int j = 0; j < candidates.size(); j++)
        {
            int can_index = candidates[j];
            int gramnum2 = file2_str[can_index].length() - gramlen + 1;
            int internum = intersection(vecgram, file2_grams[can_index]);
            double jaccard_res = double(internum / (double)(gramnum + gramnum2 - internum));
            if(jaccard_res >= jaccardSim)
            {
                JaccardJoinResult temp_res;
                temp_res.id1 = i;
                temp_res.id2 = can_index;
                temp_res.s = jaccard_res;
                result.push_back(temp_res);
            }
        }
    }

    /*for(int i = 0; i < result.size(); i++)
    {
        int id1 = result[i].id1;
        int id2 = result[i].id2;
        double sim = result[i].s;
        cout <<file1_str[id1] <<"--"<<file2_str[id2] <<endl;
        cout <<"id1: "<<id1<<" id2: "<<id2<<" : "<<sim<<endl;
    }*/

	return SUCCESS;
}

int SimJoiner::joinED(const char *filename1, const char *filename2, unsigned q, unsigned threshold, vector<EDJoinResult> &result)
{
	result.clear();
    gramlen = (int)q;
    edthre = (int)threshold;
    cleanfile();
    readFile(filename1, filename2);
    createIndex(file1_str, file1_grams);
    createIndex(file2_str, file2_grams);
    //showgrams(file1_grams, file1_str);

    sortgrams(file1_grams);
    sortgrams(file2_grams);

    //cout <<"after sorting"<<endl;
    //showgrams(file1_grams, file1_str);
    //cout <<"file2_grams)"<<endl;
    //showgrams(file2_grams, file2_str);

    createEdPrefixId(file2_grams, file2_str, file2_idx);
    //showidx(file2_idx);

    //map< string, vector<int> >::iterator mapiter;
    int pre_gramnum = edthre * gramlen + 1;
    for(int i = 0; i < file1_str.size(); i++)
    {
        vector<string>& vecgram = file1_grams[i];
        vector<int> candidates ;
        if(vecgram.size() >= pre_gramnum)
        {
            int gramnum = pre_gramnum;
            map<int, int> can_map;

            for(int j = 0; j < gramnum; j ++)
            {
                string gram = vecgram[j];
                vector<int>& veci = file2_idx[gram];   //search inverted table
                if(!veci.empty())
                {
                    for(int k = 0; k < veci.size(); k ++)
                    {
                        can_map[ veci[k] ] ++;
                    }
                }
            }
            for(map<int, int>::iterator can_it = can_map.begin(); can_it != can_map.end(); can_it++)
            {
                candidates.push_back(can_it->first);
            }
        }
        else
        {
            for(int j = 0; j < file2_str.size(); j ++)
                candidates.push_back(j);
        }
        //cout <<"number " << i <<" : ";
        //for(int j = 0; j < candidates.size(); j++)
        //{
        //   cout <<"("<<i<<"-"<<candidates[j]<<")";
        //}
        //cout << endl;
        string str1 = file1_str[i];
        for(int j = 0; j < candidates.size(); j++)
        {
            int can_index = candidates[j];
            string str2 = file2_str[can_index];
            /*int maxlen = max(str1.length(), str2.length());
            int least = maxlen - gramlen + 1 - gramlen * edthre;
            int internum = intersection(vecgram, file2_grams[can_index]);
            if(internum >= least)
            {*/
                int edDistance = editDistance(str1, str2);
                if(edDistance <= edthre)
                {
                    EDJoinResult temp_res;
                    temp_res.id1 = i;
                    temp_res.id2 = can_index;
                    temp_res.s = (unsigned)edDistance;
                    result.push_back(temp_res);
                }
            //}
            //else cout <<"filter out "<<endl;
        }
    }
    /*for(int i = 0; i < result.size(); i++)
    {
        int id1 = result[i].id1;
        int id2 = result[i].id2;
        unsigned sim = result[i].s;
        cout <<file1_str[id1] <<"--"<<file2_str[id2] <<endl;
        cout <<"id1: "<<id1<<" id2: "<<id2<<" : "<<sim<<endl;
    }*/

	return SUCCESS;
}
int SimJoiner::intersection(vector<string>& grams1, vector<string>& grams2)
{
    int internum = 0;
    map<string, int> gn1;
    map<string, int> gn2;
    map<string, int>::iterator gnit;
    for(int i = 0; i < grams1.size(); i ++)
    {
        gn1[grams1[i]]++;
    }
    for(int i = 0; i < grams2.size(); i ++)
    {
        gn2[grams2[i]]++;
    }
    if(gn1.size() <= gn2.size())
    {
        for(gnit = gn1.begin(); gnit != gn1.end(); gnit++)
        {
            int n1 = gnit->second;
            int n2 = gn2[gnit->first];
            int num = min(n1, n2);
            internum += num;
        }
    }
    else
    {
        for(gnit = gn2.begin(); gnit != gn2.end(); gnit++)
        {
            int n1 = gnit->second;
            int n2 = gn1[gnit->first];
            int num = min(n1, n2);
            internum += num;
        }
    }
    return internum;

}

int SimJoiner::editDistance(string& source, string& target)
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
