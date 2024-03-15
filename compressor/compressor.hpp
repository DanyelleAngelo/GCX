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
 */
void grammar(char *fileIn, char *fileOut, char *reportFile, char *queriesFile, string op);

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
 * @param levelCoverage lista de tamanho das regras por nível
 * @param leafLevelRules armazena as regras do último nível da gramática
 */
void readCompressedFile(char *fileName, i32 *&header, uarray **&encodedSymbols, i32 &xsSize, int *&levelCoverage, unsigned char *&leafLevelRules);

/**
 * @brief Exibe na tela as informações da gramática
 * 
 * @param header armazena número de regras por nível da gramática
 * @param levels número de níveis
 * @param levelCoverage vetor com o tamanho das regras da gramática
 */
void grammarInfo(i32 *header, int levels, int *levelCoverage);

/**
 * @brief realiza a compactação do texto por meio de sucessivas chamadas recursivas, onde são geradas regras que codificam o texto.
 * 
 * @param text texto a ser reduzido, codificado através de um inteiro
 * @param tuples array de índices de tuplas de sufixos ordenados
 * @param textSize tamanho do texto
 * @param fileName nome do arquivo onde são gravadas as regras geradas a cada nível
 * @param level nível atual
 * @param levelCoverage vetro com o tamanho das regras do texto
 * @param header contém as informações da gramática, quantidade de níveis, quantidade de regras por nível
 * @param sigma armazena o tamanho do alfabeto a ser usado nesse nível (é a quantidade de regras do nível anterior.)
 */
void compress(i32 *text, i32 *tuples, i32 textSize, char *fileName, int level, vector<int> &levelCoverage, vector<i32> &header, i32 sigma);

/**
 * @brief Calcula o lcp médio de regras já ordenadas e com tamanho igual á coverage
 * 
 * @param text texto a ser compactado
 * @param tuples array de sufixos do texto ordenado
 * @param textSize tamanho máximo do texto
 * @param coverage cobertura inicial passada durante a invocação do programa, parâmetro x
 * @param sigma tamanho do alfabeto
 * @return int lcp médio das regras (considerando apenas regras únicas)
 */
int getLcpMean(i32 *text, i32 *tuples, i32 textSize, int coverage, i32 sigma);

/**
 * @brief verifica se a geração de regras continua a covergir
 * 
 * @param level nível atual da recursão
 * @param lcp_mean  lcp_mean calculado neste nível, indica o tamanho das regras
 * @return int - retorna 0, se o nível tiver qtyRules = nTuples, ou quando a condição anteriror for falsa mas lcp_mean = 1. Retorna 1 caso nenhuma das condições anteriores for atendida.
 */
int checkCoverageConvergence(int level, int lcp_mean, i32 qtyRules, i32 nTuples);

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
 * @brief Cria e abre o arquivo para gravar as informações da gramática, e em seguida grava o símbolo inicial.
 * 
 * @param fileName nome do arquivo
 * @param text símbolo inicial
 * @param header contém as informações da gramática
 * @param levelCoverage vetor com o tamanho das regras por nível
 */
void storeStartSymbol(char *fileName, i32 *text, vector<i32> &header, vector<int> &levelCoverage);

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
 * @brief Decodifica texto
 * 
 * @param text array que conterá o texto decodificado
 * @param levels número de níveis na gramática
 * @param encodedSymbols matriz de regras no formato codificado, cada linha representa as regras de um nível
 * @param xsSize tamanho do símbolo inicial, ao final do algoritmo conterá o tamanho do texto decodificado
 * @param leafLevelRules array contendo as regras do último nível do processo recursivo
 * @param levelCoverage vetor com o tamanho das regras
 */
void decode(unsigned char *&text, int levels, uarray **encodedSymbols, i32 &xsSize, unsigned char *leafLevelRules, int *levelCoverage);

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

/**
 * @brief Responsável por realizar a extração em batch de múltiplos intervalos. Além de tudo este método é reponsável por salvar o resultado da extração.
 * 
 * @param fileName caminho do arquivo onde serão anexado o resultado do extract de cada intervalo
 * @param text substring decodificada
 * @param subtreeSize tamanho máximo das subárvores em cada nível 
 * @param encodedSymbols matriz de regras no formato codificado, cada linha representa as regras de um nível
 * @param leafLevelRules array contendo as regras do último nível do processo recursivo
 * @param levelCoverage vetor com o tamanho das regras
 * @param txtSize tamanho do intervalo a ser decodificado
 * @param queries vetor de intervalos usado para o processo de extração
 * @param levels quantidade de níveis da gramática que codifica o texto
 * @return tempo total para realizar todas as operações de extração
 */
double extractBatch(char *fileName, unsigned char *&text, int *subtreeSize, uarray **encodedSymbols, unsigned char *leafLevelRules, int *levelCoverage, i32 txtSize, vector<pair<i32, i32>> queries, int levels);

/**
 * @brief Realiza a extração de uma substring do texto compactado em um intervalo fechado especificado.
 * 
 * @param text responsável por armazenar a substring decodificada
 * @param subtreeSize tamanho máximo das subárvores em cada nível 
 * @param encodedSymbols matriz de regras no formato codificado, cada linha representa as regras de um nível
 * @param leafLevelRules array contendo as regras do último nível do processo recursivo
 * @param levelCoverage vetor com o tamanho das regras
 * @param txtSize tamanho do intervalo a ser decodificado
 * @param l limitante inferior
 * @param r limitante superior
 * @param levels quantidade de níveis da gramática que codifica o texto
 */
void extract(unsigned char *&text, i32 *temp, i32 *xs, int *subtreeSize, uarray **encodedSymbols, unsigned char *leafLevelRules, int *levelCoverage, i32 txtSize, i32 l, i32 r, int levels);

#endif