#include "compressor-int.hpp"
#include <iostream>
#include <vector>
#include <cstring>
#include <math.h>
#include <sdsl/int_vector.hpp>

using namespace std;
using namespace sdsl;

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

void grammar(char *fileIn, char *fileOut, char op) {
    long long int textSize;
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
    rules0 = (unsigned char*)malloc(nRulesLastLevel*3*sizeof(unsigned char));

    fseek(file, 0, SEEK_END);
    textSize = (((int)ftell(file) - (grammarInfo.size()*4) - (nRulesLastLevel*module))/4);
    textC = (uint32_t*)malloc(textSize*sizeof(uint32_t));

    fseek(file, grammarInfo.size()*sizeof(uint32_t), SEEK_SET);
    fread(textC, sizeof(uint32_t), textSize, file);

    fread(rules0, sizeof(char), nRulesLastLevel*3, file);
    fclose(file);
}

int calculatesNumberOfSentries(long long int textSize) {
    if(textSize %3 == 1) return 2;
    else if(textSize %3 ==2) return 1;
    return 0;
}
void encode(uint32_t *uText, long long int textSize, char *fileName, int level){
    long long int triplesSize = ceil((double)textSize/3);
    uint32_t *rank = (uint32_t*) malloc(textSize * sizeof(uint32_t));
    uint32_t * triples =  (uint32_t*) malloc(triplesSize * sizeof(uint32_t));

    radixSort(uText, triplesSize, triples);
    long int qtyRules = createLexNames(uText, triples, rank, triplesSize);
    grammarInfo.insert(grammarInfo.begin(), qtyRules);

    long long int redTextSize =calculatesNumberOfSentries(triplesSize) + triplesSize;
    uint32_t *redText = (uint32_t*) malloc((redTextSize) * sizeof(uint32_t)); 
    createReducedText(rank, redText, triplesSize, textSize, redTextSize);

    if(qtyRules < triplesSize)
        encode(redText, redTextSize, fileName, level+1);
    else {
        //cout << "\tEncoded Text: ";
        //for(int i=0; i < redTextSize; i++)printf("%d.",redText[i]);
        //cout << endl;
        grammarInfo.insert(grammarInfo.begin(), level+1);
        storeStartSymbol(fileName, redText, redTextSize);
    }
    
    if(level!=0)storeRules(uText, triples, rank, triplesSize, fileName);
    else storeRules(text, triples, rank, triplesSize, fileName);

    free(rank);
    free(triples);
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
        startLevel += (grammarInfo[l]*3);
        //printf("Simbolo traduzido é: ");
        //print(symbol, xsSize);
        l++;
        level--;
    }

    saveDecodedText(symbol, xsSize, fileName);

    free(symbol);
}

void radixSort(uint32_t *uText, int triplesSize, uint32_t *triples){
    uint32_t *triplesTemp = (uint32_t*) calloc(triplesSize, sizeof(uint32_t));
    for(int i=0, j=0; i < triplesSize; i++, j+=3)triples[i] = j;
    long int n = triplesSize*3;
    int *bucket = ( int*) calloc(n, sizeof( int));
    for(int d= module-1; d >=0; d--) {
        for(int i=0; i < n;i++)bucket[i]=0;
        for(int i=0; i < triplesSize; i++) bucket[uText[triples[i] + d]+1]++; 
        for(int i=1; i < n; i++) bucket[i] += bucket[i-1];

        for(int i=0; i < triplesSize; i++) {
            int index = bucket[uText[triples[i] + d]]++;
            triplesTemp[index] = triples[i];
        }
        for(int i=0; i < triplesSize; i++) triples[i] = triplesTemp[i];
    }

    free(bucket);
    free(triplesTemp);
}

long int createLexNames(uint32_t *uText, uint32_t *triples, uint32_t *rank, long int triplesSize) {
    long int i=0;
    long int uniqueTriple = 1;
    rank[triples[i++]] = 1;
    for(; i < triplesSize; i++) {
        bool equal = true;
        for(int j=0; j < module; j++)
            if(uText[triples[i-1]+j] != uText[triples[i]+j]){
                equal = false;
                break;
            }
        if(equal)rank[triples[i]] = rank[triples[i-1]];
        else {
            rank[triples[i]] = rank[triples[i-1]] + 1;
            uniqueTriple++;
        }
    }
    printf("Número de trincas = %ld, quantidade de trincas sem repetição: %ld, quantidade de trincas com repetição = %ld\n", triplesSize, uniqueTriple, triplesSize-uniqueTriple);
    return uniqueTriple;
}

void  createReducedText(uint32_t *rank, uint32_t *redText, long long int triplesSize, long long int textSize, long long int redTextSize) {
    for(int i=0, j=0; j < textSize; i++, j+=3) 
        redText[i] = rank[j];

    while(triplesSize < redTextSize)
        redText[triplesSize++] = 0;
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

void storeRules(uint32_t *uText, uint32_t *triples, uint32_t *rank, int triplesSize, char *fileName){
    int lastRank = 0;

    FILE*  file= fopen(fileName,"ab");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    for(int i=0; i < triplesSize; i++) {
        if(rank[triples[i]] == lastRank)
            continue;
        lastRank = rank[triples[i]];
        fwrite(&uText[triples[i]], sizeof(uint32_t), module, file);
    }
    fclose(file);
}

void storeRules(unsigned char *text, uint32_t *triples, uint32_t *rank, int triplesSize, char *fileName){
    int lastRank = 0;
    
    FILE*  file= fopen(fileName,"ab");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    for(int i=0; i < triplesSize; i++) {
        if(rank[triples[i]] == lastRank)
            continue;
        lastRank = rank[triples[i]];
        fwrite(&text[triples[i]], sizeof(char), module, file);
    }
    fclose(file);
}

void decodeSymbol(uint32_t* textC, uint32_t *&symbol, long long int &xsSize, int l, int start) {
    uint32_t *symbolTemp = (uint32_t*) malloc(xsSize*3* sizeof(uint32_t*));
    int j = 0;
    for(int i=0; i < xsSize; i++) {
        int rule = symbol[i];
        if(rule==0)continue; 
        int rightHand = start + ((rule-1)*3);
        //cout << "\n---- Level: " << l << endl;
        //cout << "The rule " << rule << " starts at " << rightHand << endl;
        //cout << "\nv" << rule << " -> ";
        for(int k=0; k < 3; k++){
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

    int n = textSize*3;
    char *str = (char*)malloc(n*sizeof(char));
    for(int i=0,k=0; i < textSize; i++){
        int rightHand = (symbol[i]-1)*3;
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