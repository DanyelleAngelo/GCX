#ifndef COMPRESSOR_ELIAS_H
#define COMPRESSOR_ELIAS_H

#include <iostream>
#include <vector>
#include <sdsl/int_vector.hpp>
using namespace std;
using namespace sdsl;


/**
 * @brief Identifica a operação escolhida pelo usuário, e chama o método responsável pela operação (encode ou decode).
 * 
 * @param fileIn contém o texto plano (caso a opção escolhida seja encode) ou o texto comprimido (caso a opção escolhida seja decode)
 * @param fileOut arquivo vazio, onde será gravado o resultado da operação escolhida
 * @param op armazena a opção escolhida pelo usuário "e" para encode, e "d" para decode
 */
void grammar(char *fileIn, char *fileOut, char op, int ruleSize);

void readCompressedFile(char *fileName, int_vector<32> &decoded, int &levels);
void encode(uint32_t *uText, long long int textSize, int level, int module);
void decode(int_vector<32> decoded, int level, int qtyLevels, int module, char *fileName);
void encodeTextWithEliasAndSave(char *fileName);
void decodeSymbol(int_vector<32> decoded, uint32_t *&symbol, long long int &xsSize, int level, int startRules, int module);
void saveDecodedText(int_vector<32> decoded, uint32_t *symbol, int xsSize, int module, char *fileName);

#endif
