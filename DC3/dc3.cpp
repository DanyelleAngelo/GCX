#include "dc3.hpp"
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstring>

using namespace std;


#define lessThanInInverseArray(d) (inverse[sa12[i]+d] < inverse[sa0[j]+d]) ? sa12[i++] : sa0[j++]

#define lessThanInText(d) (text[sa12[i]+d] < text[sa0[j]+d]) ? sa12[i++] : sa0[j++]

template <typename T>
void print(T v[], long int n){
    cout << *(v);
    for(long int i=1; i < n ; i++) cout  << ", " << *(v+i);
    cout << endl;
}

void dc3(long int *text, long int *sa, long int ts, long int level, int module, long int sigma) {
    long int sa12_size = ts - ceil((double)ts/module);
    long int sa0_size = ts - sa12_size;
    long int *sa12 = (long int *)calloc(sa12_size, sizeof(long int));
    long int *sa0 = (long int *)calloc(sa0_size, sizeof(long int));

    orderSA12(text, ts, sa12, sa12_size, level, module, sigma);
    orderSA0(text, sa12, ts, sa0, sa0_size, module, level, sigma);
    merge(sa, text, sa12, sa0, sa12_size, sa0_size, module, level, ts);
}

void orderSA12(long int *text, long int ts,long int *sa12, long int sa12_size, long int level, int module, long int sigma) {
    #if DSA12
        printf("\n######## Ordenando SA12 (NÍVEL %ld)\n\tTexto de entrada: ", level);
        print(text, ts);
    #endif
    long int *rank = (long int *) calloc(ts, sizeof(long int));
    long int *str12 = (long int *) calloc(sa12_size, sizeof(long int));
    
    for(long int i=0,j=0; j < ts; j++) {
        if(j % module != 0) {
            sa12[i] = j;
            str12[i++] = j;
        }
    }

    radix_sort(text, sa12, sa12_size, module, sigma);
    lex_names(text, sa12, rank, sa12_size, sigma, module);

    if(sigma < sa12_size) {
        long int redSize = getSizeOfReducedString(sa12_size, module);
        long int *textR = (long int *)calloc(redSize+module, sizeof(long int));
        long int *saR = (long int *)calloc(redSize+module, sizeof(long int));

        createStrReduced(str12, rank, textR, sa12_size, redSize, module);
        dc3(textR, saR, redSize, level+1, module, sigma+module);

        #if DSA12
            printf("\n######## Ordenando SA12 (NÍVEL %ld)\n", level);
            printf("\tApós realizar o merge, a string reduzida ordenada é: ");
            print(saR, redSize);
        #endif

        mapReducedStringToOriginalStr(saR, str12, sa12, sa12_size, 
        redSize, level);
        free(textR);
        free(saR);
    }

    free(rank);
    free(str12);
}

void orderSA0(long int * text, long int *sa12, long int ts,long int *sa0, long int sa0_size, int module, long int level, long int sigma) {
    long int sa12_size = ts - sa0_size;
    long int i=0;

    #if DSA0
        printf("\n######## Ordenando SA0 (NÍVEL %ld)\n ", level);
    #endif

    if( (ts -1) % module == 0 )sa0[i++] = ts-1;
    for(long int j=0; j < sa12_size; j++) {
        if(sa12[j] % module == 1) sa0[i++] = sa12[j]-1;
    }
    radix_sort(text, sa0, sa0_size, 1, sigma);

    #if DSA0
        printf("\tSA0 ordenado: ");
        print(sa0, sa0_size);
        printf("\n");
    #endif
}

void merge(long int *sa, long int *text,long int *sa12,long int *sa0,  long int sa12_size, long int sa0_size, int module, long int level, long int textSize) {
    long int *inverse;
    long int i = 0, j = 0, k = 0;
    constructInverseArray(sa12,inverse, textSize+module, sa12_size);

    #if DMERGE
        printf("######## Merge (NÍVEL %ld)\n", level);
        printf("\t SA12: ");
        print(sa12, sa12_size);
        printf("\t SA0: ");
        print(sa0, sa0_size);
    #endif

    while(i < sa12_size && j < sa0_size && k < textSize) {
        if(text[sa12[i]] != text[sa0[j]])sa[k] = lessThanInText(0);
        else {
            if(sa12[i] % module == 1) sa[k] = lessThanInInverseArray(1);
            else {
                if(text[sa12[i]+1] != text[sa0[j]+1])sa[k] = lessThanInText(1);
                else sa[k] = lessThanInInverseArray(2);
            }
        }
        k++;
    }

    for(; i < sa12_size; i++) sa[k++] = sa12[i];
    for(; j < sa0_size; j++) sa[k++] = sa0[j];

    free(inverse);
    #if DMERGE
        printf("\t SA12 e SA0 mergeados: ");
        print(sa, textSize);
    #endif
}

void radix_sort(long int *text, long int *sa, long int sa_size, int module, long int sigma) {
    long int *saTemp = (long int *) calloc(sa_size, sizeof(long int));
    long int *bucket = (long int *) calloc(sigma, sizeof(long int));
    #if DRADIX
        printf("\t## Radix sort ##\n");
        printf("\t\tString a ser ordenada: ");
        print(sa, sa_size);
    #endif

    for(long int d = module -1; d >=0; d--) {
        for(long int i=0; i < sigma;i++)bucket[i]=0;

        for(long int i=0; i < sa_size; i++) {
            long int cBucket = text[sa[i] + d];
            bucket[cBucket+1]++;
        }
        for(long int i = 1; i < sigma; i++) bucket[i] += bucket[i-1];
        for(long int i = 0; i < sa_size; i++) {
            long int cBucket = text[sa[i] + d];
            saTemp[bucket[cBucket]++] = sa[i];
        }
        for(long int i=0; i<sa_size; i++) sa[i] = saTemp[i];
    }
    free(bucket);
    free(saTemp);

    #if DRADIX
        printf("\t\tResultado do vetor de sufixos: ");
        print(sa, sa_size);
    #endif
}

void lex_names(long int *text, long int *sa, long int *rank, long int sa_size, long int &sigma, int module) {
    sigma=0;
    rank[sa[0]] = ++sigma;
    rank[sa[1]] = ++sigma;

    for(long int i=2; i < sa_size; i++) {
        bool equals = true;
        for(long int j=0; j < module; j++) {
            if(text[sa[i-1]+j] != text[sa[i]+j]){
                equals = false;
                break;
            }
        }
        rank[sa[i]] = equals ? sigma : ++sigma;
    }
}

void createStrReduced(long int *str12, long int *rank, long int *u, long int sa12_size, long int redSize, int module) {
    long int m = floor((double)redSize/2);
    u[m] = 0;

    #if DREDUCED
        printf("\n\t###### Montando a string reduzida para o vetor de rank\n\tTexto de entrada: ");
        print(rank, sa12_size);
    #endif

    for(long int i=0, l=0, r=m+1; i < sa12_size; i++){
        if(str12[i] % module == 1)
            u[l++] = rank[str12[i]];
        else
            u[r++] = rank[str12[i]];
    }
}

void mapReducedStringToOriginalStr(long int *saR, long int * str12,long int *sa12, long int sa12_size, long int redSize,  long int level) {
    long int m = ceil((double)sa12_size/2);
    long int r = redSize/2;
    #if DMAPPING
        printf("\n######## Mapeamento da string reduzida (NÍVEL %ld)\n", level);
        printf("\t Array de sufixos da string reduzida  (u) ordenada: ");
        print(saR, redSize);
    #endif

    for(long int i=0, j=0; i < redSize && j < sa12_size; i++) {
        if(saR[i] < m) {
            long int index = 2*saR[i];
            if(index < sa12_size) sa12[j++] = str12[index];
        } else if(saR[i] > r){
            long int index =  2 * (saR[i] - r) - 1;
            if(index < sa12_size) sa12[j++] = str12[index];
        }
    }

    #if DMAPPING
        printf("\t SA12 completamente ordenado após mapeamento: ");
        print(sa12, sa12_size);
    #endif
}

void constructInverseArray(long int *sa, long int*&inverse, long int text_size, long int sa12_size) {
    inverse = (long int *) calloc(text_size, sizeof(long int));
    for(long int i=0; i < sa12_size; i++) inverse[sa[i]] = i;
}

long int getSizeOfReducedString(long int sa12_size, long int module) {
    long int middle = ceil(sa12_size/2);
    long int firstHalf = (middle % module != 0) ? (middle /module +1)*module : middle+1;
    long int secondHalf = firstHalf +1;

    return firstHalf + secondHalf +1;
}

