#ifndef COMPRESSOR_ELIAS_H
#define COMPRESSOR_ELIAS_H

#include <iostream>
#include <vector>
#include <sdsl/int_vector.hpp>
using namespace std;
using namespace sdsl;

void readCompressedFile(char *fileName, int_vector<32> &decoded, int &levels);
void encode(uint32_t *uText, long long int textSize, int level, int module);
void decode(int_vector<32> decoded, int level, int qtyLevels, int module, char *fileName);
void encodeTextWithEliasAndSave(char *fileName);
void decodeSymbol(int_vector<32> decoded, uint32_t *&symbol, long long int &xsSize, int level, int startRules, int module);
void saveDecodedText(int_vector<32> decoded, uint32_t *symbol, int xsSize, int module, char *fileName);

#endif
