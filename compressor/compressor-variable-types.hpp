#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <iostream>
#include <cstdint>
#include <vector>

using namespace std;
void grammar(char *fileIn, char *fileOut, char op, int ruleSize);
void readPlainText(char *fileName, unsigned char *&text, int32_t &textSize, int module);
int numberOfSentries(int32_t textSize, int module);
void getInfoFromCompressedText(char *fileIn, vector<uint32_t> &grammarInfo);
void decode(uint32_t *uText, int32_t textSize, int level, int qtyLevels, char *fileName, unsigned char *rules0, int module, vector<uint32_t> &grammarInfo);
void decodeSymbol(uint32_t* uText, uint32_t *&symbol, int32_t &xsSize, int l, int start, int module);
void radixSort(uint32_t *text, int32_t nTuples, uint32_t *tuples, int module);
void createLexNames(uint32_t *text, uint32_t *names, int32_t nTuples, uint32_t *tuples, int level, int module, int32_t &qtyRules);
size_t maxSizeInBytes(int32_t s, int level);
void saveDecodedText(uint32_t *symbol, int32_t symbolSize, char *fileName, unsigned char* rules0, int module);
#endif