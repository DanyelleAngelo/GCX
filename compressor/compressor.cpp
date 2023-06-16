#include "compressor.hpp"
#include <iostream>
#include <vector>
#include <cstring>
#include <math.h>
#include <sdsl/int_vector.hpp>
#include <sdsl/coder.hpp>

using namespace std;

int numberOfSentries(int32_t textSize, int module){
    if(textSize > module && textSize % module != 0) {
        return module - (textSize % module);
    } else if(textSize % module !=0) {
        return module - textSize;
    }
    return 0;
}

void readPlainText(char *fileName, unsigned char *&text, int32_t &textSize, int module) {
    FILE*  file= fopen(fileName,"r");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    textSize = ftell(file);
    int32_t i = textSize;
    int nSentries=numberOfSentries(textSize, module);
    textSize += nSentries;
    text = (unsigned char*)malloc(textSize*sizeof(unsigned char));
    while(i < textSize) text[i++] =0;
    fseek(file, 0, SEEK_SET);
    fread(text, 1, textSize-nSentries, file);
    fclose(file);
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

void createLexNames(uint32_t *uText, uint32_t *tupleIndex, uint32_t *rank, long int nTuples, int module, int32_t &qtyRules) {
    qtyRules= 1;
    rank[tupleIndex[0]/module] = 1;
    for(int32_t i=1; i < nTuples; i++) {
        bool equal = true;
        for(int j=0; j < module; j++)
            if(uText[tupleIndex[i-1]+j] != uText[tupleIndex[i]+j]){
                equal = false;
                break;
            }
        if(equal)rank[tupleIndex[i]/module] = rank[tupleIndex[i-1]/module];
        else {
            rank[tupleIndex[i]/module] = rank[tupleIndex[i-1]/module] + 1;
            qtyRules++;
        }
    }
    #if DEBUG == 1
        printf("## Número de trincas %d, quantidade de trincas únicas: %d.\n", nTuples, qtyRules);
    #endif
}