#include "compressor.hpp"
#include "compressor-int.hpp"
#include "uarray.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <math.h>

using namespace std;

#define ASCII_SIZE 255
#define GET_RULE_INDEX() (xs[i]-1)*coverage

void grammarInteger(char *fileIn, char *fileOut, char op, i32 l, i32 r, int coverage) {
    i32 *uText = nullptr;
    i32 textSize;
    switch (op){
        case 'c': {
            vector<i32> header;
            unsigned char *text;
            #if (FILE_OUTPUT == 1) || (SCREEN_OUTPUT==1)
                cout << "\n\n\x1b[32m>>>> Encode <<<<\x1b[0m\n";
            #endif

            readPlainText(fileIn, text, textSize, coverage);
            uText = (i32*)calloc(textSize,sizeof(i32));
            if(uText == NULL)exit(EXIT_FAILURE);
            for(int i=0; i < textSize; i++)uText[i] = (i32)text[i];
            
            i32 nTuples = textSize/coverage;
            i32 *tuples = (i32*) malloc(nTuples * sizeof(i32));
            //i32 *rank = (i32*) calloc(nTuples+ padding(nTuples, coverage), sizeof(i32));
            compress(text, uText, tuples, textSize, strcat(fileOut, ".dcx"), 0, coverage, header, ASCII_SIZE);

            #if (FILE_OUTPUT == 1) || (SCREEN_OUTPUT==1)
                i32 levels = header.at(0);
                grammarInfo(header.data(), levels, coverage);
            #endif
            free(text);
            //free(rank);
            free(tuples);
            break;
        }
        case 'd': {
            i32* header=nullptr;
            #if (FILE_OUTPUT == 1) || (SCREEN_OUTPUT==1)
                cout << "\n\n\x1b[32m>>>> Decode <<<<\x1b[0m\n";
            #endif

            decode(fileIn, fileOut, header,  coverage);

            #if (FILE_OUTPUT == 1) || (SCREEN_OUTPUT==1)
                i32 levels = header[0];
                grammarInfo(header, levels, coverage);
            #endif
            break;
        }
        case 'e': {
            #if (FILE_OUTPUT == 1) || (SCREEN_OUTPUT==1)
                cout << "\n\n\x1b[32m>>>> Extract T[" << l <<"," << r << "]<<<<\x1b[0m\n";
            #endif

            extract(fileIn, fileOut, l, r, coverage);
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

void grammarInfo(i32 *header, int levels, int coverage) {
    cout << "\tCompressed file information:\n" <<
            "\t\tSize of Tuples: " << coverage <<
            "\n\t\tAmount of levels: " << levels << endl;

    for(int i=levels; i >0; i--){
        printf("\t\tLevel: %d - amount of rules: %u.\n",i,header[i]);
    }
}

void compress(unsigned char *text0, i32 *uText, i32 *tuples, i32 textSize, char *fileName, int level, int coverage, vector<i32> &header, i32 sigma){
    i32 nTuples = textSize/coverage, qtyRules=0;
    i32 reducedSize =  nTuples + padding(nTuples, coverage);
    i32 *rank = (i32*) calloc(reducedSize, sizeof(i32));

    uarray *encdIntRules = nullptr;
    unsigned char *leafRules = nullptr;

    sigma+=coverage;
    radixSort(uText, nTuples, tuples, sigma, coverage);
    createLexNames(uText, tuples, rank, qtyRules, nTuples, coverage);
    header.insert(header.begin(), qtyRules);

    if(level !=0)selectUniqueRules(uText, encdIntRules, tuples, rank, nTuples, coverage, level, qtyRules, sigma-coverage);
    else selectUniqueRules(text0, leafRules, tuples, rank, nTuples, coverage, level, qtyRules);

    if(qtyRules < nTuples){
        compress(text0, rank, tuples, reducedSize, fileName, level+1, coverage, header, qtyRules);
    }else {
        header.insert(header.begin(), level+1);
        storeStartSymbol(fileName, rank, header);
    }
    storeRules(fileName, encdIntRules, leafRules, level, qtyRules*coverage);

    if(encdIntRules != nullptr) ua_free(encdIntRules);
    else if(leafRules != nullptr) free(leafRules);
    free(rank);
}

void decode(char *compressedFile, char *decompressedFile, i32 *&header, int coverage) {
    FILE*  file= fopen(compressedFile,"rb");
    isFileOpen(file, "An error occurred while opening the compressed file.");

    i32 xsSize;
    uarray *xsEncoded = nullptr;

    getHeaderAndXs(file, header, xsEncoded, xsSize, coverage);
    
    i32 *xs = (i32*)calloc(xsSize, sizeof(i32));
    for(int i=0; i < xsSize; i++)xs[i] = (i32)ua_get(xsEncoded, i);
    ua_free(xsEncoded);

    for(i32 i=1; i < header[0]; i++) {
        decodeSymbol(file, header[i]*coverage, header[i+1], xs , xsSize, coverage);
    }

    unsigned char *rules = nullptr;
    getRulesInTheLastLevel(file, header[header[0]]*coverage, rules);
    saveDecodedText(decompressedFile, xs, xsSize, rules, coverage);
    free(rules);
    free(xs);
}

void extract(char *fileIn, char *fileOut, i32 l, i32 r, int coverage){
    FILE*  compressedFile = fopen(fileIn,"rb");
    isFileOpen(compressedFile, "An error occurred while opening the compressed file.");
    if(l > r) {
        error("The value of r must be greater than or equal to l.");
    }

    unsigned char *plainTxt = nullptr;
    i32 *header = nullptr, *xs=nullptr;
    uarray *xsEncoded = nullptr;
    i32 xsSize, n_nodes, start_node, end_node, txtSize = r-l+1;
    r++;
    getHeaderAndXs(compressedFile, header, xsEncoded, xsSize, coverage);

    //Determines the interval in Xs that we need to decode
    n_nodes = pow(coverage, header[0]);
    start_node = l/n_nodes;
    end_node = r/n_nodes;
    xsSize = end_node - start_node + 1;
    i32 l2 = l%n_nodes;
    i32 r2 = r - (start_node*n_nodes);

    xs = (i32*)calloc(xsSize, sizeof(i32));
    for(int i=start_node, j=0; j < xsSize; i++) {
        xs[j++] = (i32)ua_get(xsEncoded, i);
    }
    ua_free(xsEncoded);

    searchInterval(compressedFile, plainTxt, xs, header, xsSize, txtSize, l2, r2, coverage);
    
    #if SCREEN_OUTPUT == 1
        int msgSize = snprintf(NULL, 0, "The size of the substring[%d,%d] is: ",l,r);
        char msg[msgSize+1];
        snprintf(msg,msgSize+1, "The size of the substring[%d,%d] is: ",l,r);
        print(plainTxt, txtSize, msg);
    #endif

    #if FILE_OUTPUT == 1
        FILE*  fileOutput = fopen(fileOut,"a");
        isFileOpen(fileOutput, "An error occurred while opening the compressed file.");
        fprintf(fileOutput, "[%d,%d]\n", l,r);
        fwrite(&plainTxt[0], sizeof(char), txtSize, fileOutput);
        fprintf(fileOutput, "\n");
        free(plainTxt);
        fclose(fileOutput);
    #endif
}

void storeStartSymbol(char *fileName, i32 *startSymbol, vector<i32> &header) {
    uarray *encodedSymbol = ua_alloc(header[1], ceil(log2(header[1]))+1);
    for(int i=0, j=0; i < header.at(1);i++){
        if(startSymbol[i] ==0)break;
        ua_put(encodedSymbol, j++, startSymbol[i]);
    }

    FILE* file = fopen(fileName,"wb");
    isFileOpen(file, "An error occurred while opening the file for store start symbol.");
    fwrite(&header[0], sizeof(i32), header.size(), file);
    size_t numElements = (encodedSymbol->n * encodedSymbol->b/64) + 1;
    fwrite(encodedSymbol->V, sizeof(u64), numElements, file);
    ua_free(encodedSymbol);
    fclose(file);
}

void selectUniqueRules(unsigned char *text, unsigned char *&rules, i32 *tuples, i32 *rank, i32 nTuples, int coverage, int level, i32 qtyRules) {
    i32 lastRank = 0, n=0;
    rules = (unsigned char*)malloc(qtyRules*coverage*sizeof(unsigned char));

    for(i32 i=0; i < nTuples; i++) {
        if(rank[tuples[i]/coverage] == lastRank)continue;
        lastRank = rank[tuples[i]/coverage];

        for(int k=0; k < coverage; k++){
            rules[n++] = text[tuples[i]+k];
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

void getHeaderAndXs(FILE *compressedFile, i32 *&header, uarray *&xsEncoded, i32 &xsSize, int coverage) {
    i32 levels;
    fread(&levels, sizeof(i32), 1, compressedFile);

    header = (i32*)calloc(levels+1, sizeof(i32));
    header[0]=levels;
    fread(&header[1], sizeof(i32), levels, compressedFile);

    xsSize = header[1];
    xsEncoded = ua_alloc(xsSize, ceil(log2(xsSize))+1);
    fread(xsEncoded->V, sizeof(u64), (xsEncoded->n * xsEncoded->b/64)+1, compressedFile);/*cada V[i] armazena até 64b (n*b=número total de bits).*/
}

void decodeSymbol(FILE *compressedFile, i32 sizeRules, i32 sigma, i32 *&xs, i32 &xsSize, int coverage) {
    uarray *rules = ua_alloc(sizeRules, ceil(log2(sigma))+1);
    fread(rules->V, sizeof(u64), (rules->n * rules->b/64)+1, compressedFile);

    i32 *temp = (i32*)calloc(xsSize*coverage, sizeof(i32));
    i32 j=0;
    for(int i =0; i < xsSize; i++) {
        i32 rule = GET_RULE_INDEX();
        if(xs[i] == 0)break;
        for(int k=0; k < coverage; k++) {
            i32 ch = ua_get(rules, rule+k);
            temp[j++] = ch;
        }
    }
    ua_free(rules);

    xsSize = xsSize*coverage;
    xs = (i32*)calloc(xsSize, sizeof(i32));
    for(int i=0,j=0; i < xsSize; i++)xs[j++] = temp[i];
    free(temp);
}

void getRulesInTheLastLevel(FILE *file, i32 size, unsigned char *&rules) {
    rules = (unsigned char*)calloc(size, sizeof(unsigned char));
    fread(&rules[0], sizeof(unsigned char), size, file);
    fclose(file);
}

void saveDecodedText(char *fileName, i32 *xs, i32 xsSize, unsigned char *rules, int coverage) {
    FILE*  file= fopen(fileName,"w");
    isFileOpen(file, "An error occurred while trying to open the file to save the decoded text." );

    i32 plainTxtSize = xsSize*coverage;
    unsigned char *plainTxt = (unsigned char*)malloc(plainTxtSize*sizeof(unsigned char));

    i32 size=0;
    for(int i=0; i < xsSize; i++){
        i32 rule = GET_RULE_INDEX();
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

void searchInterval(FILE *compressedFile, unsigned char *&plainTxt, i32 *xs, i32 *header, i32 xsSize, i32 &txtSize, i32 l, i32 r, int coverage) {
    i32 n_nodes, start_node, end_node, size = xsSize;

    for(int i=1; i < header[0]; i++) {
        //reading rules that generate this actual text
        decodeSymbol(compressedFile, header[i]*coverage, header[i+1], xs , xsSize, coverage);
    
        //Choose rules (nodes) that we will use in the next iteration
        n_nodes = pow(coverage, header[0]-i);
        start_node = l/n_nodes;
        end_node = r/n_nodes;
        size = end_node - start_node+1;

        //symbol to translate in the next level
        i32 *temp = (i32*)calloc(size, sizeof(i32));
        for(int i=start_node, j=0; j < size && i < xsSize; i++) {
            temp[j++] = xs[i];
        }
        for(int j=0; j < size; j++)xs[j] = temp[j];
        free(temp);

        l = l%n_nodes;
        r -= start_node*n_nodes;
        r++;
    }
    
    unsigned char *rules = nullptr;
    getRulesInTheLastLevel(compressedFile, header[header[0]]*coverage, rules);
    //decode the last level
    plainTxt = (unsigned char*)calloc(txtSize+1, sizeof(unsigned char));
    int i,k;

    l= l % n_nodes;
    for(i=0,k=0; i < size && k < txtSize; i++){
        i32 rule = GET_RULE_INDEX();
        if(xs[i] == 0)continue;
        for(int j= (i == 0) ? l : 0; j < coverage && k < txtSize; j++) {
            char ch = rules[rule+j];
            if(ch != 0)plainTxt[k++] = ch;
        }
    }
    free(rules);
   txtSize = (k < txtSize) ? k : txtSize;
}