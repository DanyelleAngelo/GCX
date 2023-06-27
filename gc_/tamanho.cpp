#include <iostream>
#include <cstdint>
#include <vector>
#include <limits>
#include "compressor-variable-types.hpp"

using namespace std;
//#define DEBUG 1

template <typename T>
void print(T v[], int n){
    cout << *(v);
    for(int i=1; i < n ; i++) cout  << ", " << *(v+i);
     cout << endl;
} 

void grammar(char *fileIn, char *fileOut, char op, int ruleSize) {
    long long int textSize;
    int32_t module = ruleSize;
    vector<uint32_t> grammarInfo;

    switch (op){
        case 'e': {
            unsigned char *text;
            cout << "\n\n>>>> Encode <<<<\n";
            readPlainText(fileIn, text, textSize, module);
            uint32_t *uText = (uint32_t*)malloc(textSize*sizeof(uint32_t));
            if(uText == NULL)exit(EXIT_FAILURE);
            for(int i=0; i < textSize; i++)uText[i] = (uint32_t)text[i];
            encode(text, uText,textSize, fileOut, module,0,grammarInfo);

            int levels = grammarInfo.at(0);
            cout << "\tCompressed file information:\n" <<
                    "\t\tSize of Tuples: " << module <<
                   // "\n\t\tAmount of levels: " << levels <<
                    //"\n\t\tStart symbol size (including $): "<< grammarInfo.at(1) <<
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

void readPlainText(char *fileName, unsigned char *&text, long long int &textSize, int module) {
    FILE*  file= fopen(fileName,"r");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    textSize = ftell(file);
    long long int i = textSize;
    int nSentries=numberOfSentries(textSize, module);
    textSize += nSentries;
    text = (unsigned char*)malloc(textSize*sizeof(unsigned char));
    while(i < textSize) text[i++] =0;
    fseek(file, 0, SEEK_SET);
    fread(text, 1, textSize-nSentries, file);
    fclose(file);
}
int numberOfSentries(int32_t textSize, int module){
    if(textSize > module && textSize % module != 0) {
        return module - (textSize % module);
    } else if(textSize % module !=0) {
        return module - textSize;
    }
    return 0;
}

void encode(unsigned char *text0, uint32_t *text, int32_t textSize, char *fileOut, int module, int level, vector<uint32_t> &grammarInfo){
    int32_t nTuples = textSize/module, qtyRules=0;
    uint32_t reducedSize = nTuples + numberOfSentries(nTuples, module);
    uint32_t *textR = (uint32_t*) calloc(reducedSize, sizeof(uint32_t));
    uint32_t *tuples = (uint32_t*) calloc(nTuples, sizeof(uint32_t));

    radixSort(text, nTuples, tuples, module);
    createLexNames(text, textR, nTuples, tuples, level, module, qtyRules);
    grammarInfo.insert(grammarInfo.begin(), qtyRules);

    if(qtyRules < nTuples){
        encode(text0, textR, reducedSize, fileOut, module, level+1, grammarInfo);
    }else{
        grammarInfo.insert(grammarInfo.begin(), level+1);
        storeStartSymbol(fileOut, textR, reducedSize, grammarInfo);
    }

    storeRules(text0, text, tuples, textR, nTuples, fileOut,module, level);
    free(textR);
    free(tuples);
}

void getInfoFromCompressedText(char *fileIn, vector<uint32_t> &grammarInfo) {
    FILE*  file= fopen(fileIn,"rb");

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
}

void readCompressedFile(char *fileName, uint32_t *&textC, long long int &textSize, unsigned char* &rules0, int module, vector<uint32_t> grammarInfo) {
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
    int32_t size = grammarInfo[1];
    size_t s = maxSizeInBytes(grammarInfo[1], 1);
    int j=0;
    if(s==1){
        unsigned char *temp = (unsigned char*)calloc(size, sizeof(unsigned char));
        fread(temp, s, grammarInfo[1], file);
        for(; j < size;j++)textC[j]=(uint32_t)temp[j];
    }else if(s==2){
        unsigned short *temp = (unsigned short*)calloc(size, sizeof(unsigned short));
        fread(temp, s, size, file);
        for(; j < size;j++)textC[j]=(uint32_t)temp[j];
    }else if(s==4){
        unsigned *temp = (unsigned*)calloc(size, sizeof(unsigned));
        fread(temp, s, size, file);
        for(; j < size;j++)textC[j]=(uint32_t)temp[j];
    }else if(s==8){
        unsigned long *temp = (unsigned long*)calloc(size, sizeof(unsigned long));
        fread(temp, s, size, file);
        for(; j < size;j++)textC[j]=(uint32_t)temp[j];
    }

    for(int i=1; i < grammarInfo.size()-1; i++){
        int k=0;
        int a=j;
        int32_t size = grammarInfo[i]*module;
        size_t s = maxSizeInBytes(grammarInfo[i], i);
        if(s==1){
            cout << "level " << i << " = char\n";
            unsigned char *temp = (unsigned char*)calloc(size, sizeof(unsigned char));
           fread(temp, s, size, file);
           for(; j < size;j++)textC[j]=(uint32_t)temp[k++];
        }else if(s==2){

            cout << "level " << i << " = short\n";
        
            unsigned short *temp = (unsigned short*)calloc(size, sizeof(unsigned short));
            fread(temp, s, size, file);
            for(; j < size;j++)textC[j]=(uint32_t)temp[k++];
        }else if(s==4){
            cout << "level " << i << " = int\n";

            unsigned *temp = (unsigned*)calloc(size, sizeof(unsigned));
            fread(temp, s, size, file);
            for(; j < size;j++)textC[j]=(uint32_t)temp[k++];
        }else if(s==8){
            cout << "level " << i << " = long \n";

            unsigned long *temp = (unsigned long*)calloc(size, sizeof(unsigned long));
            fread(temp, s, size, file);
            for(; j < size;j++)textC[j]=(uint32_t)temp[k++];
        }
    }
    cout << "TEsteee \n";
    fread(rules0, sizeof(unsigned char), nRulesLastLevel*module, file);
    print(rules0, nRulesLastLevel*module);
    for(int i=0; i < nRulesLastLevel*module; i++)printf("%c,",rules0[i]);
    char *teste = (char*)calloc(10, sizeof(char));
    fread(teste, sizeof(char), 10,file);
    fclose(file);
}

void decode(uint32_t *textC, long long int textSize, int level, int qtyLevels, char *fileName, unsigned char *rules0, int module, vector<uint32_t> grammarInfo){
    int startLevel = 0;
    long long int xsSize = grammarInfo.at(1);
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

void radixSort(uint32_t *text, int32_t nTuples, uint32_t *tuples, int module){
    uint32_t *tuplesTemp = (uint32_t*) calloc(nTuples, sizeof(uint32_t));

    for(int32_t i=0, j=0; i < nTuples; j+=module)tuples[i++]=j;
    
    long int big=text[0];
    for(int i=1; i < nTuples*module;i++)if(text[i] > big)big=text[i];
    int32_t sigma = 255+big;

    for(int d = module -1; d >=0; d--){
        int32_t *bucket = (int32_t*) calloc(sigma, sizeof(int32_t));

        for(int32_t i=0; i < nTuples; i++)
            bucket[text[tuples[i]+d]+1]++;
        for(int32_t i=1; i < sigma; i++)
            bucket[i] +=bucket[i-1];

        for(int32_t i=0; i < nTuples; i++) {
            int32_t index = bucket[text[tuples[i]+d]]++;
            tuplesTemp[index] = tuples[i];
        }
        for(int32_t i=0; i < nTuples; i++)tuples[i]=tuplesTemp[i];
        free(bucket);
    }
    free(tuplesTemp);
}

void createLexNames(uint32_t *text, uint32_t *names, int32_t nTuples, uint32_t *tuples, int level, int module, int32_t &qtyRules){
    int32_t i=0;
    names[tuples[i++]/module] = 1;
    qtyRules =1;

    for(; i < nTuples; i++) {
        bool equals = true;
        for(int j=0; j < module; j++) {
            if(text[tuples[i-1]+j] != text[tuples[i]+j]){
                equals= false;
                break;
            }
        }
        if(equals){
            names[tuples[i]/module] = names[tuples[i-1]/module];
        } else {
            names[tuples[i]/module] = names[tuples[i-1]/module] +1;
            qtyRules++;
        }
    }

    #if DEBUG == 1
        printf("## Número de trincas %d, quantidade de trincas únicas: %d.\n", nTuples, qtyRules);
    #endif
}

void storeStartSymbol(char *fileOut, uint32_t *textR, int32_t reducedSize, vector<uint32_t> grammarInfo) {
    FILE *file = fopen(fileOut, "wb");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    fwrite(&grammarInfo[0], sizeof(uint32_t), grammarInfo.size(), file);
    int32_t startSymbolSize = grammarInfo[1];

    size_t size = maxSizeInBytes(startSymbolSize, -1);

    for(int i=0; i < reducedSize;i++){
        if(textR[i]!=0)fwrite(&textR[i], size, 1, file);
    }
}

void storeRules(unsigned char *text0, uint32_t *text, uint32_t *tuples, uint32_t *textR, int32_t nTuples, char *fileOut,int32_t module, int level){
   int32_t lastRank=0;
   int32_t sigma = nTuples*module;

    FILE*  file= fopen(fileOut,"ab");

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    size_t size = maxSizeInBytes(sigma, level);

    for(int32_t i=0; i < nTuples;i++){
        if(textR[tuples[i]/module] == lastRank)continue;
        lastRank = textR[tuples[i]/module];
        if(level!=0)fwrite(&text[tuples[i]/module], size, module, file);
        else fwrite(&text0[tuples[i]/module], sizeof(char), module, file);
    }
}

size_t maxSizeInBytes(int32_t s, int level){
    if(level == 0 || s <= numeric_limits<unsigned char>::max())return sizeof(unsigned char);
    if(s <= numeric_limits<unsigned short int>::max())return sizeof(unsigned short int);
    if(s <= numeric_limits<unsigned int>::max())return sizeof(unsigned int);
    if(s <= numeric_limits<unsigned long int>::max())return sizeof(unsigned long int);
    return sizeof(unsigned long long int);
}

void decodeSymbol(uint32_t* textC, uint32_t *&symbol, long long int &xsSize, int l, int start, int module) {
    uint32_t *symbolTemp = (uint32_t*) malloc(xsSize*module* sizeof(uint32_t*));
    int j = 0;
printf("XSSize= %d\n", xsSize);
if(l==9){
    for(int i= start; i < (xsSize*module); i++){
        cout << textC[i] << ",";
    }
}
    for(int i=0; i < xsSize; i++) {
        int rule = symbol[i];
        if(rule==0)continue; 
        int rightHand = start + ((rule-1)*module);
        for(int k=0; k < module; k++){
            if(textC[rightHand+k] ==0)continue;
            symbolTemp[j++] = textC[rightHand+k];
        }
    }

    free(symbol);

    xsSize = j;
    symbol = (uint32_t*) malloc(xsSize* sizeof(uint32_t*));
    for(int i=0; i < xsSize; i++) symbol[i] = symbolTemp[i];
if(l==9)print(symbol, xsSize);
    free(symbolTemp);
}

void saveDecodedText(uint32_t *symbol, long long int symbolSize, char *fileName, unsigned char* rules0, int module) {
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
