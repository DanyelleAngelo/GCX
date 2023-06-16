#include "compressor.hpp"
#include "compressor-int.hpp"
#include <iostream>
#include <vector>
#include <cstring>
#include <math.h>

using namespace std;


template <typename T>
void print(T v[], int n){
    cout << *(v);
    for(int i=1; i < n ; i++) cout  << ", " << *(v+i);
     cout << endl;
} 

void grammar(char *fileIn, char *fileOut, char op, int ruleSize) {
    vector<uint32_t> grammarInfo;
    unsigned char *text;
    int32_t textSize;
    int module = ruleSize;
    switch (op){
        case 'e': {
            cout << "\n\n>>>> Encode <<<<\n";
            readPlainText(fileIn, text, textSize, module);
            uint32_t *uText = (uint32_t*)malloc(textSize*sizeof(uint32_t));
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
            uint32_t *textC =nullptr;
            cout << "\n\n>>>> Decode <<<<\n";
            readCompressedFile(fileIn, textC, textSize, rules0, module, grammarInfo);
            uint32_t levels = grammarInfo.at(0);
            cout << "\tCompressed file information:\n" <<
                    "\t\tSize of Tuples: " << module <<
                    "\n\t\tAmount of levels: " << levels << endl;

            for(int i=grammarInfo.size()-1; i >0; i--){
                printf("\t\tLevel: %d - amount of rules: %u.\n",i,grammarInfo[i]);
            }
            decode(textC, textSize, levels-1, levels, fileOut, rules0, module, grammarInfo);
            free(rules0);
            free(textC);
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

void readCompressedFile(char *fileName, uint32_t *&textC, int32_t &textSize, unsigned char* &rules0, int module, vector<uint32_t> &grammarInfo) {
    int nRulesLastLevel;
    FILE*  file= fopen(fileName,"rb");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    uint32_t levels;
    fread(&levels, sizeof(uint32_t), 1, file);
    grammarInfo.push_back(levels);

    for(int i = levels-1; i >=0; i--){
        uint32_t n;
        fread(&n, sizeof(uint32_t), 1, file); 
        grammarInfo.push_back(n);
    }

    nRulesLastLevel = grammarInfo[levels];
    rules0 = (unsigned char*)malloc(nRulesLastLevel*module*sizeof(unsigned char));

    fseek(file, 0, SEEK_END);
    textSize = (((int)ftell(file) - (grammarInfo.size()*4) - (nRulesLastLevel*module))/4);
    textC = (uint32_t*)malloc(textSize*sizeof(uint32_t));
    fseek(file, grammarInfo.size()*sizeof(uint32_t), SEEK_SET);
    fread(textC, sizeof(uint32_t), textSize, file);

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

void decode(uint32_t *textC, int32_t textSize, int level, int qtyLevels, char *fileName, unsigned char *rules0, int module, vector<uint32_t> &grammarInfo){
    int startLevel = 0;
    int32_t xsSize = grammarInfo.at(1);

    uint32_t *symbol = (uint32_t*)malloc(xsSize * sizeof(uint32_t));
    for(int i=0; i < xsSize; i++)
        symbol[i] = textC[i];
    int l=1;
    startLevel += xsSize; 

    while(level > 0 && l < qtyLevels) {
        decodeSymbol(textC,symbol, xsSize, level, startLevel, module);

        startLevel += (grammarInfo[l]*module);
        l++;
        level--;
    }
    saveDecodedText(symbol, xsSize, fileName, rules0, module);
    free(symbol);
}

void storeStartSymbol(char *fileName, uint32_t *startSymbol, int size, vector<uint32_t> &grammarInfo) {
    FILE*  file= fopen(fileName,"wb");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }
    fwrite(&grammarInfo[0], sizeof(uint32_t), grammarInfo.size(), file);
    for(int i=0; i < size;i++){
        if(startSymbol[i]!=0)
            fwrite(&startSymbol[i], sizeof(uint32_t), 1, file);
    }
    fclose(file);
}

void storeRules(unsigned char *text0, uint32_t *uText, uint32_t *tuples, uint32_t *rank, int nTuples, char *fileName, int module, int level){
    uint32_t lastRank = 0;
    FILE*  file= fopen(fileName,"ab");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    for(int i=0; i < nTuples; i++) {
        if(rank[tuples[i]/module] == lastRank)
            continue;
        lastRank = rank[tuples[i]/module];
        
        if(level!=0)fwrite(&uText[tuples[i]], sizeof(uint32_t), module, file);
        else fwrite(&text0[tuples[i]], sizeof(char), module, file);
    }
    fclose(file);
}

void decodeSymbol(uint32_t* textC, uint32_t *&symbol, int32_t &xsSize, int l, int start, int module) {
    uint32_t *symbolTemp = (uint32_t*) malloc(xsSize*module* sizeof(uint32_t*));
    int j = 0;

    for(int i=0; i < xsSize; i++) {
        int rule = symbol[i];
        if(rule==0)continue;
        #if DEBUG_RULES
            if(l==9)printf("\nv%u -> ", rule);
        #endif
        int rightHand = start + ((rule-1)*module);
        for(int k=0; k < module; k++){
            if(textC[rightHand+k] ==0)continue;
            #if DEBUG_RULES
                if(l==9)printf("%u.", textC[rightHand+k]);
            #endif
            symbolTemp[j++] = textC[rightHand+k];
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
