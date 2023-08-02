#include "compressor.hpp"
#include "compressor-int.hpp"
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
            unsigned char *charRules = nullptr;
            cout << "\n\n\x1b[32m>>>> Decode (plain int) <<<<\x1b[0m\n";

            readCompressedFile(fileIn, uText, textSize, charRules, mod, header);

            uint32_t levels = header.at(0);
            grammarInfo(header, levels, mod);

            decode(uText, textSize, levels-1, levels, fileOut, charRules, mod, header);
            free(charRules);
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

    fseek(file, 0, SEEK_END);
    textSize = (ftell(file) - (header.size()*sizeof(int32_t)) - (nRulesLastLevel*mod))/sizeof(int32_t);
    uText = (uint32_t*)malloc(textSize*sizeof(uint32_t));
    fseek(file, header.size()*sizeof(uint32_t), SEEK_SET);
    fread(uText, sizeof(uint32_t), textSize, file);

    fread(charRules, sizeof(char), nRulesLastLevel*mod, file);
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
        encode(text0, rank, reducedSize, fileName, level+1, mod, header, qtyRules);
    }else {
        header.insert(header.begin(), level+1);
        storeStartSymbol(fileName, rank, header);
    }
    storeRules(text0, uText, tuples, rank, nTuples, fileName, mod, level);

    free(rank);
    free(tuples);
}

void decode(uint32_t *uText, int32_t textSize, int level, int qtyLevels, char *fileName, unsigned char *charRules, int mod, vector<uint32_t> &header){
    int32_t xsSize = header.at(1);
    uint32_t *xs = (uint32_t*)calloc(xsSize, sizeof(uint32_t));
    for(int i=0; i < xsSize; i++)xs[i] = uText[i];

    int32_t startRules = xsSize; 
    for(int i=1; level > 0 && i < qtyLevels; i++) {
        decodeSymbol(uText,xs, xsSize, level, startRules, mod);
        startRules += (header[i]*mod);
        level--;
    }
    saveDecodedText(xs, xsSize, fileName, charRules, mod);
    free(xs);
}

void storeStartSymbol(char *fileName, uint32_t *startSymbol, vector<uint32_t> &header) {
    FILE* file= fopen(fileName,"wb");
    isFileOpen(file, "An error occurred while opening the file for store start symbol.");

    fwrite(&header[0], sizeof(uint32_t), header.size(), file);
    for(int i=0; i < header.at(1);i++){
        if(startSymbol[i]==0)break;
        fwrite(&startSymbol[i], sizeof(uint32_t), 1, file);
    }
    fclose(file);
}

void storeRules(unsigned char *text0, uint32_t *uText, uint32_t *tuples, uint32_t *rank, int32_t nTuples, char *fileName, int mod, int level){
    FILE*  file= fopen(fileName,"ab"), *fileReport=nullptr;
    isFileOpen(file, "An error occurred while opening the file for store rules.");

    #if LEVEL_REPORT==1
        string reportLevel = "report/report-int-level-" + to_string(level);
        fileReport=fopen(reportLevel.c_str(), "w");
        isFileOpen(fileReport, "An error occurred while opening the file for report (store rules).");
    #endif

    int32_t lastRank = 0;
    for(int i=0; i < nTuples; i++) {
        if(rank[tuples[i]/mod] == lastRank)continue;
        lastRank = rank[tuples[i]/mod];

        if(level!=0){
            fwrite(&uText[tuples[i]], sizeof(uint32_t), mod, file);
            #if LEVEL_REPORT==1
                fwrite(&uText[tuples[i]], sizeof(uint32_t), mod, fileReport);
            #endif
        }
        else {
            fwrite(&text0[tuples[i]], sizeof(unsigned char), mod, file);
            #if LEVEL_REPORT==1
                fwrite(&text0[tuples[i]], sizeof(unsigned char), mod, fileReport);
            #endif
        }
    }
    fclose(file);

    if(fileReport != NULL)fclose(fileReport);
}

void decodeSymbol(uint32_t *uText, uint32_t *&xs, int32_t &xsSize, int level, int32_t start, int mod)
{
    uint32_t *xsTemp = (uint32_t*) malloc(xsSize*mod * sizeof(uint32_t));
    int32_t j = 0;

    for(int32_t i=0; i < xsSize; i++) {
        int32_t rule = xs[i];
        if(rule==0)break;

        int32_t rightHand = start + ((rule-1)*mod);
        for(int k=0; k < mod; k++){
            if(uText[rightHand+k] ==0)break;
            xsTemp[j++] = uText[rightHand+k];
        }
    }
    xsSize = j;

    free(xs);
    xs = (uint32_t*) malloc(xsSize* sizeof(uint32_t*));
    for(int i=0; i < xsSize; i++) xs[i] = xsTemp[i];
    free(xsTemp);
}

void saveDecodedText(uint32_t *xs, int32_t xsSize, char *fileName, unsigned char* charRules, int mod) {
    FILE*  file= fopen(fileName,"w");
    isFileOpen(file, "An error occurred while trying to open the file to save the decoded text." );

    int32_t textSize = xsSize*mod;
    char *decodedText = (char*)malloc(textSize*sizeof(char));

    for(int i=0,k=0; i < xsSize; i++){
        int32_t rightHand = (xs[i]-1)*mod;
        for(int j=0; j < mod; j++){
            char ch = charRules[rightHand+j];
            if(ch != 0)decodedText[k++] = ch;
            else textSize--;
        }
    }

    fwrite(&decodedText[0], sizeof(char), textSize, file);
    free(decodedText);
    fclose(file);
}