#ifndef COMPRESSOR_INT_H
#define COMPRESSOR_INT_H

#include <iostream>
#include <vector>

using namespace std;
/**
 * @brief Identifica a operação escolhida pelo usuário, e chama o método responsável pela operação (encode ou decode).
 * 
 * @param fileIn contém o texto plano (caso a opção escolhida seja encode) ou o texto comprimido (caso a opção escolhida seja decode)
 * @param fileOut arquivo vazio, onde será gravado o resultado da operação escolhida
 * @param op armazena a opção escolhida pelo usuário "e" para encode, e "d" para decode
 * @param rulesSize tamanho das regras que devemos gerar
 */
void grammar(char *fileIn, char *fileOut, char op, int ruleSize);

/**
 * @brief Realiza a leitura e armazena o texto codificado em uma variável.
 * 
 * @param fileName nome do arquivo contendo a gramática que comprime o texto original
 * @param textC texto compactado
 * @param textSize iniciada em 0, ao final da função conterá o tamanho do texto, incluindo as informaçoes da gramática, como quantidade de níveis e número de regras por nível
 * @param rules0 armazena as regras do último nível da recursão, ou seja as regras que são originadas do texto plano
 * @param module tamanho das tuplas do texto
 * @param grammarInfo contém as informações da gramática, quantidade de níveis, quantidade de regras por nível
 */
void readCompressedFile(char *fileName, uint32_t *&textC, int32_t &textSize,  unsigned char* &rules0, int module, vector<uint32_t> &grammarInfo);

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
 * @brief realiza a descompressão do texto, por meio da decodificação do texto nível a nível.
 * 
 * @param text texto a ser decodificado
 * @param textSize tamanho do texto no nível atual
 * @param level nível atual
 * @param qtyLevels quantidade de níveis na gramática
 * @param fileName arquivo onde deve ser gravado o resultado da descompressão
 * @param rules0 armazena as regras do último nível da recursão, ou seja as regras que são originadas do texto plano
 * @param module tamanho das tuplas do texto
 * @param grammarInfo contém as informações da gramática, quantidade de níveis, quantidade de regras por nível
 */
void decode(uint32_t *text, int32_t textSize, int level, int qtyLevels, char *fileName, unsigned char *rules0, int module, vector<uint32_t> &grammarInfo);

/**
 * @brief Abre o arquivo e posiciona o cursor no ínicio do arquivo. Grava as informações da gramática (quantidade de níveis, e quantidade de regras em cada nível), e em seguida grava o símbolo inicial.
 * 
 * @param fileName nome do arquivo
 * @param startSymbol símbolo inicial
 * @param sizeSymbol: tamanho do símbolo inicial
 * @param grammarInfo contém as informações da gramática, quantidade de níveis, quantidade de regras por nível
 */
void storeStartSymbol(char *fileName, uint32_t *startSymbol, int sizeSymbol, vector<uint32_t> &grammarInfo);

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
 * @brief Decodifica o textp reduzido em determinado nível da gramática
 * 
 * @param text conteúdo compactado lido do arquivo de entrado
 * @param symbol símbolo a ser decodificado (ao final conterá o símbolo já decodficado)
 * @param xsSize tamanho do símbolo a ser decodificado (ao final, conterá o tamanho do símbolo já decodificado)
 * @param l nível atual
 * @param start índice onde as regras do nível "l" começam
 * @param module tamanho das tuplas do texto
 */
void decodeSymbol(uint32_t* text, uint32_t *&symbol, int32_t &xsSize, int l, int start, int module);

/**
 * @brief abre o arquivo de saída e grava o texto decodificado
 * 
 * @param text texto decodificado
 * @param textSize tamanho do texto
 * @param fileName arquivo de saída
 * @param rules0 armazena as regras do último nível da recursão, ou seja as regras que são originadas do texto plano
 * @param module tamanho das tuplas do texto
 */
void saveDecodedText(uint32_t *symbol, int32_t textSize, char *fileName, unsigned char *rules0, int module);

#endif