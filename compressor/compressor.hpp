#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <iostream>
#include <cstdint>
#include "abbrevs.h"

using namespace std;

template <typename T>
void print(T v[], int n, const char *msg){
    printf("\n%s", msg);
    cout << *(v);
    for(int i=1; i < n ; i++) cout  << ", " << *(v+i);
     cout << endl;
} 
/**
 * @brief Recebe uma mensagem de erro, a exibe e encerra o programa.
 * 
 * @param msg mensagem de erro.
 */
void error(const char *msg);

/**
 * @brief Verifica se FILE é diferente de NULL, em caso negativo chama a função error passando uma mensagem como parâmetro.
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
int padding(i32 textSize, int coverage);

/**
 * @brief Realiza a leitura do texto plano.
 * 
 * @param fileName nome do arquivo contendo o texto plano
 * @param text array de caracteres usado para armazenar o texto
 * @param textSize iniciada em 0, ao final da função conterá o tamanho do texto, incluindo $
 * @param coverage tamanho das tuplas do texto
 */
void readPlainText(char *fileName,unsigned char *&text, i32 &textSize, int coverage);

/**
 * @brief ordena o texto com base em tuplas, iniciadas em números múltiplos de `coverage`
 * 
 * @param uText texto a ser ordenado
 * @param nTuples quantidade de tuplas que devem ser ordenadas
 * @param tuples array que ao final do método conterá os índices das tuplas de forma ordenada
 * @param coverage tamanho das tuplas do texto
 * @param sigma tamanho do alfabeto
 */
void radixSort(i32 *uText, i32 nTuples, i32 *tuples, i32 sigma, int coverage); 

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
void createLexNames(i32 *uText, i32 *tuples, i32 *rank, i32 &qtyRules, long int nTuples, int coverage);

#endif
