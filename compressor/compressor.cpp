#include "utils.hpp"
#include "compressor.hpp"
#include "uarray.h"
#include "stack_count.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <math.h>
#include <chrono>
#include <fstream>

//HEADER: levels, |xs| , |rule_level_1|, |rule_level_2|, ..., |rule_level_n|

using namespace std;
using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

#define ASCII_SIZE 255
#define GET_RULE_INDEX() (xs[i]-1)*coverage

void grammar(char *fileIn, char *fileOut, char *reportFile, char *queriesFile, char op, int coverage) {
    double duration =0.0;
    i32 textSize;
    void* base = stack_count_clear();
    switch (op){
        case 'c': {
            vector<i32> header;
            vector<int> levelCoverage;
            levelCoverage.push_back(coverage);
            //preparing text
            unsigned char *text;
            readPlainText(fileIn, text, textSize, coverage);
            i32* uText = (i32*)calloc(textSize+coverage,sizeof(i32));
            for(int i=0; i < textSize; i++)uText[i] = (i32)text[i];
            free(text);

            //starting process
            auto start = timer::now();
            i32 *tuples = (i32*) malloc(textSize * sizeof(i32));
            compress(uText, tuples, textSize, strcat(fileOut,".dcx"), 0, levelCoverage, header, ASCII_SIZE);
            auto stop = timer::now();
            duration = (double)duration_cast<seconds>(stop - start).count();

            //printing compressed informations
            cout << "\n\n\x1b[32m>>>> Encode <<<<\x1b[0m\n";
            i32 levels = header.at(0);
            grammarInfo(header.data(), levels, levelCoverage.data());

            break;
        }
        case 'd': {
            unsigned char *leafLevelRules, *text;
            i32 *header = nullptr;
            int *levelCoverage = nullptr;
            uarray **encodedSymbols;

            //reading compressed text and rules per level
            i32 xsSize = 0;
            readCompressedFile(fileIn, header, encodedSymbols, xsSize, levelCoverage, leafLevelRules);

            //starting process
            auto start = timer::now();
            decode(text, header[0], encodedSymbols, xsSize, leafLevelRules, levelCoverage);
            auto stop = timer::now();
            duration = (double)duration_cast<seconds>(stop - start).count();
            //saving output
            saveDecodedText(fileOut, text, xsSize);

            //printing compressed informations
            cout << "\n\n\x1b[32m>>>> Decompression <<<<\x1b[0m\n";
            grammarInfo(header, header[0], levelCoverage);

            free(leafLevelRules);
            free(text);
            for(int i=0; i < header[0]; i++)ua_free(encodedSymbols[i]);
            free(encodedSymbols);
            free(header);
            free(levelCoverage);
            break;
        }
        case 'e': {
            unsigned char *leafLevelRules, *text;
            i32 *subtreeSize = nullptr, l, r, txtSize;
            int *levelCoverage = nullptr;
            uarray **encodedSymbols;
            int xsSize = 0, levels;
            vector<pair<i32, i32>> queries;

            //preparing data
            readCompressedFile(fileIn, subtreeSize, encodedSymbols, xsSize, levelCoverage, leafLevelRules);

            //printing compressed informations
            cout << "\n\n\x1b[32m>>>> Extract <<<<\x1b[0m\n";
            grammarInfo(subtreeSize, subtreeSize[0], levelCoverage);

            ifstream file(queriesFile);
            if (!file.is_open())error("Unable to open file with intervals");
            while (file >> l >> r) queries.push_back(make_pair(l, r));
            file.close();

            //start process
            levels=subtreeSize[0];
            for(int i=0; i < levels; i++){
                subtreeSize[i] = 1;
                for(int j=1+i; j < levels+1; j++) subtreeSize[i] *= levelCoverage[j];
            }

            txtSize = queries[0].second-queries[0].first+1;
            text = (unsigned char*)calloc(txtSize+1, sizeof(unsigned char));
            duration = extractBatch(fileOut, text, subtreeSize, encodedSymbols, leafLevelRules, levelCoverage, txtSize, queries, levels);

            free(leafLevelRules);
            free(text);
            free(subtreeSize);
            for(int i=0; i < levels; i++)ua_free(encodedSymbols[i]);
            free(encodedSymbols);
            free(levelCoverage);
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
    //generate reports
    #if REPORT == 1
        generateReport(reportFile, duration, base);
    #endif
    printf("Time: %5.15lf(s)\n",duration);
}

void readPlainText(char *fileName, unsigned char *&text, i32 &textSize, int coverage) {
    FILE*  file= fopen(fileName,"r");
    isFileOpen(file, "An error occurred while opening the input plain file");

    fseek(file, 0, SEEK_END);
    textSize = ftell(file);

    i32 i = textSize;
    int nSentries= padding(textSize, coverage);
    
    textSize += nSentries;
    text = (unsigned char*)malloc((textSize)*sizeof(unsigned char));
    while(i < textSize) text[i++] =0;

    fseek(file, 0, SEEK_SET);
    fread(text, 1, textSize-nSentries, file);
    fclose(file);
}

void readCompressedFile(char *fileName, i32 *&header, uarray **&encodedSymbols, i32 &xsSize, int *&levelCoverage, unsigned char *&leafLevelRules) {
    FILE*  file= fopen(fileName,"rb");
    isFileOpen(file, "An error occurred while opening the compressed file.");

    //get header
    i32 levels;
    fread(&levels, sizeof(i32), 1, file);
    header = (i32*)calloc(levels+2, sizeof(i32));//for number of levels and size initial symbol
    levelCoverage = (i32*)calloc(levels+1, sizeof(i32));
    header[0]=levels;
    fread(&header[1], sizeof(i32), levels+1, file);
    fread(&levelCoverage[0], sizeof(int), levels+1, file);
    //for(int i=0; i < levels+1; i++)levelCoverage[i] = 3;
    
    //get xs and internal nodes
    encodedSymbols = (uarray**)malloc(header[0]*sizeof(uarray));
    xsSize = header[1];
    for(i32 i=1, j=0; j < header[0]; i++, j++) {
        if(j == 0) {
            encodedSymbols[j] = ua_alloc(xsSize, ceil(log2(xsSize))+1);
        } else {
            encodedSymbols[j] = ua_alloc(header[i]*levelCoverage[j], ceil(log2(header[i+1]))+1);
        }
        /*cada V[i] armazena até 64b (n*b=número total de bits).*/
        fread(encodedSymbols[j]->V, sizeof(u64), (encodedSymbols[j]->n * encodedSymbols[j]->b/64)+1, file);
    }

    //get leaf nodes
    i32 size = header[levels+1]*levelCoverage[levels];
    leafLevelRules = (unsigned char*)malloc(size*sizeof(unsigned char));
    fread(&leafLevelRules[0], sizeof(unsigned char), size, file);

    fclose(file);
}

void grammarInfo(i32 *header, int levels, int *levelCoverage) {
    cout << "\tCompressed file information:\n" <<
            "\n\t\tAmount of levels: " << levels << endl;

    printf("\t\tSize initial partition: %d\n", levelCoverage[0]);
    printf("\t\tInitial symbol size: %d\n", header[1]);
    for(int i=levels+1, j=levels; i >1; i--, j--){
        printf("\t\tLevel: %d - amount of rules: %u - size of rules %d.\n",j,header[i], levelCoverage[j]);
    }
}

void compress(i32 *text, i32 *tuples, i32 textSize, char *fileName, int level, vector<int> &levelCoverage, vector<i32> &header, i32 sigma){
    int lcp_mean = getLcpMean(text, tuples, textSize, levelCoverage[level], sigma);
    int cover = (lcp_mean > 2) ? lcp_mean : levelCoverage[1];
    levelCoverage.insert(levelCoverage.begin()+1, cover);

    i32 nTuples = ceil((double)textSize/cover), qtyRules=0;
    i32 reducedSize =  nTuples + padding(nTuples, cover);
    uarray *encdIntRules = nullptr;
    unsigned char *leafRules = nullptr;

    radixSort(text, nTuples, tuples, sigma+cover, cover);
    
    i32 *rank = &tuples[nTuples];
    for(i32 i=0; i < reducedSize; i++)rank[i] = 0;
    createLexNames(text, tuples, rank, qtyRules, nTuples, cover);
    header.insert(header.begin(), qtyRules);

    if(level !=0) {
        selectUniqueRules(text, encdIntRules, tuples, rank, nTuples, cover, level, qtyRules, sigma);
    }
    else {
        selectUniqueRules(text, leafRules, tuples, rank, nTuples, cover, level, qtyRules);
        free(text);
    }

    if(qtyRules != nTuples && lcp_mean > 1){
        compress(rank, tuples, reducedSize, fileName, level+1, levelCoverage, header, qtyRules);
    }else {
        header.insert(header.begin(), level+1);
        header.insert(header.begin()+1, reducedSize);
        storeStartSymbol(fileName, rank, header, levelCoverage);
        free(tuples);
    }

    storeRules(fileName, encdIntRules, leafRules, level, qtyRules*cover);

    if(encdIntRules != nullptr) ua_free(encdIntRules);
    else if(leafRules != nullptr) free(leafRules);
}

int getLcpMean(i32 *text, i32 *tuples, i32 textSize, int coverage, i32 sigma) {
    i32 nTuples = textSize/coverage, lcpMean =0;
    int qtyRules=0;
    radixSort(text, nTuples, tuples, sigma+coverage, coverage);
    for(int i=1; i < nTuples; i++) {
        int lcp =0;
        for(int j=0; j < coverage; j++) {
            if(text[tuples[i]+j] != text[tuples[i-1]+j])break;
            lcp++;
        }
        if(lcp != coverage) {
            lcpMean += lcp;
            qtyRules++;
        }
    }
    return lcpMean/qtyRules;
}

void selectUniqueRules(i32 *text, unsigned char *&rules, i32 *tuples, i32 *rank, i32 nTuples, int coverage, int level, i32 qtyRules) {
    i32 lastRank = 0, n=0;
    rules = (unsigned char*)malloc(qtyRules*coverage*sizeof(unsigned char));

    for(i32 i=0; i < nTuples; i++) {
        if(rank[tuples[i]/coverage] == lastRank)continue;
        lastRank = rank[tuples[i]/coverage];

        for(int k=0; k < coverage; k++){
            rules[n++] = (unsigned char)text[tuples[i]+k];
        }
    }
}

void selectUniqueRules(i32 *text, uarray *&rules, i32 *tuples, i32 *rank, i32 nTuples, int coverage, int level, i32 qtyRules, i32 sigma){
    i32 lastRank = 0, n=0;
    rules = ua_alloc(qtyRules*coverage, ceil(log2(sigma))+1);

    for(i32 i=0; i < nTuples; i++) {
        if(rank[tuples[i]/coverage] == lastRank)continue;
        lastRank = rank[tuples[i]/coverage];

        for(int k=0; k < coverage; k++){
            ua_put(rules, n++, text[tuples[i]+k]);
        }
    }
    rules->n = n;
}

void storeStartSymbol(char *fileName, i32 *text, vector<i32> &header, vector<int> &levelCoverage) {
    uarray *encodedSymbol = ua_alloc(header[1], ceil(log2(header[1]))+1);
    i32 size=0;
    for(int i=0; i < header[1]; i++){
        if(text[i] ==0)break;
        ua_put(encodedSymbol, size++, text[i]);
    }
    encodedSymbol->n = size;
    header[1]=size;

    FILE* file = fopen(fileName,"wb");
    isFileOpen(file, "An error occurred while opening the file for store start symbol.");

    fwrite(&header[0], sizeof(i32), header.size(), file);
    fwrite(&levelCoverage[0], sizeof(i32), levelCoverage.size(), file);

    size_t numElements = (encodedSymbol->n * encodedSymbol->b/64) + 1;
    fwrite(encodedSymbol->V, sizeof(u64), numElements, file);
    ua_free(encodedSymbol);
    fclose(file);
}

void storeRules(char *fileName, uarray *encdIntRules, unsigned char *leafRules, int level, i32 size) {
    FILE*  file= fopen(fileName,"ab");
    isFileOpen(file, "An error occurred while opening the file for store rules.");

    if(level != 0 && encdIntRules != NULL){
        size_t numElements = (encdIntRules->n * encdIntRules->b/64) + 1;
        fwrite(encdIntRules->V, sizeof(u64), numElements, file);
    } else if(leafRules != NULL) {
        fwrite(&leafRules[0], sizeof(unsigned char), size, file);
    } else {
        error("Error while trying to store rules.");
    }

    fclose(file);
}

void decode(unsigned char *&text, int levels, uarray **encodedSymbols, i32 &xsSize, unsigned char *leafLevelRules, int *levelCoverage) {
    i32 *xs = (i32*)calloc(xsSize, sizeof(i32));
    for(int i=0; i < xsSize; i++)xs[i] = (i32)ua_get(encodedSymbols[0], i);

    //decode internal nodes
    for(i32 i=1; i < levels; i++) {
        decodeSymbol(encodedSymbols[i], xs, xsSize, levelCoverage[i]);
    }

    //decode last level
    int coverage = levelCoverage[levels];
    i32 plainTxtSize=xsSize*levelCoverage[levels], k=0;
    text = (unsigned char*)malloc(plainTxtSize*sizeof(unsigned char));
    for(int i=0; i < xsSize; i++){
        if(xs[i] == 0)continue;
        i32 rule = GET_RULE_INDEX();
        for(int j=0; j < coverage; j++) {
            char ch = leafLevelRules[rule+j];
            if(ch != 0)text[k++] = ch;
        }
    }
    xsSize = k;
}

void decodeSymbol(uarray *encodedSymbols, i32 *&xs, i32 &xsSize, int coverage) {
    i32 *temp = (i32*)malloc(xsSize*coverage*sizeof(i32));//tirar mover
    int j=0;
    for(int i =0; i < xsSize; i++) {
        if(xs[i] == 0)break;
        i32 rule=GET_RULE_INDEX();
        for(int k=0; k < coverage; k++) {
            temp[j++] = ua_get(encodedSymbols, rule+k);
        }
    }

    xsSize =j;
    free(xs);//mover
    xs = (i32*)malloc(xsSize*sizeof(i32));
    for(int i=0,j=0; i < xsSize; i++)xs[j++] = temp[i];
    free(temp);//mover
}

void saveDecodedText(char *fileName, unsigned char* text, i32 size) {
    FILE*  file= fopen(fileName,"w");
    isFileOpen(file, "An error occurred while trying to open the file to save the decoded text." );
    fwrite(&text[0], sizeof(unsigned char), size, file);
    fclose(file);
}

double extractBatch(char *fileName, unsigned char *&text, int *subtreeSize, uarray **encodedSymbols, unsigned char *leafLevelRules, int *levelCoverage, i32 txtSize, vector<pair<i32, i32>> queries, int levels) {
    i32 *temp = (i32*)malloc(50000*sizeof(i32));
    i32 *xs = (i32*)malloc(50000*sizeof(i32));

    duration<double> duration;
    auto first = timer::now();
    auto totalTime = timer::now();
    for(auto i : queries) {
        auto t0 = timer::now();
        extract(text, temp, xs, subtreeSize, encodedSymbols, leafLevelRules, levelCoverage, txtSize, i.first, i.second, levels);
        auto t1 = timer::now();
        totalTime += t1-t0;

        #if FILE_OUTPUT == 1
            FILE* fileOutput = fopen(fileName,"a");
            isFileOpen(fileOutput, "An error occurred while opening the compressed file.");
            fprintf(fileOutput, "[%d,%d]\n", i.first,i.second);
            fwrite(&text[0], sizeof(char), txtSize, fileOutput);
            fprintf(fileOutput, "\n");
            fclose(fileOutput);
        #endif
    }
    duration = totalTime - first;
    free(temp);
    free(xs);
    return duration.count();
}

void extract(unsigned char *&text, i32 *temp, i32 *xs, int *subtreeSize, uarray **encodedSymbols, unsigned char *leafLevelRules, int *levelCoverage, i32 txtSize, i32 l, i32 r, int levels){
    int coverage, k, end, p;
    //Determines the interval in Xs that we need to decode
    i32 startNode = l/subtreeSize[0], endNode = r/subtreeSize[0], size;
    i32 xsSize = endNode - startNode + 1;
    //get xs
    for(int i=startNode, j=0; i <= endNode; i++)xs[j++] = (i32)ua_get(encodedSymbols[0], i);
    
    l = l%subtreeSize[0], r = r%subtreeSize[0];
    for(int j=1; j < levels; j++) {
        coverage = levelCoverage[j];
        //trim interval
        if(subtreeSize[j] > l) startNode = 0;
        else {
            startNode = l/subtreeSize[j];
            l = l%subtreeSize[j];
        }
        if(subtreeSize[j] > r) endNode =0;
        else{
            endNode = r/subtreeSize[j];
            r = r%subtreeSize[j];
        }

        p=0;
        for(int i =0; i < xsSize; i++) {
            if(xs[i] == 0)break;
            i32 rule = GET_RULE_INDEX();
            k=0;
            end = coverage;
            if(i==0)k=startNode;
            else if(j==0) end = endNode;
            while(k < end) {
                temp[p++] = ua_get(encodedSymbols[j], rule+k);
                k++;
            }
        }

        xsSize = p;
        for(int i=0; i < xsSize; i++)xs[i] = temp[i];
    }

    coverage = levelCoverage[levels];
    startNode = l;
    endNode = r;
    char ch;
    for(int i=0, j=0; i < xsSize && j < txtSize; i++){
        if(xs[i] == 0)break;
        i32 rule = GET_RULE_INDEX();
        k=0;
        end = coverage;
        if(i==0)k=startNode;
        else if(j==0) end = endNode;
        while(k < end) {
            ch = leafLevelRules[rule+k];
            if(ch != 0)text[j++] = ch;
            k++;
        }
    }
}
