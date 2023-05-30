#include "compressor.hpp"
#include "compressor-elias.hpp"
#include <iostream>
#include <vector>
#include <cstring>
#include <math.h>
#include <sdsl/int_vector.hpp>
#include <sdsl/coder.hpp>
#include <string>

using namespace std;
using namespace sdsl;

vector<uint32_t> grammarInfo;
unsigned char *text;
FILE * report;
const char fileReport[] = "report-elias.txt";

template <typename T>
void print(T v[], int n){
    cout << *(v);
    for(int i=1; i < n ; i++) cout  << ", " << *(v+i);
     cout << endl;
} 

void grammar(char *fileIn, char *fileOut, char op, int ruleSize) {
    long long int textSize;
    int module = ruleSize;
    int levels=0;

    switch (op){
        case 'e': {
            cout << "\n\n>>>> Encode with Elias Gamma <<<<\n";

            readPlainText(fileIn, text, textSize, module);

            uint32_t *uText = (uint32_t*)malloc(textSize*sizeof(uint32_t));
            if(uText == NULL)
                exit(EXIT_FAILURE);
            for(int i=0; i < textSize; i++)uText[i] = (uint32_t)text[i];

            encode(uText,textSize, 0, module);

            levels = grammarInfo.at(0);
            cout << "\tCompressed file information:\n" <<
                    "\t\tAmount of levels: " << levels <<
                    "\n\t\tStart symbol size (including $): "<< grammarInfo.at(1) <<
                    endl;

            encodeTextWithEliasAndSave(fileOut, textSize, module);
            for(int i=levels; i >0; i--){
                printf("\t\tLevel: %d - amount of rules: %u.\n",i,grammarInfo[i]);
            }

            free(uText);
            free(text);
            break;
        }
        case 'd': {
            int_vector<32> decoded;

            cout << "\n\n>>>> Decode with Elias Gamma <<<<\n";
            readCompressedFile(fileIn, decoded, levels);

            cout << "\tCompressed file information:\n" <<
                    "\n\t\tAmount of levels: " << levels << endl;
            for(int i=levels; i >0; i--){
                printf("\t\tLevel: %d - amount of rules: %u.\n",i,decoded[i]);
            }
            decode(decoded, levels-1, levels, module, fileOut);
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

void readCompressedFile(char *fileName, int_vector<32> &decoded, int &levels) {
    coder::elias_gamma eg;
    int_vector<32> encoded;
    
    load_from_file(encoded, fileName);
    eg.decode(encoded, decoded);

    levels = decoded[0];
}

void encode(uint32_t *uText, long long int textSize, int level, int module){
    if(level ==0){
        report=fopen(fileReport, "w");
    }

    long long int tupleIndexSize = textSize/module;
    uint32_t *rank = (uint32_t*) calloc(textSize, sizeof(uint32_t));
    uint32_t *tupleIndex = (uint32_t*) calloc(tupleIndexSize, sizeof(uint32_t));

    radixSort(uText, tupleIndexSize, tupleIndex, level, module);
    long int qtyRules = createLexNames(uText, tupleIndex, rank, tupleIndexSize, module);
    grammarInfo.insert(grammarInfo.begin(), qtyRules);

    long long int redTextSize =  tupleIndexSize + calculatesNumberOfSentries(tupleIndexSize, module);
    uint32_t *redText = (uint32_t*) calloc(redTextSize, sizeof(uint32_t));
    createReducedText(rank, redText, tupleIndexSize, textSize, redTextSize, module);
    
    fprintf(report, "\n--- Nível %d:\n\tQuantidade de regras sem repetição: %lu;\n", level, qtyRules);
    fprintf(report, "\tTamanho do texto recebido neste nível: %lld;\n\tTamanho da string reduzida gerada: %lld;\n\tTexto de entrada do próximo nível (%d):\n", textSize, redTextSize, level+1);
    for(int i=0; i < redTextSize;i++)fprintf(report, "%u,",redText[i]);

    if(qtyRules < tupleIndexSize)
        encode(redText, redTextSize, level+1, module);
    else {
        grammarInfo.insert(grammarInfo.begin(), level+1);
        for(int i=0; i < redTextSize;i++)
            if(redText[i]!=0)grammarInfo.push_back(redText[i]);
    }
    
    uint32_t lastRank = 0;
    int_vector<32> v_rep;
    v_rep.resize(qtyRules*module);
    for(int i=0, k=0; i < tupleIndexSize; i++) {
        if(rank[tupleIndex[i]] == lastRank)
            continue;
        lastRank = rank[tupleIndex[i]];
        if(level==0){
            for(int j=0; j < module;j++){
                grammarInfo.push_back(text[tupleIndex[i]+j]);
                v_rep[k++]=text[tupleIndex[i]+j];
            }
        }else{
            for(int j=0; j< module;j++){
               grammarInfo.push_back(uText[tupleIndex[i]+j]);
                v_rep[k++]=text[tupleIndex[i]+j];
            }
        }
    }

    //report
    coder::elias_gamma eg;
    int_vector<32> encoded;
    string reportLevel = "report/report-level-" + to_string(level) + "-"+ to_string(module);
    eg.encode(v_rep, encoded);
    store_to_file(encoded, reportLevel);
    FILE *reportLevel2 = fopen(reportLevel.c_str(), "a");
    fprintf(reportLevel2, "\nNúmero de regras neste nível %llu.\nTamanho das regras codificadas %llu", v_rep.size(),encoded.size());


    free(redText);
    free(rank);
    free(tupleIndex);
    if(level==0 && report!=NULL)fclose(report);
}

void decode(int_vector<32> decoded, int level, int qtyLevels, int module, char *fileName){
    int startRules = qtyLevels + decoded[1]+1;

    long long int xsSize = decoded[1];
    uint32_t *symbol = (uint32_t*)malloc(xsSize * sizeof(uint32_t*));
    for(int i=qtyLevels+1, j=0; j < xsSize; i++,j++)symbol[j] = decoded[i];

    int l=1;
    while(level > 0 && l < qtyLevels) {
        decodeSymbol(decoded,symbol, xsSize, level, startRules, module);
        startRules += (decoded[l]*module);
        l++;
        level--;
    }

    saveDecodedText(decoded, symbol, xsSize, module, fileName);
    free(symbol);
}

void encodeTextWithEliasAndSave(char *fileName, uint32_t textSize, int module){
    coder::elias_gamma eg;
    int_vector<32> v;
    int_vector<32> encoded;
    
    v.resize(grammarInfo.size());
    for(int i=0; i < grammarInfo.size();i++)v[i] = grammarInfo[i];
    
    eg.encode(v, encoded);
    store_to_file(encoded, fileName);

    long int n=0;
    for(int i=1; i <=grammarInfo[0]; i++)n+=grammarInfo[i];
    report=fopen(fileReport, "a");
    fprintf(report, "\n\n## Tamanho do texto original: %u;\n## Número de elementos no array codificado com Elias Gamma: %llu;\n## Quantidade de informações na gramática sem codificação: %lu;\n", textSize, encoded.size(), grammarInfo.size());

    fprintf(report, "## Número total de regras na gramática: %lu;\n## Quantidade de símbolos necessários para representar todas as regras: %lu;\n## Tamanho do símbolo inicial: %u;\n## File name: %s",  n,(n*module), grammarInfo[1], fileName);
}

void decodeSymbol(int_vector<32> decoded, uint32_t *&symbol, long long int &xsSize, int level, int startRules, int module) {
    uint32_t *symbolTemp = (uint32_t*) malloc(xsSize*module* sizeof(uint32_t*));
    int j = 0;
    for(int i=0; i < xsSize; i++) {
        int rule = symbol[i];
        if(rule==0)continue; 
        int rightHand = startRules + ((rule-1)*module);
        for(int k=0; k < module; k++){
            if(decoded[rightHand+k] ==0)continue;
            symbolTemp[j++] = decoded[rightHand+k];
        }
    }

    free(symbol);

    xsSize = j;
    symbol = (uint32_t*) malloc(xsSize* sizeof(uint32_t*));
    for(int i=0; i < xsSize; i++) symbol[i] = symbolTemp[i];
    free(symbolTemp);
}

void saveDecodedText(int_vector<32> decoded, uint32_t *symbol, int xsSize, int module, char *fileName){
    FILE*  file= fopen(fileName,"w");
    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }
    
    int startLastLevel = decoded[0] + decoded[1]+1;
    for(int i=1; i < decoded[0]; i++) startLastLevel+= decoded[i]*module;

    int n = xsSize * module;
    char *text = (char*)malloc(n*sizeof(char));

    for(int i=0,k=0; i < xsSize; i++){
        int rightHand = startLastLevel + ((symbol[i]-1)*module);
        for(int j=0; j < module; j++){
            if(decoded[rightHand+j]==0){
                n--;
                continue;
            }
            text[k++] = decoded[rightHand+j];
        }
    }
    fwrite(&text[0], sizeof(char), n, file);
    free(text);
    fclose(file);
}
