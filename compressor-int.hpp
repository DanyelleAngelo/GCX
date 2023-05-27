#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <iostream>
#include <vector>

using namespace std;

/**
 * @brief Identifica a operação escolhida pelo usuário, e chama o método responsável pela operação (encode ou decode).
 * 
 * @param fileIn contém o texto plano (caso a opção escolhida seja encode) ou o texto comprimido (caso a opção escolhida seja decode)
 * @param fileOut arquivo vazio, onde será gravado o resultado da operação escolhida
 * @param op armazena a opção escolhida pelo usuário "e" para encode, e "d" para decode
 */
void grammar(char *fileIn, char *fileOut, char op, int ruleSize);

/**
 * @brief Realiza a leitura e armazena o texto decodificado em uma variável.
 * 
 * @param fileName nome do arquivo contendo o texto plano
 * @param textSize iniciada em 0, ao final da função conterá o tamanho do texto, incluindo $
 * @param module tamanho das tuplas do texto
 */
void readPlainText(char *fileName, long long int &textSize, int module);

/**
 * @brief Realiza a leitura e armazena o texto codificado em uma variável.
 * 
 * @param fileName nome do arquivo contendo a gramática que comprime o texto
 * @param textC texto compactado
 * @param textSize iniciada em 0, ao final da função conterá o tamanho do texto, incluindo as informaçoes da gramática, como quantidade de níveis e número de regras por nível
 * @param rules0 armazena as regras do último nível da recursão, ou seja as regras que são originadas do texto plano
 * @param module tamanho das tuplas do texto
 */
void readCompressedFile(char *fileName, uint32_t *&textC, long long int &textSize,  unsigned char* &rules0, int module);

/**
 * @brief Calcula a quantidade de sentinelas ($) que devem ser anexadas ao final do texto plano (ou texto reduzido), para que as regras possam ser geradas contendo 3 caractes
 * 
 * @param textSize tamanho do texto
 * @return int quantidade de $ que devem ser anexadas ao final do texto
 * @param module tamanho das tuplas do texto
 */
int calculatesNumberOfSentries(long long int textSize, int module);

/**
 * @brief realiza a compactação do texto por meio de sucessivas chamadas recursivas, onde são geradas regras (por meio da ordenação do texto) que codificam o texto.
 * 
 * @param text texto a ser reduzido
 * @param textSize tamanho do texto
 * @param fileName nome do arquivo onde são gravadas as regras geradas a cada nível
 * @param module tamanho das tuplas do texto
 * @param level nível atual
 */
void encode(uint32_t *text, long long int textSize, char *fileName, int level, int module);

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
 */
void decode(uint32_t *text, long long int textSize, int level, int qtyLevels, char *fileName, unsigned char *rules0, int module);

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
 * @param rank array que ao final do método armazenará todos os lex-names de cada trinca
 * @param tupleIndexSize quantidade de trincas
 * @param module tamanho das tuplas do texto
 * @return int número de trincas sem repetição
 */
long int createLexNames(uint32_t *uText, uint32_t *tupleIndex, uint32_t *rank, long int tupleIndexSize, int module);

/**
 * @brief cria um texto reduzido usando os lex-names definidos para cada trinca
 * 
 * @param rank array contendo lex-names
 * @param redText array que conterá o texto reduzido ao final do método
 * @param tupleIndexSize número de trincas (número de trincas é o tamanho do texto reduzido excluindo $)
 * @param textSize tamanho do texto ainda não reduzido
 * @param module tamanho das tuplas do texto
 */
void createReducedText(uint32_t *rank, uint32_t *redText, long long int tupleIndexSize, long long int textSize, long long int redTextSize, int module);

/**
 * @brief Abre o arquivo e posiciona o cursor no ínicio do arquivo. Grava as informações da gramática (quantidade de níveis, e quantidade de regras em cada nível), e em seguida grava o símbolo inicial.
 * 
 * @param fileName nome do arquivo
 * @param startSymbol símbolo inicial
 * @param size: tamanho do símbolo inicial
 */
void storeStartSymbol(char *fileName, uint32_t *startSymbol, int size);

/**
 * @brief Armazena regras do tipo INT geradas em cada nível
 * 
 * @param uText texto que representa a regra que deve ser armazenada
 * @param tupleIndex array contendo os índices iniciais de cada trinca em ordem
 * @param rank classificação de cada trinca
 * @param tupleIndexSize quantidade de trincas
 * @param fileName arquivo onde as regras devem ser salvas
 */
void storeRules(uint32_t *uText, uint32_t *tupleIndex, uint32_t *rank, int tupleIndexSize, char *fileName, int module);

/**
 * @brief Armazena regras do tipo CHAR geradas no último nível
 *
 * @param tupleIndex array contendo os índices iniciais de cada trinca em ordem
 * @param rank classificação de cada trinca
 * @param tupleIndexSize quantidade de trincas
 * @param fileName arquivo onde as regras devem ser salvas
 */
void storeRules(uint32_t *tupleIndex, uint32_t *rank, int tupleIndexSize, char *fileName, int module);

/**
 * @brief Decodifica o textp reduzido em determinado nível da gramática
 * 
 * @param text conteúdo compactado lido do arquivo de entrado
 * @param symbol símbolo a ser decodificado (ao final conterá o símbolo já decodficiado)
 * @param xsSize tamanho do símbolo a ser decodificado (ao final, conterá o tamanho do símbolo já decodificado)
 * @param l nível atual
 * @param start índice onde as regras do nível "l" começam
 * @param module tamanho das tuplas do texto
 */
void decodeSymbol(uint32_t* text, uint32_t *&symbol, long long int &xsSize, int l, int start, int module);

/**
 * @brief abre o arquivo de saída e grava o texto decodificado
 * 
 * @param text texto decodificado
 * @param textSize tamanho do texto
 * @param fileName arquivo de saída
 * @param rules0 armazena as regras do último nível da recursão, ou seja as regras que são originadas do texto plano
 * @param module tamanho das tuplas do texto
 */
void saveDecodedText(uint32_t *symbol, long long int textSize, char *fileName, unsigned char *rules0, int module);

#endif