#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <math.h>
#include "radix.hpp"

using namespace std;

vector<unsigned char> grammarInfo;

template <typename T>
void print(T v[], int n){
    cout << *(v);
    for(int i=1; i < n ; i++) cout  << ", " << *(v+i);
     cout << endl;
} 

void grammar(char *fileIn, char *fileOut, char op) {
    int textSize;
    switch (op){
        case 'e': {
            cout << "\n\n>>>> Encode <<<<\n";
            unsigned char *plainText=readPlainText(fileIn, textSize);
            encode(plainText,textSize, fileOut, 0);
            int levels = grammarInfo.at(0);
            cout << "\tCompressed file information:\n" <<
                    "\t\tAmount of levels: " << levels <<
                    //"\n\t\tStart symbol size (including $): "<< grammarInfo.at(1) <<
                    endl;

            for(int i=levels-1,j=1; i >=0; i--,j++)
                printf("\t\tLevel: %d - amount of rules: %d.\n",i,grammarInfo.at(j));
            delete [] plainText;
            break;
        }
        case 'd': {
            cout << "\n\n>>>> Decode <<<<\n";
            unsigned char *text=readCompressedFile(fileIn, textSize);
            int levels = (int)text[0];
            cout << "\tCompressed file information:\n" <<
                    "\t\tNumber of symbols: " << textSize <<
                    "\n\t\tAmount of levels: " << levels << endl;
                    //"\n\t\tStart symbol size: " << text[1] << endl;
            decode(text, textSize, levels-1, levels, fileOut);
            delete [] text;
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

unsigned char* readPlainText(char *fileName, int &textSize) {
    unsigned char*text;
    FILE*  file= fopen(fileName,"r");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    textSize = ftell(file);
    size_t i = textSize;
    int nSentries=calculatesNumberOfSentries(textSize);
    textSize += nSentries;
    text = new unsigned char[textSize];

    while(i < textSize) text[i++] =0;
    fseek(file, 0, SEEK_SET);
    fread(text, 1, textSize-nSentries, file);
    fclose(file);
    return text;
}

unsigned char* readCompressedFile(char *fileName, int &textSize) {
    char unsigned *text;
    FILE*  file= fopen(fileName,"r");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    textSize = ftell(file)+1;
    text = new unsigned char[textSize];      
    fseek(file, 0, SEEK_SET);
    fread(text, 1, textSize, file);
    fclose(file);
    return text;
}

int calculatesNumberOfSentries(int textSize) {
    if(textSize %3 == 1) return 2;
    else if(textSize %3 ==2) return 1;
    return 0;
}

void encode(unsigned char *text, int textSize, char *fileName, int level){
    int triplesSize = ceil((double)textSize/3);
    unsigned int *rank = (unsigned int*) malloc(textSize * sizeof(unsigned));
    unsigned int * triples =  (unsigned int*) malloc(triplesSize * sizeof(unsigned));

    radixSort(text, triplesSize, triples);
    unsigned char qtyRules = createLexNames(text, triples, rank, triplesSize);
    grammarInfo.insert(grammarInfo.begin(), qtyRules);
    
    int redTextSize =calculatesNumberOfSentries(triplesSize) + triplesSize;
    unsigned char *redText = (unsigned char*) malloc((redTextSize) * sizeof(unsigned char)); 
    createReducedText(rank, redText, triplesSize, textSize, redTextSize);

    if(qtyRules < triplesSize)
        encode(redText, redTextSize, fileName, ++level);
    else {
        //cout << "\tEncoded Text: ";
        //for(int i=0; i < redTextSize; i++)printf("%d.",redText[i]);
        //cout << endl;
        grammarInfo.insert(grammarInfo.begin(), level+1);
        storeStartSymbol(fileName, redText, redTextSize);
    }
    
    storeRules(text, triples, rank, triplesSize, fileName);
    free(rank);
    free(triples);
    free(redText);
}

void decode(unsigned char *text, int textSize, int level, int qtyLevels, char *fileName){
    int startLevel = 1 + qtyLevels;
    int xsSize = text[1];
    unsigned char *symbol = (unsigned char*)malloc(xsSize * sizeof(unsigned char));
  
    for(int i=0, j=startLevel; i < xsSize; i++)
        symbol[i] = text[j++];
    
    int l=1;
    startLevel += text[l]; 
    while(level >= 0 && l <= qtyLevels) {
        decodeSymbol(text,symbol, xsSize, level, startLevel);
        //cout << "\nInicio das regras do  nível " << level << " é: " << startLevel  << endl;
        startLevel += (text[l]*3);
        l++;
        level--;
    }

    saveDecodedText(symbol,xsSize, fileName);

    //cout << "\n\tText: ";
    //for(int i=0; i < xsSize; i++)printf("%c",symbol[i]);
    //cout << endl;
    free(symbol);
}

void radixSort(unsigned char *text, int triplesSize, unsigned int *triples){
    unsigned int *triplesTemp = (unsigned int*) calloc(triplesSize, sizeof(unsigned int));

    for(int i=0, j=0; i < triplesSize; i++, j+=3)triples[i] = j;

    unsigned int *bucket = (unsigned int*) calloc(257, sizeof(unsigned int));
    for(int d= module-1; d >=0; d--) {
        for(int i=0; i < 256;i++)bucket[i]=0;
        //Porque não incrementar de 3 em 3? estamos calculando a ocorrência dos elementos contidos em triples, e triples já contém os sufixos iniciados em posições múltiplas de 3: 0,3,6,9,12 e assim por diante. 
        for(int i=0; i < triplesSize; i++) bucket[text[triples[i] + d]+1]++; 

        for(int i=1; i < 256; i++) bucket[i] += bucket[i-1];

        for(int i=0; i < triplesSize; i++) {
            int index = bucket[text[triples[i] + d]]++;
            triplesTemp[index] = triples[i];
        }

        for(int i=0; i < triplesSize; i++) triples[i] = triplesTemp[i];
    }
    free(bucket);
    free(triplesTemp);
}

int createLexNames(unsigned char *text, unsigned int *triples, unsigned int *rank, int triplesSize) {
    int i=0;
    int uniqueTriple = 1;
    rank[triples[i++]] = 1;

    for(; i < triplesSize; i++) {
        if(memcmp(&text[triples[i-1]], &text[triples[i]], module) == 0)
            rank[triples[i]] = rank[triples[i-1]];
        else {
            rank[triples[i]] = rank[triples[i-1]] + 1;
            uniqueTriple++;
        }
    }
    return uniqueTriple;
}

void  createReducedText(unsigned int *rank, unsigned char *redText, int triplesSize, int textSize, int redTextSize) {
    for(int i=0, j=0; j < textSize; i++, j+=3) 
        redText[i] = rank[j];

    while(triplesSize < redTextSize)
        redText[triplesSize++] = 0;
}

void storeStartSymbol(char *fileName, unsigned char *startSymbol, int size) {
    ofstream grammarFile;
    grammarFile.open(fileName, ios::out | ios::binary);
    if(!grammarFile.is_open())exit(EXIT_FAILURE);
    
    for(int i=0; i < grammarInfo.size(); i++)
        grammarFile << grammarInfo.at(i);

    grammarFile << startSymbol;
    grammarFile.close();
}

void storeRules(unsigned char *text, unsigned int *triples, unsigned int *rank, int triplesSize, char *fileName){
    int lastRank = 0;

    ofstream grammarFile;
    grammarFile.open(fileName, ios::app | ios::binary);
    if(!grammarFile.is_open())exit(EXIT_FAILURE);

    for(int i=0; i < triplesSize; i++) {
        if(rank[triples[i]] == lastRank)
            continue;
        lastRank = rank[triples[i]];
    
        grammarFile.write((char*)&text[triples[i]], module);
    }
    grammarFile.close();
}

void decodeSymbol(unsigned char* text,unsigned char *&symbol, int &xsSize, int l, int start) {
    unsigned char *symbolTemp = (unsigned char*) malloc(xsSize*3* sizeof(unsigned char*));
    int j = 0;

    for(int i=0; i < xsSize; i++) {
        int rule = symbol[i];
        if(rule==0)continue; 
        //cout << "\n---- Rules in the level: " << l << endl;
        //cout << "\nv" << rule << " -> ";
        for(int k=0; k < 3; k++){
            int rightHand = start + ((rule-1)*3) +k;
            if(text[rightHand] ==0)continue;
            symbolTemp[j++] = text[rightHand];
            //if(isalpha(symbolTemp[j-1]))printf("%c . ", symbolTemp[j-1]);
            //else printf("%d . ", symbolTemp[j-1]);
        }
    }

    free(symbol);

    xsSize = j;
    symbol = (unsigned char*) malloc(xsSize* sizeof(unsigned char*));
    for(int i=0; i < xsSize; i++) symbol[i] = symbolTemp[i];

    free(symbolTemp);
}

void saveDecodedText(unsigned char *text, int textSize, char *fileName) {
    ofstream outFile;
    outFile.open(fileName, ios::out);
    if(!outFile.is_open())exit(EXIT_FAILURE);
    outFile.write((char*)&text[0], textSize);
    outFile.close();
}
