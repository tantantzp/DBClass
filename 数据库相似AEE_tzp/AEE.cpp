#include "AEE.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <unordered_map>
#include <map>

using namespace std;


#define MAXLEN 2050
#define EDMAX 1000000


AEE::AEE()
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

AEE::~AEE()
{
    for(int i = 0; i <MAXLEN;i++)
    {
        delete[] distance[i];
    }
    delete distance;
}



void AEE::showidx (unordered_map< string, vector<int> >& file_idx)
{
    cout <<"show fileidx"<<endl;
    unordered_map< string, vector<int> >::iterator mapiter;
    //int num = 0;
    for(mapiter = file_idx.begin(); mapiter != file_idx.end(); mapiter++)
    {
        //num ++;
        //if(num > 5) break;
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
int AEE::readFile(const char *filename,vector<string>& vecstr)
{
    string line;
    ifstream fin(filename);
    if(!fin)
    {
        cout << "Can't read file:";
        return FAILURE;
    }
    /*while (!fin.eof())
    {
        getline(fin, line);
        //line = line.substr(0, line.length()-1);
        vecstr.push_back(line);
    }*/
    while(getline(fin, line))
    {
        //line = line.substr(0, line.length()-1);
        vecstr.push_back(line);
    }
    fin.close();
    //cout << vecstr[4297] <<":::";
    return SUCCESS;
}

int AEE::createfileIDX(vector<string>& file_str, unordered_map< string, vector<int> >& file_idx)
{

    for(int i = 0; i < file_str.size(); i++)
    {
        string data = file_str[i];
        for(int j = 0; j < (int)(data.length() - gramlen + 1); j++)
        {
             string gram = data.substr(j, gramlen);
             vector<int>& tvec = file_idx[gram];
             if(tvec.empty() || tvec[tvec.size() - 1] != i)
                 tvec.push_back(i);
        }

    }

	return SUCCESS;
}

int AEE::createIndex(const char *filename, unsigned q)
{
    gramlen = q;
    readFile(filename, dictionary);
    createfileIDX(dictionary, dic_idx);

	return SUCCESS;
}

int AEE::aeeJaccard(const char *doc, double threshold, vector<JaccardExtractResult> &result)
{
	result.clear();
	string docstr = doc;


	return SUCCESS;
}

void AEE::array_count(int dic_num, int doc_num, string& doc_str, vector<string>& doc_gram, int* table,vector<EDExtractResult> &result)
{
    int doc_gramnum = doc_gram.size();
    string dic_str = dictionary[dic_num];
    int dic_strlen = dic_str.length();
    int min_len = dic_strlen - edthre;
    int max_len = dic_strlen + edthre;
    int* count_array = new int[doc_gramnum];
    memset(count_array, 0, sizeof(int) * doc_gramnum );
    int* thre_array = new int[edthre*2 + 1];

    for(int t_len = min_len; t_len <= max_len; t_len++)
    {
        thre_array[t_len - min_len] = max(dic_strlen, t_len) - gramlen + 1 - edthre*gramlen;
    }
    for(int i = 0; i < (doc_gramnum - min_len + 2); i++)
    {
        for(int t_len = min_len; t_len <= max_len; t_len++)
        {
             if(t_len == min_len)
             {
                 for(int j = i; j < i + (t_len - gramlen + 1); j ++)
                 {
                     count_array[i] += table[j];
                 }
                 if(count_array[i] >= thre_array[0])
                 {
                     string t_str = doc_str.substr(i, t_len);
                     int edDistance = editDistance(t_str, dic_str);
                     if(edDistance <= edthre)
                     {
                        EDExtractResult temp_res;
                        temp_res.id = (unsigned)dic_num;
                        temp_res.pos = (unsigned)i;
                        temp_res.len = (unsigned)t_len;
                        temp_res.sim = (unsigned)edDistance;

                        result.push_back(temp_res);

                     }
                 }
             }
             else
             {
                 int ni = i + t_len - gramlen;
                 if(ni < doc_gramnum)
                 {
                     count_array[i] +=  table[ni];
                     if(count_array[i] >= thre_array[t_len - min_len])
                     {
                         string t_str = doc_str.substr(i, t_len);
                         int edDistance = editDistance(t_str, dic_str);
                         if(edDistance <= edthre)
                         {
                            EDExtractResult temp_res;
                            temp_res.id = (unsigned)dic_num;
                            temp_res.pos = (unsigned)i;
                            temp_res.len = (unsigned)t_len;
                            temp_res.sim = (unsigned)edDistance;

                            result.push_back(temp_res);

                         }
                     }
                 }
                 else break;

             }
        }
    }

    delete [] count_array;
    delete [] thre_array;
}


void AEE::do_aeeED(string doc, int doc_num, vector<EDExtractResult> &result)
{
    vector<string> doc_gram;
    vector<vector<int>*> doc_idx;
    vector<int> doc_idx_index;
    vector<int> doc_idx_p;
    int doc_gramnum = (int)(doc.length() - gramlen + 1);

    for(int i = 0; i < doc_gramnum; i++)
    {
        string gram = doc.substr(i, gramlen);
        doc_gram.push_back(gram);
        vector<int>& veci = dic_idx[gram];   //search inverted table

        if(!veci.empty())
        {
            doc_idx.push_back(&veci);
            doc_idx_index.push_back(i);
            doc_idx_p.push_back(0);
        }
    }

    /*for(int i = 0;i <doc_gramnum; i ++) cout <<doc_gram[i]<<",";
    cout <<endl;
    cout <<doc_num<<":"<< doc<<endl;
    for(int i = 0; i < doc_idx.size(); i++)
    {
        cout <<doc_gram[doc_idx_index[i]] <<"::";
        vector<int>* tvec = doc_idx[i];
        for(int j = 0; j < tvec->size(); j++)
        {
            cout << (*tvec)[j] <<",";
        }
        cout << endl;
    }*/

    int curr_index = 2147483647;
    int* table = new int[doc_gramnum + 1];
    int  curr_num = 0;
    while(1)
    {
        curr_index = 2147483647;
        memset(table, 0, (doc_gramnum + 1) * sizeof(int));
        curr_num = 0;
        for(int i = 0; i < doc_idx.size(); i ++)
        {
            vector<int>* tvec = doc_idx[i];
            int& pos = doc_idx_p[i];
            if(pos < tvec->size())
            {
                int temp = (*tvec)[pos];
                if(temp < curr_index)
                {
                    curr_index  = temp;
                }
            }
        }
        for(int i = 0; i < doc_idx.size(); i ++)
        {
            vector<int>* tvec = doc_idx[i];
            int& pos = doc_idx_p[i];
            if(pos < tvec->size())
            {
                int temp = (*tvec)[pos];
                if(temp == curr_index)
                {
                    table[doc_idx_index[i]] = 1;
                    pos ++;
                    curr_num ++;
                }
            }
        }

        if(curr_num == 0) break;
        string curr_entity = dictionary[curr_index];
        int thre = curr_entity.length() - gramlen + 1 - edthre * gramlen;
        /*cout <<"current_index:" <<curr_index <<","<<curr_num<<endl;
        for(int i = 0; i < doc_gramnum; i ++)
        {
            cout << table[i] <<',';
        }
        cout << endl;

        cout <<curr_entity <<","<<thre<<endl;*/

        if(curr_num < thre) continue;
        array_count(curr_index, doc_num, doc, doc_gram, table, result);

    }


}


int AEE::aeeED(const char *doc, unsigned threshold, vector<EDExtractResult> &result)
{
	result.clear();
	string docstr = doc;
	edthre = threshold;
	int docstr_pos = 0;
	int doc_num = 0;
	//showidx(dic_idx);

   /* for(int str_p = 0; str_p < docstr.size(); str_p ++)
    {
        if(docstr[str_p] == '\n')
        {
            int subdoc_len = str_p - docstr_pos;

            if(subdoc_len > 0)
            {
                string subdoc =  docstr.substr(docstr_pos, subdoc_len);
                //cout <<doc_num<< ":" <<subdoc<<endl;
                do_aeeED(subdoc, doc_num, result);

                /*doc_num ++;
            }
            docstr_pos = str_p + 1;
        }
    }*/
    do_aeeED(docstr, doc_num, result);

    /*for(int i = 0; i < min((int)result.size(), 100); i ++)
    {
        cout <<i<<":"<< result[i].id <<","<<result[i].pos<<","<<result[i].len<<","<<result[i].sim<<endl;
    }*/

	return SUCCESS;
}


int AEE::editDistance(string& source, string& target)
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

