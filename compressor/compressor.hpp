#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <iostream>
#include <vector>
#include "uarray.h"

using namespace std;

/**
 * @brief Identifica a operação escolhida pelo usuário, e chama o método responsável pela operação (encode, decode ou extract).
 * 
 * @param fileIn nome do arquivo contendo o texto sobre o qual realizaremos nossas operações/
 * @param fileOut arquivo vazio, onde será gravado o resultado da operação escolhida
 * @param reportFile usado para persistência de consumo de tempo e memória.
 * @param queriesFile cada linha do arquivo contém 2 inteiros (l,r) separados por espaço, esses valores são usados durante a operação de extract
 * @param op armazena a opção escolhida pelo usuário "e" para encode, e "d" para decode
 * @param coverage tamanho das regras que devemos gerar.
 */
void grammar(char *fileIn, char *fileOut, char *reportFile, char *queriesFile, char op, int coverage);

/**
 * @brief Realiza a leitura do texto plano.
 * 
 * @param fileName nome do arquivo contendo o texto plano
 * @param text array de caracteres usado para armazenar o texto
 * @param textSize iniciada em 0, ao final da função conterá o tamanho do texto, incluindo o padding
 * @param coverage tamanho das regras que comporão o texto
 */
void readPlainText(char *fileName,unsigned char *&text, i32 &textSize, int coverage);

/**
 * @brief Realiza a leitura do texto compactado.
 * 
 * @param fileName arquivo contendo o texto compactado
 * @param header usado para armanezar quantidade de regras por nível
 * @param encodedSymbols matriz de regras no formato codificado, cada linha representa as regras de um nível
 * @param xsSize tamanho do símbolo inicial
 * @param coverage tamanho das regras do texto
 * @param leafLevelRules armazena as regras do último nível da gramática
 */
void readCompressedFile(char *fileName, i32 *&header, uarray **&encodedSymbols, i32 &xsSize, int coverage, unsigned char *&leafLevelRules);

/**
 * @brief Exibe na tela as informações da gramática
 * 
 * @param header armazena número de regras por nível da gramática
 * @param levels número de níveis
 * @param coverage tamanho das regras na gramática
 */
void grammarInfo(i32 *header, int levels, int coverage);

/**
 * @brief realiza a compactação do texto por meio de sucessivas chamadas recursivas, onde são geradas regras que codificam o texto.
 * 
 * @param text texto a ser reduzido, codificado através de um inteiro
 * @param tuples array de índices de tuplas de sufixos ordenados
 * @param textSize tamanho do texto
 * @param fileName nome do arquivo onde são gravadas as regras geradas a cada nível
 * @param level nível atual
 * @param coverage tamanho das tuplas do texto
 * @param header contém as informações da gramática, quantidade de níveis, quantidade de regras por nível
 * @param sigma armazena o tamanho do alfabeto a ser usado nesse nível (é a quantidade de regras do nível anterior.)
 */
void compress(i32 *text, i32 *tuples, i32 textSize, char *fileName, int level, int coverage, vector<i32> &header, i32 sigma);

/**
 * @brief Decodifica texto
 * 
 * @param text array que conterá o texto decodificado
 * @param header array com informações de regras por nível
 * @param encodedSymbols matriz de regras no formato codificado, cada linha representa as regras de um nível
 * @param xsSize tamanho do símbolo inicial, ao final do algoritmo conterá o tamanho do texto decodificado
 * @param leafLevelRules array contendo as regras do último nível do processo recursivo
 * @param coverage tamanho das regras
 */
void decode(unsigned char *&text, i32 *header, uarray **encodedSymbols, i32 &xsSize, unsigned char *leafLevelRules, int coverage);

/**
 * @brief Responsável por realizar a extração em batch de múltiplos intervalos. Além de tudo este método é reponsável por salvar o resultado da extração.
 * 
 * @param fileName caminho do arquivo onde serão anexado o resultado do extract de cada intervalo
 * @param text substring decodificada
 * @param subtree_size tamanho máximo das subárvores em cada nível 
 * @param encodedSymbols matriz de regras no formato codificado, cada linha representa as regras de um nível
 * @param leafLevelRules array contendo as regras do último nível do processo recursivo
 * @param coverage tamanho das regras
 * @param txtSize tamanho do intervalo a ser decodificado
 * @param queries vetor de intervalos usado para o processo de extração
 * @param levels quantidade de níveis da gramática que codifica o texto
 * @return tempo total para realizar todas as operações de extração
 */
double extract_batch(char *fileName, unsigned char *&text, int *subtree_size, uarray **encodedSymbols, unsigned char *leafLevelRules, int coverage, i32 txtSize, vector<pair<i32, i32>> queries, int levels);

/**
 * @brief Realiza a extração de uma substring do texto compactado em um intervalo fechado especificado.
 * 
 * @param text responsável por armazenar a substring decodificada
 * @param subtree_size tamanho máximo das subárvores em cada nível 
 * @param encodedSymbols matriz de regras no formato codificado, cada linha representa as regras de um nível
 * @param leafLevelRules array contendo as regras do último nível do processo recursivo
 * @param coverage tamanho das regras
 * @param txtSize tamanho do intervalo a ser decodificado
 * @param l limitante inferior
 * @param r limitante superior
 * @param levels quantidade de níveis da gramática que codifica o texto
 */
void extract(unsigned char *&text, i32 *temp, i32 *xs, int *subtree_size, uarray **encodedSymbols, unsigned char *leafLevelRules, int coverage, i32 txtSize, i32 l, i32 r, int levels);


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
 * @param text contém as regras do último nível da recursão
 * @param rules array onde serão armazenadas as regras únicas
 * @param tuples array contendo os índices iniciais de cada tupla em ordem lexicográfica
 * @param rank classificação de cada tupla
 * @param nTuples quantidade de tuplas
 * @param coverage tamanho das regras
 * @param level indica o nível em que as regras foram geradas
 * @param qtyRules quantidade de regras
 */
void selectUniqueRules(i32 *text, unsigned char *&rules, i32 *tuples, i32 *rank, i32 nTuples, int coverage, int level, i32 qtyRules);

/**
 * @brief Seleciona as regras únicas do nível atual da recursão, e as armazena em ordem lexicográfica (em relação ao símbolo não terminal) em uma estrutura de dados, para posteriormente serem gravadas em um arquivo.
 * @param text contém as regras dos demais níveis
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
  * @brief armazena as regras da gramática
  * 
  * @param fileName arquivo onde as regras devem ser salvas
  * @param encdIntRules regras dos níveis internos
  * @param leafRules regras do último nível da recursão
  * @param level nível atual
  * @param size quantidade de símbolos a serem armazenados
  */
void storeRules(char *fileName, uarray *encdIntRules, unsigned char *leafRules, int level, i32 size);

/**
 * @brief decodifica um símbolo xs
 * 
 * @param encodedSymbols array contendo as regras codificadas do nível anterior (que gerou xs)
 * @param xs texto reduzido que precisa ser decodificado
 * @param xsSize tamanho do texto codificado
 * @param coverage tamanho das regras
 */
void decodeSymbol(uarray *encodedSymbols, i32 *&xs, i32 &xsSize, int coverage);

/**
 * @brief Salva o texto decodificado
 * 
 * @param fileName caminho do arquivo de destino
 * @param text texto decodificado
 * @param size tamanho do texto
 */
void saveDecodedText(char *fileName, unsigned char* text, i32 size);

#endif