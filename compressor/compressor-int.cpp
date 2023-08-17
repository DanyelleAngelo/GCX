#include "compressor.hpp"
#include "compressor-int.hpp"
#include "uarray.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <math.h>

using namespace std;

void grammarInteger(char *fileIn, char *fileOut, char op, int ruleSize) {
    uint32_t *uText = nullptr;
    int32_t textSize;
    switch (op){
        case 'e': {
            vector<uint32_t> header;
            unsigned char *text;
            cout << "\n\n\x1b[32m>>>> Encode (plain int) <<<<\x1b[0m\n";

            readPlainText(fileIn, text, textSize, ruleSize);
            uText = (uint32_t*)calloc(textSize,sizeof(uint32_t));
            if(uText == NULL)exit(EXIT_FAILURE);
            for(int i=0; i < textSize; i++)uText[i] = (uint32_t)text[i];

            encode(text, uText,textSize, fileOut, 0, ruleSize, header, 0);

            uint32_t levels = header.at(0);
            grammarInfo(header.data(), levels, ruleSize);

            free(text);
            break;
        }
        case 'd': {
            uint32_t* header=nullptr;
            cout << "\n\n\x1b[32m>>>> Decode (plain int) <<<<\x1b[0m\n";

            decode(fileIn, fileOut, header,  ruleSize);
            uint32_t levels = header[0];
            grammarInfo(header, levels, ruleSize);
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

void grammarInfo(uint32_t *header, int levels, int mod) {
    cout << "\tCompressed file information:\n" <<
            "\t\tSize of Tuples: " << mod <<
            "\n\t\tAmount of levels: " << levels << endl;

    for(int i=levels; i >0; i--){
        printf("\t\tLevel: %d - amount of rules: %u.\n",i,header[i]);
    }
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
        encode(text0, rank, reducedSize, fileName, level+1, mod, header, qtyRules);
    }else {
        header.insert(header.begin(), level+1);
        storeStartSymbol(fileName, rank, header);
    }
    storeRules(text0, uText, tuples, rank, nTuples, fileName, mod, level, qtyRules, sigma);

    free(rank);
    free(tuples);
}

void decode(char *compressedFile, char *decompressedFile, uint32_t *&header, int mod) {
    FILE*  file= fopen(compressedFile,"rb");
    isFileOpen(file, "An error occurred while opening the compressed file.");

    uint32_t levels, nRules;
    fread(&levels, sizeof(uint32_t), 1, file);
    header = (uint32_t*)calloc(levels+1, sizeof(uint32_t));
    header[0]=levels;
    fread(&header[1], sizeof(uint32_t), levels, file);

    int32_t xsSize = header[1];
    uarray *xsEncoded = ua_alloc(xsSize, ceil(log2(xsSize)));
    fread(xsEncoded->V, sizeof(u64), (xsEncoded->n * xsEncoded->b/64)+1, file);/*cada V[i] armazena até 64b (n*b=número total de bits).*/
    uint32_t *xs = (uint32_t*)calloc(xsSize, sizeof(uint32_t));
    for(int i=0; i < xsSize; i++)xs[i] = (uint32_t)ua_get(xsEncoded, i);
    ua_free(xsEncoded);

    int32_t i=0;
    for(i=1; i < header[0]; i++) {
        nRules = header[i] * mod;
        uarray *rules = ua_alloc(nRules, ceil(log2(header[i+1])));
        fread(rules->V, sizeof(u64), (rules->n * rules->b/64)+1, file);

        decodeSymbol(rules, xs , xsSize, mod);
        ua_free(rules);
    }

    nRules = header[header[0]]*mod;
    unsigned char *rules = (unsigned char*)calloc(nRules, sizeof(unsigned char));
    fread(&rules[0], sizeof(unsigned char), nRules, file);

    fclose(file);

    saveDecodedText(decompressedFile, xs, xsSize, rules, mod);
    free(rules);
    free(xs);
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
    size_t numElements = (encodedSymbol->n * encodedSymbol->b/64) + 1;
    fwrite(encodedSymbol->V, sizeof(u64), numElements, file);
    fclose(file);
}

void storeRules(unsigned char *text0, uint32_t *uText, uint32_t *tuples, uint32_t *rank, int32_t nTuples, char *fileName, int mod, int level, int32_t qtyRules, int32_t sigma){
    FILE*  file= fopen(fileName,"ab"), *fileReport=nullptr;
    isFileOpen(file, "An error occurred while opening the file for store rules.");

    int32_t lastRank = 0, n=0;
    uarray *encodedSymbol = nullptr;
    if(level !=0)encodedSymbol = ua_alloc(qtyRules*mod, ceil(log2(sigma)));

    for(int32_t i=0; i < nTuples; i++) {
        if(rank[tuples[i]/mod] == lastRank)continue;
        lastRank = rank[tuples[i]/mod];

        if(level!=0) {
            for(int k=0; k < mod; k++)ua_put(encodedSymbol, n++, uText[tuples[i]+k]);
        } else {
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

void decodeSymbol(uarray *rules, uint32_t *&xs, int32_t &xsSize, int mod) {
    uint32_t *temp = (uint32_t*)calloc(xsSize*mod, sizeof(uint32_t));
    int32_t j=0;

    for(int i =0; i < xsSize; i++) {
        int32_t rule = (xs[i]-1)*mod;
        if(rule < 0)break;
        for(int k=0; k < mod; k++) {
            uint32_t ch = ua_get(rules, rule+k);
            if(ch == 0)break;
            temp[j++] = ch;
        }
    }

    xsSize = j;
    xs = (uint32_t*)calloc(xsSize, sizeof(uint32_t));
    for(int i=0; i < xsSize; i++)xs[i] = temp[i];
    free(temp);
}

void saveDecodedText(char *fileName, uint32_t *xs, uint32_t xsSize, unsigned char *rules, int mod) {
    FILE*  file= fopen(fileName,"w");
    isFileOpen(file, "An error occurred while trying to open the file to save the decoded text." );

    int32_t plainTxtSize = xsSize*mod;
    char *plainTxt = (char*)malloc(plainTxtSize*sizeof(char));

    for(int i=0, k=0; i < xsSize; i++){
        int32_t rule = (xs[i]-1)*mod;
        for(int j=0; j < mod; j++) {
            char ch = rules[rule+j];
            if(ch != 0)plainTxt[k++] = ch;
            else plainTxtSize--;
        }
    }

    fwrite(&plainTxt[0], sizeof(char), plainTxtSize, file);
    free(plainTxt);
    fclose(file);
}