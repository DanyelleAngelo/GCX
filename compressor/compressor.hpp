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
 * @brief Calcula a quantidade de sentinelas ($) que devem ser anexadas ao final do texto plano (ou texto reduzido), para que as regras possam ser geradas contendo 3 caractes
 * 
 * @param textSize tamanho do texto
 * @return int quantidade de $ que devem ser anexadas ao final do texto
 * @param module tamanho das tuplas do texto
 */
int numberOfSentries(int32_t textSize, int module);

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
#endif
