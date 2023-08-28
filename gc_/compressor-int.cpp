#include "compressor.hpp"
#include "compressor-int.hpp"
#include "uarray.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <math.h>

using namespace std;

#define GET_RULE_INDEX() (xs[i]-1)*coverage

void grammarInteger(char *fileIn, char *fileOut, char op, int32_t l, int32_t r, int ruleSize) {
    uint32_t *uText = nullptr;
    int32_t textSize;
    switch (op){
        case 'c': {
            vector<uint32_t> header;
            unsigned char *text;
            cout << "\n\n\x1b[32m>>>> Encode <<<<\x1b[0m\n";

            readPlainText(fileIn, text, textSize, ruleSize);
            uText = (uint32_t*)calloc(textSize,sizeof(uint32_t));
            if(uText == NULL)exit(EXIT_FAILURE);
            for(int i=0; i < textSize; i++)uText[i] = (uint32_t)text[i];
            compress(text, uText,textSize, strcat(fileOut, ".dcx"), 0, ruleSize, header, 0);

            uint32_t levels = header.at(0);
            grammarInfo(header.data(), levels, ruleSize);

            free(text);
            break;
        }
        case 'd': {
            uint32_t* header=nullptr;
            cout << "\n\n\x1b[32m>>>> Decode <<<<\x1b[0m\n";

            decode(fileIn, fileOut, header,  ruleSize);
            uint32_t levels = header[0];
            grammarInfo(header, levels, ruleSize);
            break;
        }
        case 'e': {
            cout << "\n\n\x1b[32m>>>> Extract T[" << l <<"," << r << "]<<<<\x1b[0m\n";
            extract(fileIn, fileOut, l, r, ruleSize);
            break;
        }
        default: {
            cout << "\n>>> Invalid option! <<< \n"
                 << "\tPlease one of the options below:\n"
                 << "\tc - to compress the text;\n"
                 << "\td - to decompress the text.\n"
                 << "\te - to extract substring[l,r] from the text.\n";
            break;
        }
    }

    if(uText != NULL)free(uText);
}

void grammarInfo(uint32_t *header, int levels, int coverage) {
    cout << "\tCompressed file information:\n" <<
            "\t\tSize of Tuples: " << coverage <<
            "\n\t\tAmount of levels: " << levels << endl;

    for(int i=levels; i >0; i--){
        printf("\t\tLevel: %d - amount of rules: %u.\n",i,header[i]);
    }
}

void compress(unsigned char *text0, uint32_t *uText, int32_t textSize, char *fileName, int level, int coverage, vector<uint32_t> &header, uint32_t sigma){
    int32_t nTuples = textSize/coverage, qtyRules=0;
    int32_t reducedSize =  nTuples + padding(nTuples, coverage);
    uint32_t *rank = (uint32_t*) calloc(reducedSize, sizeof(uint32_t));
    uint32_t *tuples = (uint32_t*) calloc(nTuples, sizeof(uint32_t));

    radixSort(uText, nTuples, tuples, sigma, coverage);
    createLexNames(uText, tuples, rank, qtyRules, nTuples, coverage);
    header.insert(header.begin(), qtyRules);
    if(qtyRules < nTuples){
        compress(text0, rank, reducedSize, fileName, level+1, coverage, header, qtyRules);
    }else {
        header.insert(header.begin(), level+1);
        storeStartSymbol(fileName, rank, header);
    }
    storeRules(text0, uText, tuples, rank, nTuples, fileName, coverage, level, header, qtyRules);

    free(rank);
    free(tuples);
}

void decode(char *compressedFile, char *decompressedFile, uint32_t *&header, int coverage) {
    FILE*  file= fopen(compressedFile,"rb");
    isFileOpen(file, "An error occurred while opening the compressed file.");

    int32_t xsSize;
    uint32_t chQty;
    uarray *xsEncoded = nullptr;

    getHeaderAndXs(file, header, xsEncoded, xsSize, coverage);
    
    uint32_t *xs = (uint32_t*)calloc(xsSize, sizeof(uint32_t));
    for(int i=0; i < xsSize; i++)xs[i] = (uint32_t)ua_get(xsEncoded, i);
    ua_free(xsEncoded);

    for(int32_t i=1; i < header[0]; i++) {
        decodeSymbol(file, header[i]*coverage, header[i+1], xs , xsSize, coverage);
    }

    unsigned char *rules = nullptr;
    getRulesInTheLastLevel(file, header[header[0]]*coverage, rules);
    saveDecodedText(decompressedFile, xs, xsSize, rules, coverage);
    free(rules);
    free(xs);
}

void extract(char *fileIn, char *fileOut, int32_t l, int32_t r, int coverage){
    FILE*  compressedFile = fopen(fileIn,"rb");
    isFileOpen(compressedFile, "An error occurred while opening the compressed file.");

    if(l > r) {
        error("The value of r must be greater than or equal to l.");
    }

    unsigned char *plainTxt = nullptr;
    uint32_t *header = nullptr, *xs=nullptr;
    uarray *xsEncoded = nullptr;
    int32_t xsSize, n_nodes, start_node, end_node, txtSize = r-l;

    getHeaderAndXs(compressedFile, header, xsEncoded, xsSize, coverage);

    //Determines the interval in Xs that we need to decode
    n_nodes = pow(coverage, header[0]);
    start_node = l/n_nodes;
    end_node = r/n_nodes;
    xsSize = end_node - start_node + 1;
    int32_t l2 = l%n_nodes;
    int32_t r2 = r;

    xs = (uint32_t*)calloc(xsSize, sizeof(uint32_t));
    for(int i=start_node, j=0; j < xsSize; i++) {
        xs[j++] = (uint32_t)ua_get(xsEncoded, i);
    }
    ua_free(xsEncoded);

    searchInterval(compressedFile, plainTxt, xs, header, xsSize, txtSize, l2, r2, coverage);
    
    #if D_EXTRACT == 1
        int msgSize = snprintf(NULL, 0, "The size of the substring[%d,%d] is: ",l,r);
        char msg[msgSize+1];
        snprintf(msg,msgSize+1, "The size of the substring[%d,%d] is: ",l,r);
        print(plainTxt, txtSize, msg);
    #endif

    FILE*  fileOutput = fopen(fileOut,"w");
    isFileOpen(fileOutput, "An error occurred while opening the compressed file.");
    fwrite(&plainTxt[0], sizeof(char), txtSize, fileOutput);
    free(plainTxt);
    fclose(fileOutput);
}

void storeStartSymbol(char *fileName, uint32_t *startSymbol, vector<uint32_t> &header) {
    uarray *encodedSymbol = ua_alloc(header[1], ceil(log2(header[1]))+1);
    for(int i=0, j=0; i < header.at(1);i++){
        if(startSymbol[i] ==0)break;
        ua_put(encodedSymbol, j++, startSymbol[i]);
    }

    FILE* file = fopen(fileName,"wb");
    isFileOpen(file, "An error occurred while opening the file for store start symbol.");
    fwrite(&header[0], sizeof(uint32_t), header.size(), file);
    size_t numElements = (encodedSymbol->n * encodedSymbol->b/64) + 1;
    fwrite(encodedSymbol->V, sizeof(u64), numElements, file);
    ua_free(encodedSymbol);
    fclose(file);
}

void storeRules(unsigned char *text0, uint32_t *uText, uint32_t *tuples, uint32_t *rank, int32_t nTuples, char *fileName, int coverage, int level,vector<uint32_t> header, int32_t qtyRules){
    FILE*  file= fopen(fileName,"ab"), *fileReport=nullptr;
    isFileOpen(file, "An error occurred while opening the file for store rules.");
    int32_t lastRank = 0, n=0;
    int32_t level_in_header = header[0]-level;
    uarray *encodedSymbol = nullptr;
    if(level !=0)encodedSymbol = ua_alloc(header[level_in_header]*coverage, ceil(log2(header[level_in_header+1]))+1);

    for(int32_t i=0; i < nTuples; i++) {
        if(rank[tuples[i]/coverage] == lastRank)continue;
        lastRank = rank[tuples[i]/coverage];

        if(level!=0) {
            for(int k=0; k < coverage; k++){
                ua_put(encodedSymbol, n++, uText[tuples[i]+k]);
            }
        } else {
            fwrite(&text0[tuples[i]], sizeof(unsigned char), coverage, file);
        }
    }

    if(level != 0){
        encodedSymbol->n = n;
        size_t numElements = (encodedSymbol->n * encodedSymbol->b/64) + 1;
        fwrite(encodedSymbol->V, sizeof(u64), numElements, file);
    }

    ua_free(encodedSymbol);
    fclose(file);
    if(fileReport != NULL)fclose(fileReport);
}

void getHeaderAndXs(FILE *compressedFile, uint32_t *&header, uarray *&xsEncoded, int32_t &xsSize, int coverage) {
    uint32_t levels;
    fread(&levels, sizeof(uint32_t), 1, compressedFile);

    header = (uint32_t*)calloc(levels+1, sizeof(uint32_t));
    header[0]=levels;
    fread(&header[1], sizeof(uint32_t), levels, compressedFile);

    xsSize = header[1];
    xsEncoded = ua_alloc(xsSize, ceil(log2(xsSize))+1);
    fread(xsEncoded->V, sizeof(u64), (xsEncoded->n * xsEncoded->b/64)+1, compressedFile);/*cada V[i] armazena até 64b (n*b=número total de bits).*/
}

void decodeSymbol(FILE *compressedFile, int32_t sizeRules, int32_t sigma, uint32_t *&xs, int32_t &xsSize, int coverage) {
    uarray *rules = ua_alloc(sizeRules, ceil(log2(sigma))+1);
    fread(rules->V, sizeof(u64), (rules->n * rules->b/64)+1, compressedFile);

    uint32_t *temp = (uint32_t*)calloc(xsSize*coverage, sizeof(uint32_t));
    int32_t j=0;
    for(int i =0; i < xsSize; i++) {
        int32_t rule = GET_RULE_INDEX();
        if(xs[i] == 0)break;
        for(int k=0; k < coverage; k++) {
            uint32_t ch = ua_get(rules, rule+k);
            temp[j++] = ch;
        }
    }
    ua_free(rules);

    xsSize = j;
    xs = (uint32_t*)calloc(xsSize, sizeof(uint32_t));
    for(int i=0,j=0; i < xsSize; i++)xs[j++] = temp[i];
    free(temp);
}

void getRulesInTheLastLevel(FILE *file, int32_t size, unsigned char *&rules) {
    rules = (unsigned char*)calloc(size, sizeof(unsigned char));
    fread(&rules[0], sizeof(unsigned char), size, file);
    fclose(file);
}

void saveDecodedText(char *fileName, uint32_t *xs, uint32_t xsSize, unsigned char *rules, int coverage) {
    FILE*  file= fopen(fileName,"w");
    isFileOpen(file, "An error occurred while trying to open the file to save the decoded text." );

    int32_t plainTxtSize = xsSize*coverage;
    unsigned char *plainTxt = (unsigned char*)malloc(plainTxtSize*sizeof(unsigned char));

    int32_t size=0;
    for(int i=0; i < xsSize; i++){
        int32_t rule = GET_RULE_INDEX();
        if(xs[i] == 0)continue;
        for(int j=0; j < coverage; j++) {
            char ch = rules[rule+j];
            if(ch != 0)plainTxt[size++] = ch;
        }
    }

    fwrite(&plainTxt[0], sizeof(unsigned char), size, file);
    free(plainTxt);
    fclose(file);
}

void searchInterval(FILE *compressedFile, unsigned char *&plainTxt, uint32_t *xs, uint32_t *header, int32_t xsSize, int32_t &txtSize, int32_t l, int32_t r, int coverage) {
    int32_t n_nodes, start_node, end_node, size = xsSize;

    for(int i=1; i < header[0]; i++) {
        //reading rules that generate this actual text
        decodeSymbol(compressedFile, header[i]*coverage, header[i+1], xs , xsSize, coverage);
    
        //Choose rules (nodes) that we will use in the next iteration
        n_nodes = pow(coverage, header[0]-i);
        start_node = l/n_nodes;
        end_node = r/n_nodes;
        size = end_node - start_node + 1;
        //update range of text for next level
        l = l%n_nodes;
        //r = (end_node == 0) ? r%n_nodes : r;
        xsSize = size;

        //updating Xs
        uint32_t *xsTemp = (uint32_t*)calloc(size, sizeof(uint32_t));
        for(int i=start_node, j=0; j < size; i++)xsTemp[j++] = xs[i];
        free(xs);
        xs = (uint32_t*) (uint32_t*)calloc(size, sizeof(uint32_t));
        for(int j=0; j < size; j++)xs[j] = xsTemp[j];
        free(xsTemp);
    }

    unsigned char *rules = nullptr;
    getRulesInTheLastLevel(compressedFile, header[header[0]]*coverage, rules);

    //decode the last level
    plainTxt = (unsigned char*)calloc(txtSize, sizeof(unsigned char));
    int i,k;
    for(i=0,k=0; i < size && k < txtSize; i++){
        int32_t rule = GET_RULE_INDEX();
        if(xs[i] == 0)continue;
        for(int j=l; j < coverage; j++) {
            char ch = rules[rule+j];
            if(ch != 0)plainTxt[k++] = ch;
        }
        l=0;
    }
   txtSize = (k < txtSize) ? k : txtSize;
}