#include "dc3.hpp"
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstring>

using namespace std;

#define calculatesLetterPosition(i) (sa[i] + d >= ts) ? 0 : text[sa[i] + d]

#define lessThanInInverseArray(d) (inverse_sa12[sa12[i]+d] < inverse_sa12[sa0[j]+d]) ? sa12[i++] : sa0[j++]

#define lessThanInText(d) (text[sa12[i]+d] < text[sa0[j]+d]) ? sa12[i++] : sa0[j++]

template <typename T>
void print(T v[], int n){
    cout << *(v);
    for(int i=1; i < n ; i++) cout  << ", " << *(v+i);
    cout << endl;
}

void dc3(unsigned char *text,int *sa, int ts, int level) {
    int sa12_size = ts - ceil((double)ts/module);
    int sa0_size = ts - sa12_size;
    int sa12[sa12_size];
    int sa0[sa0_size];
    orderSA12(text, ts, sa12, sa12_size, level);
    orderSA0(text, sa12, ts, sa0, sa0_size);
    merge(sa, text, sa12, sa0, sa12_size, sa0_size);
   // delete[] sa12;
    //delete[] sa0;
}

void orderSA12(unsigned char *text, int ts, int *sa12, int sa12_size, int level) {
    for(int i=0,j=1; j < ts; j++) {
        if(j % module != 0) sa12[i++] = j;
    }
    int *sa12_sorted = (int*) calloc(sa12_size, sizeof(int));
    copy(sa12, sa12+sa12_size, sa12_sorted);
    radix_sort(text, sa12_sorted, sa12_size, ts, module);
    int *rank = (int*) calloc(ts, sizeof(int));
    bool repetitions = lex_names(text,sa12_sorted, rank, sa12_size);
    //significa que há repetições de lex-names e portanto é preciso chamar DC3 recursivamente
    if(repetitions) {
        unsigned char *reduced_str = (unsigned char*)calloc(sa12_size+1, sizeof(unsigned char));
        createReducedStr(sa12, rank, reduced_str, sa12_size);
        int * sa = (int*)calloc(sa12_size+1, sizeof(int));
        dc3(reduced_str, sa, sa12_size+1, level+1);
        mapReducedStringToOriginalStr(sa, sa12, sa12_sorted, sa12_size);
        free(reduced_str);
        free(sa);
    }

    for(int i=0; i<sa12_size;i++)sa12[i] = sa12_sorted[i];
    free(rank);
    free(sa12_sorted);
}

void orderSA0(unsigned char * text, int *sa12, int ts, int *sa0, int sa0_size) {
    int sa12_size = ts - sa0_size;
    int i=0;

    //O último elemento do texto está em SA0, e já sabemos sua ordem, pois é aquele com menor ordem lexicográfica.
    if( (ts -1) % 3 == 0 )sa0[i++] = ts-1;

    //Sabemos a ordem relativa de SA0 (comparado à SA12), pois o sufixo que segue SA0[j] está em SA12, que já foi ordenado.
    for(int j=0; j < sa12_size; j++) {
        if(sa12[j] % 3 == 1) sa0[i++] = sa12[j]-1;
    }
    //Ordenamos SA0 olhando apenas para o primeiro caractere. Radix Sort é estável, por isso não perdemos o trabalho anterior.
    radix_sort(text, sa0, sa0_size, ts, 1);
}

void merge(int *sa, unsigned char *text, int *sa12, int *sa0,  int sa12_size, int sa0_size) {
    int ts = sa12_size+sa0_size;
    int*inverse_sa12 = constructInverseArray(sa12, ts, sa12_size);
    int i = 0, j = 0, k = 0;

    while(i < sa12_size && j < sa0_size) {
        sa[k] = (text[sa12[i]] == text[sa0[j]]) ? -1 : lessThanInText(0);
        if(sa[k] == -1) {
            if(sa12[i] % 3 == 1) {
                sa[k] = lessThanInInverseArray(1);
            }else {//Os sufixos que seguem os atuais estão separados
                sa[k] = (text[sa12[i]+1] == text[sa0[j]+1]) ? -1 : lessThanInText(1);

                //Os elementos possuem a primeira letra igual, precisamos deslocar o array em 2 posições e usar a matriz inversa
                if(sa[k] < 0){
                    sa[k] = lessThanInInverseArray(2);
                }
            }
        }
        k++;
    }

    for(; i < sa12_size; i++) sa[k++] = sa12[i];
    for(; j < sa0_size; j++) sa[k++] = sa0[j];

    free(inverse_sa12);
}

void radix_sort(unsigned char *text, int *sa, int sa_size, int ts, int n_char) {
    int *saTemp = (int*) calloc(sa_size, sizeof(int));

    for(int d = n_char -1; d >=0; d--) {
        int *position = (int*) calloc(255, sizeof(int));

        for(int i=0; i < sa_size; i++) {
            int chPosition = calculatesLetterPosition(i);
            position[chPosition+1]++;
        }

        //correlaciona cada elemento com relação a quantidade de elementos existentes anteriormente
        for(int j = 1; j < 255; j++) position[j] += position[j-1];

        for(int i = 0; i < sa_size; i++) {
            int chPosition = calculatesLetterPosition(i);
            int index = position[chPosition]++;
            saTemp[index] = sa[i];
        }

        for(int i=0; i<sa_size; i++) sa[i] = saTemp[i];
        free(position);
    }
    free(saTemp);
}

bool lex_names(unsigned char *text, int *sa, int*rank, int sa_size) {
    bool repetitions = false;
    rank[sa[0]] = 0;
    rank[sa[1]] = 1;

    for(int i=2; i < sa_size; i++) {
        bool equals = true;
        for(int j=0; j < module; j++) {
            if(text[sa[i-1]+j] != text[sa[i]+j]){
                equals = false;
                break;
            }
        }
        rank[sa[i]] = equals ? rank[sa[i-1]] : rank[sa[i-1]]+1;
        if(equals == true) repetitions = true;
    }
    return repetitions;
}

void createReducedStr(int *sa12, int *rank, unsigned char *u, int sa12_size) {
    int m = ceil((double)sa12_size/2);
    u[m] = 0;
    //precisa usar a ordem original de SA12
    for(int i=0, f=0, s=m+1; i < sa12_size; i++){
        if(sa12[i] % module == 1 && f < m) {
            u[f] = rank[sa12[i]];
            f++;
        } else {
            u[s] = rank[sa12[i]];
            s++;
        }
    }
}

void mapReducedStringToOriginalStr(int *reduced_sa, int * sa12, int *sa12_sorted, int sa12_size) {
    int index_in_sa12;
    int m = ceil((double)sa12_size/2);
    int *saTemp = (int*) calloc(sa12_size, sizeof(int));

    for(int i =0, j=0; i < sa12_size+1; i++) {
        if(reduced_sa[i] == m) continue;

        if(reduced_sa[i] < m) {
            index_in_sa12 = (2*reduced_sa[i]);
            saTemp[j++] = sa12[index_in_sa12];
        } else {
            index_in_sa12 = 2 * (reduced_sa[i] - m - 1) + 1;
            saTemp[j++] = sa12[index_in_sa12];
        }
    }

    for(int i=0; i < sa12_size; i++)sa12_sorted[i] = saTemp[i];
    free(saTemp);
}

int * constructInverseArray(int *sa, int text_size, int sa12_size) {
    int * inverse = (int*) calloc(text_size, sizeof(int));
    for(int i=0; i < sa12_size; i++) {
        inverse[sa[i]] = i;
    }
    return inverse;
}

