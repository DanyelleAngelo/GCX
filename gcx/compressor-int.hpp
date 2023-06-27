#ifndef COMPRESSOR_INT_H
#define COMPRESSOR_INT_H

#include <iostream>
#include <vector>

using namespace std;
/**
 * @brief Identifica a operação escolhida pelo usuário, e chama o método responsável pela operação (encode ou decode).
 * 
 * @param fileIn contém o texto plano (caso a opção escolhida seja encode) ou o texto comprimido (caso a opção escolhida seja decode)
 * @param fileOut arquivo vazio, onde será gravado o resultado da operação escolhida
 * @param op armazena a opção escolhida pelo usuário "e" para encode, e "d" para decode
 * @param rulesSize tamanho das regras que devemos gerar
 */
void grammar(char *fileIn, char *fileOut, char op, int ruleSize);

/**
 * @brief realiza a descompressão do texto, por meio da decodificação do texto nível a nível.
 * 
 * @param text texto a ser decodificado
 * @param textSize tamanho do texto no nível atual
 * @param level nível atual
 * @param qtyLevels quantidade de níveis na gramática
 * @param fileName arquivo onde deve ser gravado o resultado da descompressão
 * @param rules0 armazena as regras do último nível da recursão, ou seja as regras que são originadas do texto plano
 * @param module tamanho das tuplas do texto
 * @param grammarInfo contém as informações da gramática, quantidade de níveis, quantidade de regras por nível
 */
void decode(uint32_t *text, int32_t textSize, int level, int qtyLevels, char *fileName, unsigned char *rules0, int module, vector<uint32_t> &grammarInfo);

/**
 * @brief Decodifica o textp reduzido em determinado nível da gramática
 * 
 * @param text conteúdo compactado lido do arquivo de entrado
 * @param symbol símbolo a ser decodificado (ao final conterá o símbolo já decodficado)
 * @param xsSize tamanho do símbolo a ser decodificado (ao final, conterá o tamanho do símbolo já decodificado)
 * @param l nível atual
 * @param start índice onde as regras do nível "l" começam
 * @param module tamanho das tuplas do texto
 */
void decodeSymbol(uint32_t* text, uint32_t *&symbol, int32_t &xsSize, int l, int start, int module);

/**
 * @brief abre o arquivo de saída e grava o texto decodificado
 * 
 * @param text texto decodificado
 * @param textSize tamanho do texto
 * @param fileName arquivo de saída
 * @param rules0 armazena as regras do último nível da recursão, ou seja as regras que são originadas do texto plano
 * @param module tamanho das tuplas do texto
 */
void saveDecodedText(uint32_t *symbol, int32_t textSize, char *fileName, unsigned char *rules0, int module);

#endif