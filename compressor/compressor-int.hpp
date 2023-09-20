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
void grammarInteger(char *fileIn, char *fileOut, char op, i32 l, i32 r, int coverage);

/**
 * @brief Exibe na tela as informações da gramática
 * 
 * @param header armazena quantidade de níveis e número de regras por nível da gramática
 * @param levels número de níveis
 * @param coverage tamanho das regras na gramática
 */
void grammarInfo(i32 *header, int levels, int coverage);

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
void compress(unsigned char *text0, i32 *uText, i32 *tuples, i32 textSize, char *fileName, int level, int coverage, vector<i32> &header, i32 sigma);

/**
 * @brief Ler e decodifica arquivo nível por nível
 * 
 * @param compressedFile arquivo compactado
 * @param decompressedFile arquivo onde deve ser gravado o resultado da descompactação
 * @param header contém as informações da gramática, quantidade de níveis, quantidade de regras por nível
 * @param coverage tamanho das tuplas do texto
 */
void decode(char *compressedFile, char *decompressedFile, i32 *&header, int coverage);

/**
 * @brief Extraí e salva, a partir do texto codificado, o texto decodificado correspondente ao intervalo [l, r]. Se r for maior que o limite do texto original, retorna o texto compreendido entre [l,n] sendo n o tamanho do texto original.
 * 
 * @param fileIn arquivo de texto compactado
 * @param fileOut arquivo onde será armazenado o texto decodificado no intervalo [l,r]
 * @param l limite inferior
 * @param r limite superior
 * @param coverage cobertura (tamanho de regra)
 */
void extract(char *fileIn, char *fileOut, i32 l, i32 r, int coverage);

/**
 * @brief Cria e abre o arquivo para gravar as informações da gramática, e em seguida grava o símbolo inicial.
 * 
 * @param fileName nome do arquivo
 * @param startSymbol símbolo inicial
 * @param header contém as informações da gramática
 */
void storeStartSymbol(char *fileName, i32 *startSymbol, vector<i32> &header);

/**
 * @brief Seleciona as regras únicas do nível atual da recursão, e as armazena em ordem lexicográfica (em relação ao símbolo não terminal) em uma estrutura de dados, para posteriormente serem gravadas em um arquivo.
 * 
 * @param text0 contém as regras do último nível da recursão
 * @param rules array onde serão armazenadas as regras únicas
 * @param tuples array contendo os índices iniciais de cada tupla em ordem lexicográfica
 * @param rank classificação de cada tupla
 * @param nTuples quantidade de tuplas
 * @param coverage tamanho das regras
 * @param level indica o nível em que as regras foram geradas
 * @param qtyRules quantidade de regras
 */
void selectUniqueRules(unsigned char *text, unsigned char *&rules, i32 *tuples, i32 *rank, i32 nTuples, int coverage, int level, i32 qtyRules);

/**
 * @brief Seleciona as regras únicas do nível atual da recursão, e as armazena em ordem lexicográfica (em relação ao símbolo não terminal) em uma estrutura de dados, para posteriormente serem gravadas em um arquivo.
 * @param uText contém as regras dos demais níveis
 * @param rules array onde serão armazenadas as regras únicas
 * @param tuples array contendo os índices iniciais de cada tupla em ordem lexicográfica
 * @param rank classificação de cada tupla
 * @param nTuples quantidade de tuplas
 * @param coverage tamanho das regras
 * @param level indica o nível em que as regras foram geradas
 * @param qtyRules quantidade de regras
 * @param sigma tamanho do alfabeto usado para compor as regras
 */
void selectUniqueRules(i32 *text, uarray *&rules, i32 *tuples, i32 *rank, i32 nTuples, int coverage, int level, i32 qtyRules, i32 sigma);

 /**
  * @brief 
  * 
  * @param fileName arquivo onde as regras devem ser salvas
  * @param encdIntRules regras dos níveis internos
  * @param leafRules regras do último nível da recursão
  * @param level nível atual
  * @param size quantidade de símbolos a serem armazenados
  */
void storeRules(char *fileName, uarray *encdIntRules, unsigned char *leafRules, int level, i32 size);

/**
 * @brief Faz a leitura do cabeçalho da gramática e do símbolo inicial
 * 
 * @param compressedFile arquivo de texto compactado
 * @param header array onde será salvo as informações da gramática
 * @param xsEncoded array onde será salvo o símbolo inicial de forma compactada
 * @param xsSize tamanho do símbolo inicial (obitido diretamente de header)
 * @param coverage cobertura da gramática (tamanho das regras)
 */
void getHeaderAndXs(FILE *compressedFile, i32 *&header, uarray *&xsEncoded, i32 &xsSize, int coverage);

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
void decodeSymbol(FILE *compressedFile, i32 sizeRules, i32 sigma, i32 *&xs, i32 &xsSize, int coverage);

/**
 * @brief captura as regras do último nível, e fecha o arquivo de leitura.
 * 
 * @param file arquivo de leitura que armazena o texto compactado.
 * @param size quantidade de símbolos a serem lidos no último nível, obtido a partir do cabeçalho que armazena a quantidade de regras em cada nível. 
 * @param rules array onde será salvo as regras lidas.
 */
void getRulesInTheLastLevel(FILE *file, i32 size, unsigned char *&rules);

/**
 * @brief abre o arquivo de saída e grava o texto decodificado
 * 
 * @param fileName nome do arquivo
 * @param xs texto codificado
 * @param xsSize tamanho de xs
 * @param rules regras do último nível da recursão
 * @param coverage tamanho das tuplas do texto
 */
void saveDecodedText(char *fileName, i32 *xs, i32 xsSize, unsigned char *rules, int coverage);

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
void searchInterval(FILE *compressedFile, unsigned char *&plainTxt, i32 *xs, i32 *header, i32 xsSize, i32 &txtSize, i32 l, i32 r, int coverage);

#endif