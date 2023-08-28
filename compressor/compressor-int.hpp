#ifndef COMPRESSOR_INT_H
#define COMPRESSOR_INT_H

#include <iostream>
#include <vector>
#include "uarray.h"

using namespace std;

/**
 * @brief Identifica a operação escolhida pelo usuário, e chama o método responsável pela operação (encode ou decode).
 * 
 * @param fileIn nome do arquivo contendo o texto sobre o qual realizaremos nossas operações/
 * @param fileOut arquivo vazio, onde será gravado o resultado da operação escolhida
 * @param op armazena a opção escolhida pelo usuário "e" para encode, e "d" para decode
 * @param l limite inferior do intervalo de extract
 * @param r limite superior do intervalo de extract
 * @param rulesSize tamanho das regras que devemos gerar
 */
void grammarInteger(char *fileIn, char *fileOut, char op, int32_t l, int32_t r, int ruleSize);

/**
 * @brief Exibe na tela as informações da gramática
 * 
 * @param header armazena quantidade de níveis e número de regras por nível da gramática
 * @param levels número de níveis
 * @param coverage tamanho das regras na gramática
 */
void grammarInfo(uint32_t *header, int levels, int coverage);

/**
 * @brief realiza a compactação do texto por meio de sucessivas chamadas recursivas, onde são geradas regras que codificam o texto.
 * 
 * @param text0 texto plano
 * @param uText texto a ser reduzido, codificado através de um inteiro
 * @param textSize tamanho do texto
 * @param fileName nome do arquivo onde são gravadas as regras geradas a cada nível
 * @param level nível atual
 * @param coverage tamanho das tuplas do texto
 * @param header contém as informações da gramática, quantidade de níveis, quantidade de regras por nível
 * @param sigma armazena o tamanho do alfabeto a ser usado nesse nível (é a quantidade de regras do nível anterior.)
 */
void compress(unsigned char *text0, uint32_t *uText, int32_t textSize, char *fileName, int level, int coverage, vector<uint32_t> &header, uint32_t sigma);

/**
 * @brief Ler e decodifica arquivo nível por nível
 * 
 * @param compressedFile arquivo compactado
 * @param decompressedFile arquivo onde deve ser gravado o resultado da descompactação
 * @param header contém as informações da gramática, quantidade de níveis, quantidade de regras por nível
 * @param coverage tamanho das tuplas do texto
 */
void decode(char *compressedFile, char *decompressedFile, uint32_t *&header, int coverage);

/**
 * @brief Extraí e salva, a partir do texto codificado, o texto decodificado correspondente ao intervalo [l, r]. Se r for maior que o limite do texto original, retorna o texto compreendido entre [l,n] sendo n o tamanho do texto original.
 * 
 * @param fileIn arquivo de texto compactado
 * @param fileOut arquivo onde será armazenado o texto decodificado no intervalo [l,r]
 * @param l limite inferior
 * @param r limite superior
 * @param coverage cobertura (tamanho de regra)
 */
void extract(char *fileIn, char *fileOut, int32_t l, int32_t r, int coverage);

/**
 * @brief Cria e abre o arquivo para gravar as informações da gramática, e em seguida grava o símbolo inicial.
 * 
 * @param fileName nome do arquivo
 * @param startSymbol símbolo inicial
 * @param header contém as informações da gramática
 */
void storeStartSymbol(char *fileName, uint32_t *startSymbol, vector<uint32_t> &header);

/**
 * @brief Armazena regras geradas em cada nível
 * 
 * @param text0 contém as regras do último nível da recursão
 * @param uText contém as regras dos demais níveis
 * @param tuples array contendo os índices iniciais de cada tupla em ordem lexicográfica
 * @param rank classificação de cada tupla
 * @param nTuples quantidade de tuplas
 * @param fileName arquivo onde as regras devem ser salvas
 * @param level indica o nível em que as regras foram geradas.
 * @param header usado para capturar a quantidade de regras no nível atual, e no nível anterior (indica o tamanho do alfabeto nesse nível)
 */
void storeRules(unsigned char *text0, uint32_t *uText, uint32_t *tuples, uint32_t *rank, int32_t nTuples, char *fileName, int coverage, int level, vector<uint32_t>  header, int32_t qtyRules);

/**
 * @brief Faz a leitura do cabeçalho da gramática e do símbolo inicial
 * 
 * @param compressedFile arquivo de texto compactado
 * @param header array onde será salvo as informações da gramática
 * @param xsEncoded array onde será salvo o símbolo inicial de forma compactada
 * @param xsSize tamanho do símbolo inicial (obitido diretamente de header)
 * @param coverage cobertura da gramática (tamanho das regras)
 */
void getHeaderAndXs(FILE *compressedFile, uint32_t *&header, uarray *&xsEncoded, int32_t &xsSize, int coverage);

/**
 * @brief Ler ar regras do próximo nível e as usa pra decodificar Xs atual
 * 
 * @param compressedFile arquivo de texto compactado
 * @param sizeRules tamanho das regras usadas para gerar Xs atual, e que serão usadas para decodificá-lo
 * @param sigma tamanho do alfabeto usado para gerar Xs atual
 * @param xs texto reduzido a ser decodificado, ao final conterá o símbolo já decodficado
 * @param xsSize tamanho do símbolo a ser decodificado (ao final, conterá o tamanho do símbolo já decodificado)
 * @param coverage tamanho das tuplas do texto
 */
void decodeSymbol(FILE *compressedFile, int32_t sizeRules, int32_t sigma, uint32_t *&xs, int32_t &xsSize, int coverage);

/**
 * @brief captura as regras do último nível, e fecha o arquivo de leitura.
 * 
 * @param file arquivo de leitura que armazena o texto compactado.
 * @param size quantidade de símbolos a serem lidos no último nível, obtido a partir do cabeçalho que armazena a quantidade de regras em cada nível. 
 * @param rules array onde será salvo as regras lidas.
 */
void getRulesInTheLastLevel(FILE *file, int32_t size, unsigned char *&rules);

/**
 * @brief abre o arquivo de saída e grava o texto decodificado
 * 
 * @param fileName nome do arquivo
 * @param xs texto codificado
 * @param xsSize tamanho de xs
 * @param rules regras do último nível da recursão
 * @param coverage tamanho das tuplas do texto
 */
void saveDecodedText(char *fileName, uint32_t *xs, uint32_t xsSize, unsigned char *rules, int coverage);

/**
 * @brief Extrai o texto compreendido entre [l,r] do texto codificado
 * 
 * @param compressedFile arquivo com o texto compactado
 * @param plainTxt vetor onde será armazenado o intervalo decodificado
 * @param xs símbolo inicial, já dentro do intervalo a ser decodificado
 * @param header cabeçalho do texto compactado
 * @param xsSize tamanho de Xs, considerando apenas o intervalo que queremos decodifcar
 * @param txtSize tamanho do intervalo a ser decodificado
 * @param l limite inferior
 * @param r limite superior
 * @param coverage área de cobertura (tamanho da regra)
 */
void searchInterval(FILE *compressedFile, unsigned char *&plainTxt, uint32_t *xs, uint32_t *header, int32_t xsSize, int32_t &txtSize, int32_t l, int32_t r, int coverage);

#endif