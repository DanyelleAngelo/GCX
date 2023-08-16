#include "compressor.hpp"
#include "compressor-int.hpp"
#include "uarray.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <math.h>

using namespace std;

void grammarInteger(char *fileIn, char *fileOut, char op, int ruleSize) {
    vector<uint32_t> header;
    uint32_t *uText = nullptr;
    int32_t textSize;
    int mod = ruleSize;
    switch (op){
        case 'e': {
            unsigned char *text;
            cout << "\n\n\x1b[32m>>>> Encode (plain int) <<<<\x1b[0m\n";

            readPlainText(fileIn, text, textSize, mod);
            uText = (uint32_t*)calloc(textSize,sizeof(uint32_t));
            if(uText == NULL)exit(EXIT_FAILURE);
            for(int i=0; i < textSize; i++)uText[i] = (uint32_t)text[i];

            encode(text, uText,textSize, fileOut, 0, mod, header, 0);

            uint32_t levels = header.at(0);
            grammarInfo(header, levels, mod);

            free(text);
            break;
        }
        case 'd': {
            uint32_t* header2=nullptr;
            cout << "\n\n\x1b[32m>>>> Decode (plain int) <<<<\x1b[0m\n";

            decode(fileIn, fileOut, header2,  mod);
           // uint32_t levels = header.at(0);
            //grammarInfo(header, levels, mod);
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

    if(uText != NULL)free(uText);
}

void grammarInfo(vector<uint32_t> header, int levels, int mod) {
    cout << "\tCompressed file information:\n" <<
            "\t\tSize of Tuples: " << mod <<
            "\n\t\tAmount of levels: " << levels << endl;

    for(int i=levels; i >0; i--){
        printf("\t\tLevel: %d - amount of rules: %u.\n",i,header[i]);
    }
}

void readCompressedFile(char *fileName, uint32_t *&uText, int32_t &textSize, unsigned char* &charRules, int mod, vector<uint32_t> &header) {
    int nRulesLastLevel;
    FILE*  file= fopen(fileName,"rb");
    isFileOpen(file, "An error occurred while opening the compressed file.");

    uint32_t levels;
    fread(&levels, sizeof(uint32_t), 1, file);
    header.push_back(levels);

    for(int i = levels-1; i >=0; i--){
        uint32_t n;
        fread(&n, sizeof(uint32_t), 1, file); 
        header.push_back(n);
    }

    nRulesLastLevel = header[levels];
    charRules = (unsigned char*)malloc(nRulesLastLevel*mod*sizeof(unsigned char));
    fseek(file, -(nRulesLastLevel*mod), SEEK_END);
    fread(charRules, sizeof(char), nRulesLastLevel*mod, file);

    fseek(file, 0, SEEK_END);
    textSize = (ftell(file) - (header.size()*sizeof(int32_t)) - (nRulesLastLevel*mod))/sizeof(int32_t);
    uText = (uint32_t*)malloc(textSize*sizeof(uint32_t));
    fseek(file, header.size()*sizeof(uint32_t), SEEK_SET);
    fread(uText, sizeof(uint32_t), textSize, file);

    fclose(file);
}

void encode(unsigned char *text0, uint32_t *uText, int32_t textSize, char *fileName, int level, int mod, vector<uint32_t> &header, uint32_t sigma){
    int32_t nTuples = textSize/mod, qtyRules=0;
    int32_t reducedSize =  nTuples + numberOfSentries(nTuples, mod);
    uint32_t *rank = (uint32_t*) calloc(reducedSize, sizeof(uint32_t));
    uint32_t *tuples = (uint32_t*) calloc(nTuples, sizeof(uint32_t));

    radixSort(uText, nTuples, tuples, sigma, mod);
    createLexNames(uText, tuples, rank, qtyRules, nTuples, mod);
    header.insert(header.begin(), qtyRules);

    if(qtyRules < nTuples){
        encode(NULL, rank, reducedSize, fileName, level+1, mod, header, qtyRules);
    }else {
        header.insert(header.begin(), level+1);
        storeStartSymbol(fileName, rank, header);
    }
    storeRules(text0, uText, tuples, rank, nTuples, fileName, mod, level, qtyRules);

    free(rank);
    free(tuples);
}

void decode(char *compressedFile, char *decompressedFile, uint32_t *header, int mod) {
    FILE*  file= fopen(compressedFile,"rb");
    isFileOpen(file, "An error occurred while opening the compressed file.");
    
    uint32_t levels;
    fread(&levels, sizeof(uint32_t), 1, file);
    header = (uint32_t*)calloc(levels+1, sizeof(uint32_t));
    header[0]=levels;
    fread(&header[1], sizeof(uint32_t), levels, file);

    uint32_t *text;
    int32_t textSize = header[1], i;
    uarray *xs = ua_alloc(textSize, ceil(log2(textSize)));
    fread(xs->V, sizeof(u64), (xs->n * xs->b/64)+1, file);//cada elemento em V armazena até 64 bits (n*b indica o número total de bits).

    for(i=1; i < header[0]-1; i++) {
        uint32_t nRules = header[i+1] * mod;
        uarray *rules = ua_alloc(nRules, ceil(log2(nRules/mod)));
        fread(rules->V, sizeof(u64), (rules->n * rules->b/64)+1, file);
        
        decodeSymbol(xs, rules, text , textSize, mod);

        if(i==1) {
            ua_free(xs);
            xs = nullptr;
        }
        ua_free(rules);
    }

    int32_t txtPlainSize = header[i] * mod;
    unsigned char *rules = (unsigned char*)calloc(txtPlainSize, sizeof(unsigned char));
    fread(&rules[0], sizeof(unsigned char), txtPlainSize, file);
    print(rules, 10);
    saveDecodedText(decompressedFile, text, textSize, rules, header[i], mod);
    fclose(file);

}

void storeStartSymbol(char *fileName, uint32_t *startSymbol, vector<uint32_t> &header) {
    uarray *encodedSymbol = ua_alloc(header[1], ceil(log2(header[1])));
    for(int i=0, j=0; i < header.at(1);i++){
        if(startSymbol[i] ==0)break;
        ua_put(encodedSymbol, j++, startSymbol[i]);
    }

    FILE* file = fopen(fileName,"wb");
    isFileOpen(file, "An error occurred while opening the file for store start symbol.");
    fwrite(&header[0], sizeof(uint32_t), header.size(), file);
    //cada elemento em V armazena até 64 bits (n*b indica o número total de bits).
    size_t numElements = (encodedSymbol->n * encodedSymbol->b/64) + 1;
    fwrite(encodedSymbol->V, sizeof(u64), numElements, file);
    fclose(file);
}

void storeRules(unsigned char *text0, uint32_t *uText, uint32_t *tuples, uint32_t *rank, int32_t nTuples, char *fileName, int mod, int level, int32_t qtyRules){
    FILE*  file= fopen(fileName,"ab"), *fileReport=nullptr;
    isFileOpen(file, "An error occurred while opening the file for store rules.");

    int32_t lastRank = 0, n=0;
    uarray *encodedSymbol = nullptr;
    if(level !=0)encodedSymbol = ua_alloc(qtyRules*mod, ceil(log2(qtyRules)));

    for(int32_t i=0; i < nTuples; i++) {
        if(rank[tuples[i]/mod] == lastRank)continue;
        lastRank = rank[tuples[i]/mod];

        if(level!=0){
            for(int k=0; k < mod; k++)ua_put(encodedSymbol, n++, uText[tuples[i]+k]);
        }
        else {
            fwrite(&text0[tuples[i]], sizeof(unsigned char), mod, file);
        }
    }

    if(level != 0){
        encodedSymbol->n = n;
        size_t numElements = (encodedSymbol->n * encodedSymbol->b/64) + 1;
        fwrite(encodedSymbol->V, sizeof(u64), numElements, file);
    }

    fclose(file);
    if(fileReport != NULL)fclose(fileReport);
}

void decodeSymbol(uarray *xs, uarray *rules, uint32_t *&text, int32_t textSize, int mod) {
    uint32_t *temp = (uint32_t*)calloc(textSize*mod, sizeof(uint32_t));
    int32_t j=0;

    if(xs != nullptr) {
        for(int i=0; i < textSize; i++) {
            int32_t rule = ua_get(xs, i);
            if(rule == 0)break;
            for(int k=0; k < mod; k++) {
                uint32_t ch = ua_get(rules, (rule-1)*mod+k);
                if(ch == 0) break;
                temp[j++] = ch;
            }
        }
    } else {
        for(int i =0; i < textSize; i++) {
            int32_t rule = text[i];
            if(rule ==0)break;
            for(int k=0; k < mod; k++) {
                uint32_t ch = ua_get(rules, (rule-1)+k);
                if(ch == 0)break;
                temp[j++] = ch;
            }
        }
    }

    textSize = j;
    text = (uint32_t*)calloc(textSize, sizeof(uint32_t));
    for(int i=0; i < textSize; i++)text[i] = temp[i];
    free(temp);
}

void saveDecodedText(char *fileName, uint32_t *compressTxt, uint32_t compTxtSize, unsigned char *rules, uint32_t nRules, int mod) {
    FILE*  file= fopen(fileName,"w");
    isFileOpen(file, "An error occurred while trying to open the file to save the decoded text." );

    int32_t textSize = compTxtSize*mod;
    char *plainTxt = (char*)malloc(textSize*sizeof(char));
    //TODO limite
    int teste =0;
    for(int i=0,k=0; i < compTxtSize; i++){
        int32_t rule = compressTxt[i];
        for(int j=0; j < mod; j++){
            char ch = rules[rule-1+j];
            if(ch != 0)plainTxt[k++] = ch;
            else textSize--;
        }
    }
    fwrite(&plainTxt[0], sizeof(char), textSize, file);
    free(plainTxt);
    fclose(file);
}