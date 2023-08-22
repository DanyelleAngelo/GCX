#include "compressor.hpp"
#include <iostream>
#include <vector>
#include <cstring>
#include <math.h>

#define ASCII_SIZE 255

using namespace std;

void error(const char *msg) {
    cout << "\x1b[31m[ERROR]\x1b[0m " << msg << endl;
    exit(EXIT_FAILURE);
}

void isFileOpen(FILE * file, const char *msg) {
    if(file != NULL) return;

    error(msg);
}

int padding(int32_t textSize, int coverage){
    if(textSize > coverage && textSize % coverage != 0) {
        return coverage - (textSize % coverage);
    } else if(textSize % coverage !=0) {
        return coverage - textSize;
    }
    return 0;
}

void readPlainText(char *fileName, unsigned char *&text, int32_t &textSize, int coverage) {
    FILE*  file= fopen(fileName,"r");
    isFileOpen(file, "An error occurred while opening the input plain file");

    fseek(file, 0, SEEK_END);
    textSize = ftell(file);
    int32_t i = textSize;
    int nSentries=padding(textSize, coverage);
    textSize += nSentries;

    text = (unsigned char*)malloc(textSize*sizeof(unsigned char));
    while(i < textSize) text[i++] =0;

    fseek(file, 0, SEEK_SET);
    fread(text, 1, textSize-nSentries, file);
    fclose(file);
}

void radixSort(uint32_t *uText, int32_t nTuples, uint32_t *tuples, uint32_t &sigma, int coverage){
    uint32_t *tupleIndexTemp = (uint32_t*) calloc(nTuples, sizeof(uint32_t));
    
    for(int i=0, j=0; i < nTuples; i++, j+=coverage)tuples[i] = j;

    sigma = (sigma < ASCII_SIZE + coverage) ? ASCII_SIZE +coverage : sigma+coverage;
    uint32_t *bucket =(uint32_t*) calloc(sigma, sizeof(uint32_t));

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

void createLexNames(uint32_t *uText, uint32_t *tuples, uint32_t *rank, int32_t &qtyRules, long int nTuples, int coverage) {
    int32_t name = 1;
    rank[tuples[0]/coverage] = name;

    for(int32_t i=1; i < nTuples; i++) {
        bool equal = true;
        for(int j=0; j < coverage; j++){
            if(uText[tuples[i-1]+j] != uText[tuples[i]+j]){
                equal = false;
                break;
            }
        }

        if(equal)rank[tuples[i]/coverage] = name;
        else rank[tuples[i]/coverage] = ++name;
    }

    qtyRules = name;
    #if DEBUG == 1
        printf("## Número de trincas %d, quantidade de trincas únicas: %d.\n", nTuples, qtyRules);
    #endif
}
