#include "compressor.hpp"
#include "compressor-types.hpp"
#include <iostream>
#include <vector>
#include <cstring>
#include <math.h>
#include <limits>
#include <fstream>
using namespace std;


template <typename T>
void print(T v[], int n){
    cout << *(v);
    for(int i=1; i < n ; i++) cout  << ", " << *(v+i);
     cout << endl;
} 

size_t maxSizeInBytes(int32_t s, int level){
    if(level == 0 || s <= numeric_limits<unsigned char>::max())return sizeof(unsigned char);
    if(s <= numeric_limits<unsigned short>::max())return sizeof(unsigned short);
    if(s <= numeric_limits<unsigned int>::max())return sizeof(unsigned int);
    return sizeof(unsigned long long int);
}

void grammar(char *fileIn, char *fileOut, char op, int ruleSize) {
    unsigned char *text;
    uint32_t *uText =nullptr;
    vector<uint32_t> grammarInfo;
    int32_t textSize;
    int module = ruleSize;
    switch (op){
        case 'e': {
            cout << "\n\n>>>> Encode (variable types) <<<<\n";
            readPlainText(fileIn, text, textSize, module);
            uText = (uint32_t*)malloc(textSize*sizeof(uint32_t));
            if(uText == NULL)exit(EXIT_FAILURE);
            for(int i=0; i < textSize; i++)uText[i] = (uint32_t)text[i];
            encode(text, uText,textSize, fileOut, 0, module, grammarInfo);
            int levels = grammarInfo.at(0);
            cout << "\tCompressed file information:\n" <<
                    "\t\tSize of Tuples: " << module <<
                    "\n\t\tAmount of levels: " << levels <<
                    "\n\t\tStart symbol size (including $): "<< grammarInfo.at(1) <<
                    endl;

            for(int i=grammarInfo.size()-1; i >0; i--){
                printf("\t\tLevel: %d - amount of rules: %u.\n",i,grammarInfo[i]);
            }
            free(uText);
            free(text);
            break;
        }
        case 'd': {
            unsigned char *rules0 = nullptr;
            cout << "\n\n>>>> Decode (variable types)<<<<\n";
            readCompressedFile(fileIn, uText, textSize, rules0, module, grammarInfo);
            uint32_t levels = grammarInfo.at(0);
            cout << "\tCompressed file information:\n" <<
                    "\t\tSize of Tuples: " << module <<
                    "\n\t\tAmount of levels: " << levels << endl;
            for(int i=grammarInfo.size()-1; i >0; i--){
                printf("\t\tLevel: %d - amount of rules: %u.\n",i,grammarInfo[i]);
            }
            decode(uText, textSize, levels-1, levels, fileOut, rules0, module, grammarInfo);
            free(rules0);
            free(uText);
            break;
        }
        default: {
            cout << "\n>>> Invalid option! <<< \n"
                 << "\tPlease one of the options below:\n"
                 << "\te - to compress the text;\n"
                 << "\td - to decompress the text.\n";
            break;
        }
    }
}

void readCompressedFile(char *fileName, uint32_t *&uText, int32_t &textSize, unsigned char* &rules0, int module, vector<uint32_t> &grammarInfo) {
    int nRulesLastLevel;
    FILE*  file= fopen(fileName,"rb");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    uint32_t levels;
    fread(&levels, sizeof(uint32_t), 1, file);
    grammarInfo.push_back(levels);
    uint32_t levels_temp[levels];
    fread(&levels_temp, sizeof(uint32_t), levels, file); 
    for(int i = 0; i < levels; i++){
        grammarInfo.push_back(levels_temp[i]);
        if(i==0)textSize = levels_temp[i] + (levels_temp[i]*module);
        else if(i < levels-1)textSize+= (levels_temp[i]*module);
    }

    nRulesLastLevel = grammarInfo[levels];
    rules0 = (unsigned char*)malloc(nRulesLastLevel*module*sizeof(unsigned char));

    fseek(file, 0, SEEK_END);
    //textSize = (((int)ftell(file) - (grammarInfo.size()*4) - (nRulesLastLevel*module)));
    uText = (uint32_t*)malloc(textSize*sizeof(uint32_t));

    fseek(file, grammarInfo.size()*sizeof(uint32_t), SEEK_SET);
    size_t sizeInBytes = maxSizeInBytes(grammarInfo[1], 1);
    int32_t i=0;
    if(sizeInBytes==1){
        unsigned char *temp = (unsigned char*)calloc(grammarInfo[1], sizeof(unsigned char));
        fread(temp, sizeof(unsigned char), grammarInfo[1], file);        
        for(; i < grammarInfo[1]; i++)uText[i] =(uint32_t)temp[i];
    }else if(sizeInBytes==2){
        unsigned short *temp = (unsigned short*)calloc(grammarInfo[1], sizeof(unsigned short));
        fread(temp, sizeof(unsigned short), grammarInfo[1], file);        
        for(; i < grammarInfo[1]; i++)uText[i] =(uint32_t)temp[i];
    }else if(sizeInBytes==4){
        unsigned *temp = (unsigned*)calloc(grammarInfo[1], sizeof(unsigned));
        fread(temp, sizeof(unsigned), grammarInfo[1], file);        
        for(; i < grammarInfo[1]; i++)uText[i] =(uint32_t)temp[i];
    }else{
        uint32_t *temp = (uint32_t*)calloc(grammarInfo[1], sizeof(uint32_t));
        fread(temp, sizeof(uint32_t), grammarInfo[1], file);        
        for(; i < grammarInfo[1]; i++)uText[i] =(uint32_t)temp[i];
    }

    for(int32_t l=1; i < textSize && l <= levels; l++){
        int32_t sizeLevel = grammarInfo[l]*module;
        size_t sizeInBytes = maxSizeInBytes(grammarInfo[l], l);
        if(sizeInBytes==1){
            unsigned char *temp = (unsigned char*)calloc(sizeLevel, sizeof(unsigned char));
            fread(temp, sizeof(unsigned char), sizeLevel, file);        
            for(int32_t k=0; k < sizeLevel; k++)uText[i++] =(unsigned char)temp[k];
        }
        else if(sizeInBytes==2){
            unsigned short *temp = (unsigned short*)calloc(sizeLevel, sizeof(unsigned short));
            fread(temp, sizeof(unsigned short), sizeLevel, file);        
            for(int32_t k=0; k < sizeLevel; k++)uText[i++] =(uint32_t)temp[k];
        }else if(sizeInBytes==4){
            unsigned *temp = (unsigned*)calloc(sizeLevel, sizeof(unsigned));
            fread(temp, sizeof(unsigned), sizeLevel, file);        
            for(int32_t k=0; k < sizeLevel; k++)uText[i++] =(uint32_t)temp[k];
        }else{
            unsigned long long *temp = (unsigned long long*)calloc(sizeLevel, sizeof(unsigned long long));
            fread(temp, sizeof(unsigned long long), sizeLevel, file);        
            for(int32_t k=0; k < sizeLevel; k++)uText[i++] =(uint32_t)temp[k];
        }
    }

    fread(rules0, sizeof(char), nRulesLastLevel*module, file);
    fclose(file);
}

void encode(unsigned char *text0, uint32_t *uText, int32_t textSize, char *fileName, int level, int module, vector<uint32_t> &grammarInfo){
    int32_t nTuples = textSize/module, qtyRules=0;
    int32_t reducedSize =  nTuples + numberOfSentries(nTuples, module);
    uint32_t *rank = (uint32_t*) calloc(reducedSize, sizeof(uint32_t));
    uint32_t *tuples = (uint32_t*) calloc(nTuples, sizeof(uint32_t));

    radixSort(uText, nTuples, tuples, level, module);
    createLexNames(uText, tuples, rank, nTuples, module, qtyRules);
    grammarInfo.insert(grammarInfo.begin(), qtyRules);

    if(qtyRules < nTuples){
        encode(text0, rank, reducedSize, fileName, level+1, module, grammarInfo);
    }
    else {
        grammarInfo.insert(grammarInfo.begin(), level+1);
        storeStartSymbol(fileName, rank, reducedSize, grammarInfo);
    }

    storeRules(text0, uText, tuples, rank, nTuples, fileName, module, level);
    free(rank);
    free(tuples);
}

void decode(uint32_t *uText, int32_t textSize, int level, int qtyLevels, char *fileName, unsigned char *rules0, int module, vector<uint32_t> &grammarInfo){
    int startLevel = 0;
    int32_t xsSize = grammarInfo.at(1);

    uint32_t *symbol = (uint32_t*)malloc(xsSize * sizeof(uint32_t));
    for(int i=0; i < xsSize; i++)
        symbol[i] = uText[i];
    int l=1;
    startLevel += xsSize; 
    while(level > 0 && l < qtyLevels) {
        decodeSymbol(uText, symbol, xsSize, level, startLevel, module);
        startLevel += (grammarInfo[l]*module);
        l++;
        level--;
    }
    saveDecodedText(symbol, xsSize, fileName, rules0, module);
    free(symbol);
}

void storeStartSymbol(char *fileName, uint32_t *startSymbol, int32_t sizeSymbol, vector<uint32_t> &grammarInfo) {
    FILE *file;
    file = fopen(fileName, "wb");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }
    fwrite(&grammarInfo[0], sizeof(uint32_t), grammarInfo.size(), file);

    size_t sizeInBytes = maxSizeInBytes(sizeSymbol, 1);
    for(int i=0; i < sizeSymbol;i++){
        if(startSymbol[i]!=0)fwrite(&startSymbol[i], sizeInBytes, 1, file);
    }

    fclose(file);
}

void storeRules(unsigned char *text0, uint32_t *uText, uint32_t *tuples, uint32_t *rank, int nTuples, char *fileName, int module, int level){
    uint32_t lastRank = 0;
    vector<int32_t> u;
    vector<char> t;

    FILE*  file= fopen(fileName,"ab");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }
    size_t sizeInBytes = maxSizeInBytes(nTuples, 1);

    for(int i=0; i < nTuples; i++) {
        if(rank[tuples[i]/module] == lastRank)
            continue;
        lastRank = rank[tuples[i]/module];
        if(level==0){
            fwrite(&text0[tuples[i]], sizeof(unsigned char), module, file);
        }else {
            fwrite(&uText[tuples[i]], sizeInBytes, module, file);
        }
    }

   fclose(file);
}

void decodeSymbol(uint32_t* uText, uint32_t *&symbol, int32_t &xsSize, int l, int start, int module) {
    uint32_t *symbolTemp = (uint32_t*) malloc(xsSize*module* sizeof(uint32_t*));
    int j = 0;
    for(int i=0; i < xsSize; i++) {
        uint32_t rule = symbol[i];
        if(rule==0)continue; 
        #if DEBUG_RULES
            if(l==9)printf("\nv%u -> ", rule);
        #endif
        int rightHand = start + ((rule-1)*module);
        for(int k=0; k < module; k++){
            if(uText[rightHand+k] ==0)continue;
            #if DEBUG_RULES
                if(l==9)printf("%u.", uText[rightHand+k]);
            #endif
            symbolTemp[j++] = uText[rightHand+k];
        }
    }
    free(symbol);

    xsSize = j;
    symbol = (uint32_t*) malloc(xsSize* sizeof(uint32_t*));
    for(int i=0; i < xsSize; i++) symbol[i] = symbolTemp[i];
    free(symbolTemp);
}

void saveDecodedText(uint32_t *symbol, int32_t symbolSize, char *fileName, unsigned char* rules0, int module) {
    FILE*  file= fopen(fileName,"w");
    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    int textSize = symbolSize*module;
    char *str = (char*)malloc(textSize*sizeof(char));
    for(int i=0,k=0; i < symbolSize; i++){
        int rightHand = (symbol[i]-1)*module;
        for(int j=0; j < module; j++){
            if(rules0[rightHand+j]==0){
                textSize--;
                continue;
            }
            str[k++] = rules0[rightHand+j];
        }
    }
    fwrite(&str[0], sizeof(char), textSize, file);
    free(str);
    fclose(file);
}
