#include "compressor.hpp"
#include <iostream>
#include <vector>
#include <cstring>
#include <math.h>
#include <sdsl/int_vector.hpp>
#include <sdsl/coder.hpp>

#define ASCII_SIZE 255

using namespace std;

void isFileOpen(FILE * file, const char *msg) {
    if(file != NULL) return;

    cout << "\x1b[31m[ERROR]\x1b[0m " << msg << endl;
    exit(EXIT_FAILURE);
}

int numberOfSentries(int32_t textSize, int mod){
    if(textSize > mod && textSize % mod != 0) {
        return mod - (textSize % mod);
    } else if(textSize % mod !=0) {
        return mod - textSize;
    }
    return 0;
}

void readPlainText(char *fileName, unsigned char *&text, int32_t &textSize, int mod) {
    FILE*  file= fopen(fileName,"r");
    isFileOpen(file, "An error occurred while opening the input plain file");

    fseek(file, 0, SEEK_END);
    textSize = ftell(file);
    int32_t i = textSize;
    int nSentries=numberOfSentries(textSize, mod);
    textSize += nSentries;

    text = (unsigned char*)malloc(textSize*sizeof(unsigned char));
    while(i < textSize) text[i++] =0;

    fseek(file, 0, SEEK_SET);
    fread(text, 1, textSize-nSentries, file);
    fclose(file);
}

void radixSort(uint32_t *uText, int32_t nTuples, uint32_t *tuples, uint32_t sigma, int mod){
    uint32_t *tupleIndexTemp = (uint32_t*) calloc(nTuples, sizeof(uint32_t));
    
    for(int i=0, j=0; i < nTuples; i++, j+=mod)tuples[i] = j;

    sigma = (sigma < ASCII_SIZE + mod) ? ASCII_SIZE +mod : sigma+mod;
    uint32_t *bucket =(uint32_t*) calloc(sigma, sizeof(uint32_t));


    for(int d= mod-1; d >=0; d--) {
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

void createLexNames(uint32_t *uText, uint32_t *tuples, uint32_t *rank, int32_t &qtyRules, long int nTuples, int mod) {
    int32_t name = 1;
    rank[tuples[0]/mod] = name;

    for(int32_t i=1; i < nTuples; i++) {
        bool equal = true;
        for(int j=0; j < mod; j++){
            if(uText[tuples[i-1]+j] != uText[tuples[i]+j]){
                equal = false;
                break;
            }
        }

        if(equal)rank[tuples[i]/mod] = name;
        else rank[tuples[i]/mod] = ++name;
    }

    qtyRules = name;
    #if DEBUG == 1
        printf("## Número de trincas %d, quantidade de trincas únicas: %d.\n", nTuples, qtyRules);
    #endif
}