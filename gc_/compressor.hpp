#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <iostream>

using namespace std;

template <typename T>
void print(T v[], int n){
    cout << *(v);
    for(int i=1; i < n ; i++) cout  << ", " << *(v+i);
     cout << endl;
} 

/**
 * @brief Verifica se FILE é diferente de NULL, em caso negativo exibe uma mensagem de erro e  encerra o programa.
 * 
 * @param file arquivo a ser verificado.
 * @param msg mensagem de erro, contendo o contexto do uso do arquivo.
 */
void isFileOpen(FILE * file, const char *msg);

/**
 * @brief Calcula a quantidade de sentinelas ($) que devem ser anexadas ao final do texto plano (ou texto reduzido)
 * 
 * @param textSize tamanho do texto
 * @return quantidade de $ que devem ser anexadas ao final do texto
 * @param coverage tamanho das tuplas do texto
 */
int padding(int32_t textSize, int coverage);

/**
 * @brief Realiza a leitura do texto plano.
 * 
 * @param fileName nome do arquivo contendo o texto plano
 * @param text array de caracteres usado para armazenar o texto
 * @param textSize iniciada em 0, ao final da função conterá o tamanho do texto, incluindo $
 * @param coverage tamanho das tuplas do texto
 */
void readPlainText(char *fileName,unsigned char *&text, int32_t &textSize, int coverage);

/**
 * @brief ordena o texto com base em tuplas, iniciadas em números múltiplos de `coverage`
 * 
 * @param uText texto a ser ordenado
 * @param nTuples quantidade de tuplas que devem ser ordenadas
 * @param tuples array que ao final do método conterá os índices das tuplas de forma ordenada
 * @param coverage tamanho das tuplas do texto
 * @param sigma tamanho do alfabeto
 */
void radixSort(uint32_t *uText, int32_t nTuples, uint32_t *tuples, uint32_t sigma, int coverage); 

/**
 * @brief cria lex-names para cada tupla ordenada do texto.
 * 
 * @param uText texto a ser codificado
 * @param tupleIndex tuplas que foram ordenadas anteriormente pelo radix-sort
 * @param rank array que ao final do método armazenará todos os lex-names de cada tuplas, ou seja o TEXTO REDUZIDO
 * @param qtyRules armazena o número de tuplas sem repetição (ou seja número de regras únicas)
 * @param nTuples quantidade de tuplas
 * @param coverageule tamanho das tuplas do texto
 */
void createLexNames(uint32_t *uText, uint32_t *tuples, uint32_t *rank, int32_t &qtyRules, long int nTuples, int coverage);

#endif
