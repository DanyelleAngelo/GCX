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

FILE * report;
template <typename T>
void print(T v[], int n){
    cout << *(v);
    for(int i=1; i < n ; i++) cout  << ", " << *(v+i);
     cout << endl;
} 

void grammar(char *fileIn, char *fileOut, char op, int ruleSize) {
    vector<uint32_t> grammarInfo;
    unsigned char *text;
    int32_t textSize;
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

            encode(text, uText,textSize, 0, module, grammarInfo);

            levels = grammarInfo.at(0);
            cout << "\tCompressed file information:\n" <<
                    "\t\tAmount of levels: " << levels <<
                    "\n\t\tStart symbol size (including $): "<< grammarInfo.at(1) <<
                    endl;

            encodeTextWithEliasAndSave(fileOut, textSize, module, grammarInfo);

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

void encode(unsigned char *text0, uint32_t *uText, int32_t textSize, int level, int module, vector<uint32_t> &grammarInfo){
    int32_t nTuples = textSize/module, qtyRules=0;
    int32_t reducedSize =  nTuples + numberOfSentries(nTuples, module);
    uint32_t *rank = (uint32_t*) calloc(reducedSize, sizeof(uint32_t));
    uint32_t *tuples = (uint32_t*) calloc(nTuples, sizeof(uint32_t));

    radixSort(uText, nTuples, tuples, level, module);
    createLexNames(uText, tuples, rank, nTuples, module, qtyRules);
    grammarInfo.insert(grammarInfo.begin(), qtyRules);

    #if REPORT==1
        string fileReport = "report/general-report-elias.txt";
        if(level==0)report=fopen(fileReport.c_str(), "w");
        fprintf(report, "\n--- Nível %d:\n\tQuantidade de regras sem repetição: %d;\n", level, qtyRules);
        fprintf(report, "\tTamanho do texto recebido neste nível: %d;\n\tTamanho da string reduzida gerada: %d;\n\tTexto de entrada do próximo nível (%d):\n", textSize, reducedSize, level+1);
       for(int i=0; i < reducedSize;i++)fprintf(report, "%u,",rank[i]);
    #endif


    if(qtyRules < nTuples)
        encode(text0, rank, reducedSize, level+1, module, grammarInfo);
    else {
        grammarInfo.insert(grammarInfo.begin(), level+1);
        for(int i=0; i < reducedSize;i++)grammarInfo.push_back(rank[i]);
    }
    
    createRules(text0, uText, tuples, rank, module, nTuples, level, grammarInfo);

    free(rank);
    free(tuples);
    #if REPORT==1
        if(report!=NULL && level ==0)fclose(report);
    #endif
}

void decode(int_vector<32> decoded, int level, int qtyLevels, int module, char *fileName){
    int startRules = qtyLevels + decoded[1]+1;

    int32_t xsSize = decoded[1];
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

void createRules(unsigned char *text0, uint32_t *uText, uint32_t *tuples, uint32_t *rank, int module,int32_t nTuples, int level, vector<uint32_t> &grammarInfo){
    uint32_t lastRank = 0;
    int_vector<32> v_rep;
    v_rep.resize(nTuples*module);
    for(int i=0, k=0; i < nTuples; i++) {
        if(rank[tuples[i]/module] == lastRank)
            continue;
        lastRank = rank[tuples[i]/module];
        if(level==0){
            for(int j=0; j < module;j++){
                grammarInfo.push_back(text0[tuples[i]+j]);
                v_rep[k++]=text0[tuples[i]+j];
            }
        }else{
            for(int j=0; j< module;j++){
               grammarInfo.push_back(uText[tuples[i]+j]);
                v_rep[k++]=text0[tuples[i]+j];
            }
        }
    }

    #if LEVEL_REPORT==1
        coder::elias_gamma eg;
        int_vector<32> encoded;
        string reportLevel = "report/report-level-" + to_string(level);
        eg.encode(v_rep, encoded);
        store_to_file(encoded, reportLevel);
        
        FILE *infoLevel= fopen(reportLevel.c_str(), "a");
        fprintf(infoLevel, "\nNúmero de regras neste nível %llu.\nTamanho das regras codificadas %llu", v_rep.size(),encoded.size());
        fclose(infoLevel);
    #endif
}

void encodeTextWithEliasAndSave(char *fileName, uint32_t textSize, int module, vector<uint32_t> &grammarInfo){
    coder::elias_gamma eg;
    int_vector<32> v;
    int_vector<32> encoded;
    
    v.resize(grammarInfo.size());
    for(int i=0; i < grammarInfo.size();i++)v[i] = grammarInfo[i];
    
    eg.encode(v, encoded);
    store_to_file(encoded, fileName);

    FILE *teste = fopen("teste.txt", "a");
    fwrite(&encoded[0], sizeof(int32_t), encoded.size(),teste);
    #if REPORT==1
        string fileReport = "report/general-report-elias.txt";
        report=fopen(fileReport.c_str(), "a");
        long int n=0;
        for(int i=1; i <=grammarInfo[0]; i++)n+=grammarInfo[i];
        fprintf(report, "\n\n## Tamanho do texto original: %d;\n## Número de elementos no array codificado com Elias Gamma: %llu;\n## Quantidade de informações na gramática sem codificação: %lu;\n", textSize, encoded.size(), grammarInfo.size());

        fprintf(report, "## Número total de regras na gramática: %lu;\n## Quantidade de símbolos necessários para representar todas as regras: %lu;\n## Tamanho do símbolo inicial: %u;\n## File name: %s",  n,(n*module), grammarInfo[1], fileName);
        fclose(report);
    #endif
}

void decodeSymbol(int_vector<32> decoded, uint32_t *&symbol, int32_t &xsSize, int level, int startRules, int module) {
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
