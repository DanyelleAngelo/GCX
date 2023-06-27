#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <iostream>
#include <vector>

using namespace std;

/**
 * @brief Realiza a leitura e armazena o texto decodificado em uma variável.
 * 
 * @param fileName nome do arquivo contendo o texto plano
 * @param textSize iniciada em 0, ao final da função conterá o tamanho do texto, incluindo $
 * @param module tamanho das tuplas do texto
 */
void readPlainText(char *fileName,unsigned char *&text, int32_t &textSize, int module);

/**
 * @brief Realiza a leitura e armazena o texto codificado em uma variável.
 * 
 * @param fileName nome do arquivo contendo a gramática que comprime o texto original
 * @param uText texto compactado
 * @param textSize iniciada em 0, ao final da função conterá o tamanho do texto, incluindo as informaçoes da gramática, como quantidade de níveis e número de regras por nível
 * @param rules0 armazena as regras do último nível da recursão, ou seja as regras que são originadas do texto plano
 * @param module tamanho das tuplas do texto
 * @param grammarInfo contém as informações da gramática, quantidade de níveis, quantidade de regras por nível
 */
void readCompressedFile(char *fileName, uint32_t *&uText, int32_t &textSize,  unsigned char* &rules0, int module, vector<uint32_t> &grammarInfo);

/**
 * @brief Calcula a quantidade de sentinelas ($) que devem ser anexadas ao final do texto plano (ou texto reduzido), para que as regras possam ser geradas contendo 3 caractes
 * 
 * @param textSize tamanho do texto
 * @return int quantidade de $ que devem ser anexadas ao final do texto
 * @param module tamanho das tuplas do texto
 */
int numberOfSentries(int32_t textSize, int module);

/**
 * @brief realiza a compactação do texto por meio de sucessivas chamadas recursivas, onde são geradas regras (por meio da ordenação do texto) que codificam o texto.
 * 
 * @param text0 texto plano
 * @param uText texto a ser reduzido, codificado através de um inteiro
 * @param textSize tamanho do texto
 * @param fileName nome do arquivo onde são gravadas as regras geradas a cada nível
 * @param level nível atual
 * @param module tamanho das tuplas do texto
 * @param grammarInfo contém as informações da gramática, quantidade de níveis, quantidade de regras por nível
 */
void encode(unsigned char *text0, uint32_t *uText, int32_t textSize, char *fileName, int level, int module, vector<uint32_t> &grammarInfo);

/**
 * @brief ordena o texto com base em trincas, iniciadas em números múltiplos de 3
 * 
 * @param uText texto a ser ordenado
 * @param tupleIndexSize quantidade de trincas que devem ser ordenadas
 * @param tupleIndex array que ao final do método conterá os índices das trincas de forma ordenada
 * @param module tamanho das tuplas do texto
 */
void radixSort(uint32_t *uText, int tupleIndexSize, uint32_t *tupleIndex, long int level, int module);

/**
 * @brief cria lex-names para cada trinca ordenada do texto (usando o rank)
 * 
 * @param uText texto a ser codificado
 * @param tupleIndex trincas JÁ ORDENADAS
 * @param textR array que ao final do método armazenará todos os lex-names de cada trinca, ou seja o texto reduzido
 * @param tupleIndexSize quantidade de trincas
 * @param module tamanho das tuplas do texto
 * @param qtyRules número de trincas sem repetição (ou seja número de regras únicas)
 */
void createLexNames(uint32_t *uText, uint32_t *tupleIndex, uint32_t *rank, long int tupleIndexSize, int module, int32_t &qtyRules);

/**
 * @brief Armazena regras do tipo INT geradas em cada nível
 * 
 * @param text0 texto de entrada, usado no último nível da recursão
 * @param uText texto representado em inteiro que representa a regra que deve ser armazenada 
 * @param tuples array contendo os índices iniciais de cada trinca em ordem
 * @param rank classificação de cada trinca
 * @param nTuples quantidade de trincas
 * @param fileName arquivo onde as regras devem ser salvas
 * @param level indica o nível em que as regras foram geradas, útil para saber se o texto deve ser armazenado em um char ou inteiro.
 */
void storeRules(unsigned char *text0, uint32_t *uText, uint32_t *tuples, uint32_t *textR, int32_t nTuples, char *fileOut,int32_t module, int level);

/**
 * @brief Abre o arquivo e posiciona o cursor no ínicio do arquivo. Grava as informações da gramática (quantidade de níveis, e quantidade de regras em cada nível), e em seguida grava o símbolo inicial.
 * 
 * @param fileName nome do arquivo
 * @param startSymbol símbolo inicial
 * @param sizeSymbol: tamanho do símbolo inicial
 * @param grammarInfo contém as informações da gramática, quantidade de níveis, quantidade de regras por nível
 */
void storeStartSymbol(char *fileName, uint32_t *startSymbol, int32_t sizeSymbol, vector<uint32_t> &grammarInfo);

#endif
