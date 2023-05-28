#include "compressor.hpp"
#include <iostream>
#include <vector>
#include <cstring>
#include <math.h>
#include <sdsl/int_vector.hpp>
#include <sdsl/coder.hpp>

using namespace std;

void readPlainText(char *fileName, unsigned char *&text, long long int &textSize, int module) {
    FILE*  file= fopen(fileName,"r");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    textSize = ftell(file);
    long long int i = textSize;
    int nSentries=calculatesNumberOfSentries(textSize, module);
    textSize += nSentries;
    text = (unsigned char*)malloc(textSize*sizeof(unsigned char));
    while(i < textSize) text[i++] =0;
    fseek(file, 0, SEEK_SET);
    fread(text, 1, textSize-nSentries, file);
    fclose(file);
}


int calculatesNumberOfSentries(long long int textSize, int module) {
    if(textSize > module && textSize % module !=0) {
        return (ceil((double)textSize/module)*module) - textSize;
    }else if(textSize % module !=0){
        return module - textSize;
    }
    return 0;
}

void radixSort(uint32_t *uText, int tupleIndexSize, uint32_t *tupleIndex, long int level, int module){
    uint32_t *tupleIndexTemp = (uint32_t*) calloc(tupleIndexSize, sizeof(uint32_t));
    long int big=uText[0];

    for(int i=1; i < tupleIndexSize*module;i++)if(uText[i] > big)big=uText[i];
    for(int i=0, j=0; i < tupleIndexSize; i++, j+=module)tupleIndex[i] = j;
    //Tentar entender qual deve ser o tamanho do alfabeto, definir este tamanho como sendo o maior valor em ordem lexicográfica está dando erro, sendo preciso incrementar
    long int sigma = big+module;
    int *bucket =(int*) calloc(sigma, sizeof(int));

    for(int d= module-1; d >=0; d--) {
        for(int i=0; i < sigma;i++)bucket[i]=0;
        for(int i=0; i < tupleIndexSize; i++) bucket[uText[tupleIndex[i] + d]+1]++; 
        for(int i=1; i < sigma; i++) bucket[i] += bucket[i-1];

        for(int i=0; i < tupleIndexSize; i++) {
            int index = bucket[uText[tupleIndex[i] + d]]++;
            tupleIndexTemp[index] = tupleIndex[i];
        }
        for(int i=0; i < tupleIndexSize; i++) tupleIndex[i] = tupleIndexTemp[i];
    }

    free(bucket);
    free(tupleIndexTemp);
}

long int createLexNames(uint32_t *uText, uint32_t *tupleIndex, uint32_t *rank, long int tupleIndexSize, int module) {
    long int i=0;
    long int uniqueTriple = 1;
    rank[tupleIndex[i++]] = 1;
    for(; i < tupleIndexSize; i++) {
        bool equal = true;
        for(int j=0; j < module; j++)
            if(uText[tupleIndex[i-1]+j] != uText[tupleIndex[i]+j]){
                equal = false;
                break;
            }
        if(equal)rank[tupleIndex[i]] = rank[tupleIndex[i-1]];
        else {
            rank[tupleIndex[i]] = rank[tupleIndex[i-1]] + 1;
            uniqueTriple++;
        }
    }
   
    printf("Número de trincas = %ld, quantidade de trincas sem repetição: %ld, quantidade de trincas com repetição = %ld\n", tupleIndexSize, uniqueTriple, tupleIndexSize-uniqueTriple);
    return uniqueTriple;
}

void  createReducedText(uint32_t *rank, uint32_t *redText, long long int tupleIndexSize, long long int textSize, long long int redTextSize, int module) {
    for(int i=0, j=0; j < textSize; i++, j+=module) 
        redText[i] = rank[j];

    while(tupleIndexSize < redTextSize)
        redText[tupleIndexSize++] = 0;
}