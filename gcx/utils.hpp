#ifndef UTILS_COMPRESSOR_H
#define UTILS_COMPRESSOR_H

#include <iostream>
#include <cstdint>
#include "abbrevs.h"
#include "uarray.h"

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
 * @brief ordena o texto com base em tuplas, iniciadas em números múltiplos de `coverage`
 * 
 * @param text texto a ser ordenado
 * @param nTuples quantidade de tuplas que devem ser ordenadas
 * @param tuples array que ao final do método conterá os índices das tuplas de forma ordenada
 * @param sigma tamanho do alfabeto
 * @param coverage tamanho das tuplas do texto
 */
void radixSort(const i32 *text, i32 nTuples, i32 *&tuples, i32 sigma, int coverage); 

/**
 * @brief cria lex-names para cada tupla ordenada do texto.
 * 
 * @param text texto a ser codificado
 * @param tuples tuplas que foram ordenadas anteriormente pelo radix-sort
 * @param rank array que ao final do método armazenará todos os lex-names de cada tuplas, ou seja o TEXTO REDUZIDO
 * @param qtyRules armazena o número de tuplas sem repetição (ou seja número de regras únicas)
 * @param nTuples quantidade de tuplas
 * @param coverage tamanho das tuplas do texto
 */
void createLexNames(const i32 *text, i32 *tuples, i32 *rank, i32 &qtyRules, long int nTuples, int coverage);

/**
 * @brief insere uma linha contendo informações de consumo de tempo e memória em um relatório csv
 * 
 * @param fileName nome do arquivo onde o relatório deve ser gravado
 * @param duration duração em segundos da operação
 * @param base consumo de memória
 */
void generateReport(char *fileName, double duration, void *base);
#endif
