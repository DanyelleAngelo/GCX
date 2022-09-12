#ifndef DC3_H
#define DC3_H

using namespace std;

vector<int> dc3(char *ch);

void build_sas(char*ch, vector<int> &sa12, vector<int> &sa0, vector<char*> &sa12_ch, vector<char*> &sa0_ch);

void sa_rank(vector<char*> sa, int sa12_rank[]);

void build_u(int *u, vector<int> sa12, int* sa12_rank);

void order_sa0(vector<char*> sa0_ch, vector<char*> sa12_ch, vector<int> sa0);

void build_suffix_array(vector<int> &sa, vector<int> sa0, vector<int> sa12, char* ch);

#endif