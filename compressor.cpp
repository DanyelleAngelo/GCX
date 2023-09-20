#include "compressor.hpp"
#include <iostream>
#include <vector>
#include <cstring>
#include <math.h>


using namespace std;

void error(const char *msg) {
    cout << "\x1b[31m[ERROR]\x1b[0m " << msg << endl;
    exit(EXIT_FAILURE);
}

void isFileOpen(FILE * file, const char *msg) {
    if(file != NULL) return;

    error(msg);
}

int padding(i32 textSize, int coverage){
    if(textSize > coverage && textSize % coverage != 0) {
        return coverage - (textSize % coverage);
    } else if(textSize % coverage !=0) {
        return coverage - textSize;
    }
    return 0;
}

void readPlainText(char *fileName, unsigned char *&text, i32 &textSize, int coverage) {
    FILE*  file= fopen(fileName,"r");
    isFileOpen(file, "An error occurred while opening the input plain file");

    fseek(file, 0, SEEK_END);
    textSize = ftell(file);
    i32 i = textSize;
    int nSentries=padding(textSize, coverage);
    textSize += nSentries;

    text = (unsigned char*)malloc(textSize*sizeof(unsigned char));
    while(i < textSize) text[i++] =0;

    fseek(file, 0, SEEK_SET);
    fread(text, 1, textSize-nSentries, file);
    fclose(file);
}

void radixSort(i32 *uText, i32 nTuples, i32 *tuples, i32 sigma, int coverage){
    i32 *tupleIndexTemp = (i32*) calloc(nTuples, sizeof(i32));
    
    for(int i=0, j=0; i < nTuples; i++, j+=coverage)tuples[i] = j;
    i32 *bucket =(i32*) calloc(sigma, sizeof(i32));

    for(int d= coverage-1; d >=0; d--) {
        for(int i=0; i < sigma;i++)bucket[i]=0;//TODO
        for(int i=0; i < nTuples; i++) bucket[uText[tuples[i] + d]+1]++; 
        for(int i=1; i < sigma; i++) bucket[i] += bucket[i-1];

        for(int i=0; i < nTuples; i++) {
            int index = bucket[uText[tuples[i] + d]]++;
            tupleIndexTemp[index] = tuples[i];
        }
        for(int i=0; i < nTuples; i++) tuples[i] = tupleIndexTemp[i];
    }

    free(bucket);
    free(tupleIndexTemp);
}

void createLexNames(i32 *uText, i32 *tuples, i32 *rank, i32 &qtyRules, long int nTuples, int coverage) {
    i32 name = 1;
    rank[tuples[0]/coverage] = name;

    for(i32 i=1; i < nTuples; i++) {
        bool equal = true;
        for(int j=0; j < coverage; j++){
            if(uText[tuples[i-1]+j] != uText[tuples[i]+j]){
                equal = false;
                break;
            }
        }

        if(equal) {
            rank[tuples[i]/coverage] = name;
        }
        else {
            rank[tuples[i]/coverage] = ++name;
        }
    }

    qtyRules = name;
    #if DEBUG == 1
        printf("## Número de trincas %d, quantidade de trincas únicas: %d.\n", nTuples, qtyRules);
    #endif
}
