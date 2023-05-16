#include "compressor-int.hpp"
#include <iostream>
#include <vector>
#include <cstring>
#include <math.h>
#include <sdsl/int_vector.hpp>

using namespace std;
using namespace sdsl;
int module;

unsigned char *text;
uint32_t *textC;
int nRulesLastLevel;
unsigned char *rules0;
vector<uint32_t> grammarInfo;

template <typename T>
void print(T v[], int n){
    cout << *(v);
    for(int i=1; i < n ; i++) cout  << ", " << *(v+i);
     cout << endl;
} 

void grammar(char *fileIn, char *fileOut, char op, int ruleSize) {
    long long int textSize;
    module = ruleSize;
    switch (op){
        case 'e': {
            cout << "\n\n>>>> Encode <<<<\n";
            readPlainText(fileIn, textSize);
            uint32_t *uText = (uint32_t*)malloc(textSize*sizeof(uint32_t));
            for(int i=0; i < textSize; i++)uText[i] = (uint32_t)text[i];
            encode(uText,textSize, fileOut, 0);

            int levels = grammarInfo.at(0);
            cout << "\tCompressed file information:\n" <<
                    "\t\tAmount of levels: " << levels <<
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
            cout << "\n\n>>>> Decode <<<<\n";
            readCompressedFile(fileIn, textSize);
            uint32_t levels = grammarInfo.at(0);
            cout << "\tCompressed file information:\n" <<
                    "\n\t\tAmount of levels: " << levels << endl;
            for(int i=grammarInfo.size()-1; i >0; i--){
                printf("\t\tLevel: %d - amount of rules: %u.\n",i,grammarInfo[i]);
            }
            decode(textC, textSize, levels-1, levels, fileOut);
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

void readPlainText(char *fileName, long long int &textSize) {
    FILE*  file= fopen(fileName,"r");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    textSize = ftell(file);
    long long int i = textSize;
    int nSentries=calculatesNumberOfSentries(textSize);
    textSize += nSentries;
    text = (unsigned char*)malloc(textSize*sizeof(unsigned char));
    while(i < textSize) text[i++] =0;
    fseek(file, 0, SEEK_SET);
    fread(text, 1, textSize-nSentries, file);
    fclose(file);
}

void readCompressedFile(char *fileName, long long int &textSize) {
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

int calculatesNumberOfSentries(long long int textSize) {
    if(textSize % module == 1) return 2;
    else if(textSize % module ==2) return 1;
    return 0;
}

void encode(uint32_t *uText, long long int textSize, char *fileName, int level){
    long long int rulesIndexSize = ceil((double)textSize/module);
    uint32_t *rank = (uint32_t*) malloc(textSize * sizeof(uint32_t));
    uint32_t * rulesIndex =  (uint32_t*) malloc(rulesIndexSize * sizeof(uint32_t));

    radixSort(uText, rulesIndexSize, rulesIndex);
    long int qtyRules = createLexNames(uText, rulesIndex, rank, rulesIndexSize);
    grammarInfo.insert(grammarInfo.begin(), qtyRules);

    long long int redTextSize =calculatesNumberOfSentries(rulesIndexSize) + rulesIndexSize;
    uint32_t *redText = (uint32_t*) malloc((redTextSize) * sizeof(uint32_t)); 
    createReducedText(rank, redText, rulesIndexSize, textSize, redTextSize);

    if(qtyRules < rulesIndexSize)
        encode(redText, redTextSize, fileName, level+1);
    else {
        //cout << "\tEncoded Text: ";
        //for(int i=0; i < redTextSize; i++)printf("%d.",redText[i]);
        //cout << endl;
        grammarInfo.insert(grammarInfo.begin(), level+1);
        storeStartSymbol(fileName, redText, redTextSize);
    }
    
    if(level!=0)storeRules(uText, rulesIndex, rank, rulesIndexSize, fileName);
    else storeRules(text, rulesIndex, rank, rulesIndexSize, fileName);

    free(rank);
    free(rulesIndex);
    free(redText);
}

void decode(uint32_t *textC, long long int textSize, int level, int qtyLevels, char *fileName){
    int startLevel = 0;
    long long int xsSize = grammarInfo.at(1);
    uint32_t *symbol = (uint32_t*)malloc(xsSize * sizeof(uint32_t));
    for(int i=0; i < xsSize; i++)
        symbol[i] = textC[i];
    int l=1;
    startLevel += xsSize; 
    while(level > 0 && l < qtyLevels) {
        //printf("Level: %d - amount of rules: %d.\n", level, grammarInfo.at(l));
        //printf("Inicio das regras do  nível %d é %d.\n", level, startLevel);
        decodeSymbol(textC,symbol, xsSize, level, startLevel);
        startLevel += (grammarInfo[l]*module);
        //printf("Simbolo traduzido é: ");
        //print(symbol, xsSize);
        l++;
        level--;
    }

    saveDecodedText(symbol, xsSize, fileName);

    free(symbol);
}

void radixSort(uint32_t *uText, int rulesIndexSize, uint32_t *rulesIndex){
    uint32_t *rulesIndexTemp = (uint32_t*) calloc(rulesIndexSize, sizeof(uint32_t));
    for(int i=0, j=0; i < rulesIndexSize; i++, j+=module)rulesIndex[i] = j;
    long int n = rulesIndexSize*module;
    int *bucket = ( int*) calloc(n, sizeof( int));
    for(int d= module-1; d >=0; d--) {
        for(int i=0; i < n;i++)bucket[i]=0;
        for(int i=0; i < rulesIndexSize; i++) bucket[uText[rulesIndex[i] + d]+1]++; 
        for(int i=1; i < n; i++) bucket[i] += bucket[i-1];

        for(int i=0; i < rulesIndexSize; i++) {
            int index = bucket[uText[rulesIndex[i] + d]]++;
            rulesIndexTemp[index] = rulesIndex[i];
        }
        for(int i=0; i < rulesIndexSize; i++) rulesIndex[i] = rulesIndexTemp[i];
    }

    free(bucket);
    free(rulesIndexTemp);
}

long int createLexNames(uint32_t *uText, uint32_t *rulesIndex, uint32_t *rank, long int rulesIndexSize) {
    long int i=0;
    long int uniqueTriple = 1;
    rank[rulesIndex[i++]] = 1;
    for(; i < rulesIndexSize; i++) {
        bool equal = true;
        for(int j=0; j < module; j++)
            if(uText[rulesIndex[i-1]+j] != uText[rulesIndex[i]+j]){
                equal = false;
                break;
            }
        if(equal)rank[rulesIndex[i]] = rank[rulesIndex[i-1]];
        else {
            rank[rulesIndex[i]] = rank[rulesIndex[i-1]] + 1;
            uniqueTriple++;
        }
    }
    printf("Número de trincas = %ld, quantidade de trincas sem repetição: %ld, quantidade de trincas com repetição = %ld\n", rulesIndexSize, uniqueTriple, rulesIndexSize-uniqueTriple);
    return uniqueTriple;
}

void  createReducedText(uint32_t *rank, uint32_t *redText, long long int rulesIndexSize, long long int textSize, long long int redTextSize) {
    for(int i=0, j=0; j < textSize; i++, j+=3) 
        redText[i] = rank[j];

    while(rulesIndexSize < redTextSize)
        redText[rulesIndexSize++] = 0;
}

void storeStartSymbol(char *fileName, uint32_t *startSymbol, int size) {
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

void storeRules(uint32_t *uText, uint32_t *rulesIndex, uint32_t *rank, int rulesIndexSize, char *fileName){
    int lastRank = 0;

    FILE*  file= fopen(fileName,"ab");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    for(int i=0; i < rulesIndexSize; i++) {
        if(rank[rulesIndex[i]] == lastRank)
            continue;
        lastRank = rank[rulesIndex[i]];
        fwrite(&uText[rulesIndex[i]], sizeof(uint32_t), module, file);
    }
    fclose(file);
}

void storeRules(unsigned char *text, uint32_t *rulesIndex, uint32_t *rank, int rulesIndexSize, char *fileName){
    int lastRank = 0;
    
    FILE*  file= fopen(fileName,"ab");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    for(int i=0; i < rulesIndexSize; i++) {
        if(rank[rulesIndex[i]] == lastRank)
            continue;
        lastRank = rank[rulesIndex[i]];
        fwrite(&text[rulesIndex[i]], sizeof(char), module, file);
    }
    fclose(file);
}

void decodeSymbol(uint32_t* textC, uint32_t *&symbol, long long int &xsSize, int l, int start) {
    uint32_t *symbolTemp = (uint32_t*) malloc(xsSize*module* sizeof(uint32_t*));
    int j = 0;
    for(int i=0; i < xsSize; i++) {
        int rule = symbol[i];
        if(rule==0)continue; 
        int rightHand = start + ((rule-1)*module);
        //cout << "\n---- Level: " << l << endl;
        //cout << "\nv" << rule << " -> ";
        for(int k=0; k < module; k++){
            if(textC[rightHand+k] ==0)continue;
            symbolTemp[j++] = textC[rightHand+k];
            //if(isalpha(symbolTemp[j-1]))printf("%c . ", symbolTemp[j-1]);
            //else printf("%d . ", symbolTemp[j-1]);
        }
    }

    free(symbol);

    xsSize = j;
    symbol = (uint32_t*) malloc(xsSize* sizeof(uint32_t*));
    for(int i=0; i < xsSize; i++) symbol[i] = symbolTemp[i];
    free(symbolTemp);
}

void saveDecodedText(uint32_t *symbol, long long int textSize, char *fileName) {
    FILE*  file= fopen(fileName,"w");
    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    int n = textSize*module;
    char *str = (char*)malloc(n*sizeof(char));
    for(int i=0,k=0; i < textSize; i++){
        int rightHand = (symbol[i]-1)*module;
        for(int j=0; j < module; j++){
            if(rules0[rightHand+j]==0){
                n--;
                continue;
            }
            str[k++] = rules0[rightHand+j];
        }
    }
    fwrite(&str[0], sizeof(char), n, file);
    free(str);
    fclose(file);
}
