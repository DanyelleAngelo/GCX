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

using namespace std;
using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

#define ASCII_SIZE 255
#define DEFAULT_LCP 3
#define GET_RULE_INDEX() (xs[i]-1)*coverage

void grammar(char *fileIn, char *fileOut, char *reportFile, char *queriesFile, char op, int coverage) {
    clock_t start, finish;
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
            i32* uText = (i32*)calloc(textSize,sizeof(i32));
            for(int i=0; i < textSize; i++)uText[i] = (i32)text[i];
            free(text);

            //starting process
            auto start = timer::now();
            i32 *tuples = (i32*) malloc((textSize) * sizeof(i32));
            compress(uText, tuples, textSize, strcat(fileOut,".dcx"), 0, levelCoverage, header, ASCII_SIZE);
            auto stop = timer::now();
            duration = (double)duration_cast<seconds>(stop - start).count();

            //printing compressed informations
            cout << "\n\n\x1b[32m>>>> Encode <<<<\x1b[0m\n";
            i32 levels = header.at(0);
            grammarInfo(header.data(), levels, levelCoverage.data());

            free(uText);
            free(tuples);
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
            decode(text, header, encodedSymbols, xsSize, leafLevelRules, levelCoverage);
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
    
    textSize += (nSentries==0) ? coverage : nSentries;
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
    header = (i32*)calloc(levels+1, sizeof(i32));
    levelCoverage = (i32*)calloc(levels+1, sizeof(int));
    header[0]=levels;
    fread(&header[1], sizeof(i32), levels, file);
    fread(&levelCoverage[0], sizeof(int), levels+1, file);
    //for(int i=0; i < levels+1; i++)levelCoverage[i] = 3;
    
    //get xs and internal nodes
    encodedSymbols = (uarray**)malloc(header[0]*sizeof(uarray));
    xsSize = header[1];
    for(i32 i=0; i < header[0]; i++) {
        if(i == 0) {
            encodedSymbols[0] = ua_alloc(xsSize, ceil(log2(xsSize))+1);
        } else {
            encodedSymbols[i] = ua_alloc(header[i]*levelCoverage[i], ceil(log2(header[i+1]))+1);
        }
        /*cada V[i] armazena até 64b (n*b=número total de bits).*/
        fread(encodedSymbols[i]->V, sizeof(u64), (encodedSymbols[i]->n * encodedSymbols[i]->b/64)+1, file);
    }

    //get leaf nodes
    i32 size = header[levels]*levelCoverage[levels];
    leafLevelRules = (unsigned char*)malloc(size*sizeof(unsigned char));
    fread(&leafLevelRules[0], sizeof(unsigned char), size, file);

    fclose(file);
}

void grammarInfo(i32 *header, int levels, int *levelCoverage) {
    cout << "\tCompressed file information:\n" <<
            "\n\t\tAmount of levels: " << levels << endl;

    for(int i=levels; i >0; i--){
        printf("\t\tLevel: %d - amount of rules: %u - size of rules %d.\n",i,header[i], levelCoverage[i]);
    }
}

void compress(i32 *text, i32 *tuples, i32 textSize, char *fileName, int level, vector<int> &levelCoverage, vector<i32> &header, i32 sigma){
    int x = getLcpMean(text, tuples, textSize, levelCoverage[0], sigma) +1;
    levelCoverage.insert(levelCoverage.begin()+1, x);

    i32 nTuples = textSize/x, qtyRules=0;
    //TODO: como definir o padding que deve ser aplicado no próximo nível, sem calcular o lcp médio (coverage) antes? Atualmente estamos passando o X enviado como parâmetro na url. 
    i32 reducedSize =  nTuples + padding(nTuples, levelCoverage[0]);
    i32 *rank = (i32*) calloc(reducedSize, sizeof(i32));
    uarray *encdIntRules = nullptr;
    unsigned char *leafRules = nullptr;

    radixSort(text, nTuples, tuples, sigma+x, x);
    createLexNames(text, tuples, rank, qtyRules, nTuples, x);
    header.insert(header.begin(), qtyRules);

    if(level !=0) {
        selectUniqueRules(text, encdIntRules, tuples, rank, nTuples, x, level, qtyRules, sigma);
    }
    else {
        selectUniqueRules(text, leafRules, tuples, rank, nTuples, x, level, qtyRules);
    }

    if(qtyRules < nTuples && checkCoverageConvergence(level, levelCoverage)){
        compress(rank, tuples, reducedSize, fileName, level+1, levelCoverage, header, qtyRules);
    }else {
        header.insert(header.begin(), level+1);
        storeStartSymbol(fileName, rank, header, levelCoverage);
    }

    storeRules(fileName, encdIntRules, leafRules, level, qtyRules*x);

    if(encdIntRules != nullptr) ua_free(encdIntRules);
    else if(leafRules != nullptr) free(leafRules);
    free(rank);
}

int getLcpMean(i32 *text, i32 *tuples, i32 textSize, int coverage, i32 sigma) {
    i32 nTuples = textSize/coverage, lcpMean =0, lcpMax =0;
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
    lcpMean = ceil(lcpMean/qtyRules);
    return (lcpMean > 1) ? lcpMean : DEFAULT_LCP;
}

int checkCoverageConvergence(int level, vector<int> levelCoverage) {
    if(level < 2) return 1;
    if(levelCoverage[1] == levelCoverage[2] && levelCoverage[1] == DEFAULT_LCP+1)return 0;
    return 1;
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

void storeStartSymbol(char *fileName, i32 *startSymbol, vector<i32> &header, vector<int> &levelCoverage) {
    uarray *encodedSymbol = ua_alloc(header[1], ceil(log2(header[1]))+1);
    for(int i=0, j=0; i < header.at(1);i++){
        if(startSymbol[i] ==0)break;
        ua_put(encodedSymbol, j++, startSymbol[i]);
    }

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

void decode(unsigned char *&text, i32 *header, uarray **encodedSymbols, i32 &xsSize, unsigned char *leafLevelRules, int *levelCoverage) {
    i32 *xs = (i32*)calloc(xsSize, sizeof(i32));
    for(int i=0; i < xsSize; i++)xs[i] = (i32)ua_get(encodedSymbols[0], i);

    //decode internal nodes
    for(i32 i=1, j=header[0]; i < header[0]; i++, j--) {
        decodeSymbol(encodedSymbols[i], xs, xsSize, levelCoverage[i]);
    }

    //decode last level
    int coverage = levelCoverage[header[0]];
    i32 plainTxtSize=xsSize*levelCoverage[header[0]], k=0;
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
            fwrite(&text[0], sizeof(char), i.second-i.first+1, fileOutput);
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
    int coverage;
    //Determines the interval in Xs that we need to decode
    i32 startNode = l/subtreeSize[0], endNode = r/subtreeSize[0], size;
    i32 xsSize = endNode - startNode + 1;
    l = l%subtreeSize[0], r = r - (startNode*subtreeSize[0]);

    //get xs
    for(int i=startNode, j=0; j < xsSize; i++)xs[j++] = (i32)ua_get(encodedSymbols[0], i);
    for(int j=1; j < levels; j++) {
        coverage = levelCoverage[j];
        int p=0;
        for(int i =0; i < xsSize; i++) {
            if(xs[i] == 0)break;
            i32 rule = GET_RULE_INDEX();
            for(int k=0; k < coverage && rule+k < encodedSymbols[j]->n; k++) {
                temp[p++] = ua_get(encodedSymbols[j], rule+k);
            }
        }
        xsSize = p;

        //Choose rules (nodes) that we will use in the next iteration
        startNode = l/subtreeSize[j];
        endNode = r/subtreeSize[j];
        size = endNode - startNode+1;

        //symbol to translate in the next level
        for(int m=0, k=startNode; m < size && k < xsSize; k++)xs[m++] = temp[k];

        l = l%subtreeSize[j];
        r -= startNode*subtreeSize[j];
    }

    coverage = levelCoverage[levels];
    int k= l % subtreeSize[levels-1];
    char ch;
    for(int i=0,j=0; i < size && j < txtSize; i++){
        i32 rule = GET_RULE_INDEX();
        for(; k < coverage; k++) {
            ch = leafLevelRules[rule+k];
            if(ch != 0)text[j++] = ch;
        }
        k=0;
    }
}