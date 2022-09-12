#include <iostream>
#include <string.h>
#include <cmath>
#include <vector>
#include <radix_sort.hpp>
#include <merge_sort.hpp>
#include <utils.hpp>
#include <dc3.hpp>

int m, m0,m12;

vector<int> dc3(char *ch) {
    m = strlen(ch), m0 = ceil((double)m/3), m12=ceil((double)(m*2)/3);
    int u[m12+1], sa12_rank[m12];
    vector<int> sa12(m12), sa0(m0);
    vector<char*> sa12_ch, sa0_ch;

    build_sas(ch, sa12, sa0, sa12_ch, sa0_ch);
    sa_rank(sa12_ch, sa12_rank);//computa a classificação dos elementos de sa12
    
    if(sa12_rank[m12-1] != m12-1) {
        build_u(u, sa12,sa12_rank);
        //TODO ordena sa12 com base em u
    }

    order_sa0(sa0_ch, sa12_ch, sa0);
    
    vector<int> sa(m);
    build_suffix_array(sa, sa0,sa12, ch);
    return sa;
}

void build_sas(char*ch, vector<int> &sa12, vector<int> &sa0, vector<char*> &sa12_ch, vector<char*> &sa0_ch) {
    for(int i=0, j=0,k=0; i < m; i++) {
        if(i % 3 != 0){
            sa12[j] = i;
            char *temp;
            int tam = (i+2) < m ? 3 : m - i; 
            temp = (char*)malloc(sizeof(char)*(tam+1));
            memcpy(temp, &ch[i], tam);
            temp[tam] = '\0';
            sa12_ch.push_back(temp);
            j++;
        }
        else {
            sa0[k] = i;
            char *temp;
            temp = (char*)malloc(sizeof(char)*(2));
            memcpy(temp, &ch[i], 1);
            temp[1] = '\0';
            sa0_ch.push_back(temp);
            k++;
        }
    }

    radix_sort(sa12_ch, 2, sa12);
    radix_sort(sa0_ch, 1, sa0);
}

void sa_rank(vector<char*> sa, int sa12_rank[]){
    sa12_rank[0] = 0;
    for(int i=1, j=1; i < m12; i++){
        //TODO tratar maiúsculas e minusculas
        sa12_rank[i] = strcmp(sa[i], sa[i-1]) ? j++ : sa12_rank[i-1];
    }
}

void build_u(int *u, vector<int> sa12, int* sa12_rank) {
    int m = ceil(m12/2);
    u[m] = -1; //sentinela
    for(int i=0, j = m-1, k=m12; i < m12; i++) {
        if(j >=0 && sa12[i] % 3 == 2) { //sa_12 n tem ligação com sa_rank (sa_rank esta ordenado, sa12 n)
            u[j--] = sa12_rank[i];
        } else if(k > m) {
            u[k--] = sa12_rank[i];
        }
    }
    merge_sort(u,0,m12+1);
}

void order_sa0(vector<char*> sa0_ch, vector<char*> sa12_ch, vector<int> sa0) {
    for(int i=1; i < m0; i++) {
        //se as strings são iguais, a gente verifica quem seria o sufixo de SA0[i] em SA[12]
        if(strcmp(sa0_ch[i], sa0_ch[i-1])==0 && strcmp(sa12_ch[i+1], sa0_ch[i])) {
            swap(sa0_ch[i], sa0_ch[i-1]);
            int temp = sa0[i];
            sa0[i] = sa0[i-1];
            sa0[i-1] = temp;
        }
    }
}

void build_suffix_array(vector<int> &sa, vector<int> sa0, vector<int> sa12, char* ch) {
    for(int i=0, j=0, k=0; i< m; i++) {
        //primeiro caractere é igual, desempata de acordo com o tamanho da string
        if(ch[sa0[j]] == ch[sa12[k]]) {
            if(sa0[j] > sa12[k]) sa[i] = sa0[j++];
            else sa[i] = sa12[k++];
        } else if(ch[sa0[j]] < ch[sa12[k]]) {//o primeiro caractere de sa0 é menor
            sa[i] = sa0[j++];
        } else {//o primeiro caractere de sa12 é menor
            sa[i] = sa12[k++];
        }
    }
}