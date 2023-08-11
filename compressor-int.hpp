#ifndef COMPRESSOR_INT_H
#define COMPRESSOR_INT_H

#include <iostream>
#include <vector>

using namespace std;

/**
 * @brief Identifica a operação escolhida pelo usuário, e chama o método responsável pela operação (encode ou decode).
 * 
 * @param fileIn nome do arquivo contendo o texto sobre o qual realizaremos nossas operações/
 * @param fileOut arquivo vazio, onde será gravado o resultado da operação escolhida
 * @param op armazena a opção escolhida pelo usuário "e" para encode, e "d" para decode
 * @param rulesSize tamanho das regras que devemos gerar
 */
void grammarInteger(char *fileIn, char *fileOut, char op, int ruleSize);

/**
 * @brief Exibe na tela as informações da gramática
 * 
 * @param header armazena quantidade de níveis e número de regras por nível da gramática
 * @param levels número de níveis
 * @param mod tamanho das regras na gramática
 */
void grammarInfo(vector<uint32_t> header, int levels, int mod);

/**
 * @brief Realiza a leitura e armazena o texto codificado em uma variável.
 * 
 * @param fileName nome do arquivo contendo a gramática que comprime o texto
 * @param uText responsável por armazenar o texto compactado ao final do método
 * @param textSize armazena o tamanho tamanho do texto (excluindo as informaçoes da gramática,e as regras do último nível)
 * @param charRules armazena as regras do último nível da recursão, ou seja as regras que são originadas do texto plano
 * @param mod tamanho das tuplas do texto
 * @param header contém as informações da gramática, quantidade de níveis, bem como quantidade de regras por nível
 */
void readCompressedFile(char *fileName, uint32_t *&uText, int32_t &textSize,  unsigned char* &charRules, int mod, vector<uint32_t> &header);

/**
 * @brief realiza a compactação do texto por meio de sucessivas chamadas recursivas, onde são geradas regras que codificam o texto.
 * 
 * @param text0 texto plano
 * @param uText texto a ser reduzido, codificado através de um inteiro
 * @param textSize tamanho do texto
 * @param fileName nome do arquivo onde são gravadas as regras geradas a cada nível
 * @param level nível atual
 * @param mod tamanho das tuplas do texto
 * @param header contém as informações da gramática, quantidade de níveis, quantidade de regras por nível
 * @param sigma armazena o tamanho do alfabeto a ser usado nesse nível (é a quantidade de regras do nível anterior.)
 */
void encode(unsigned char *text0, uint32_t *uText, int32_t textSize, char *fileName, int level, int mod, vector<uint32_t> &header, uint32_t sigma);

/**
 * @brief realiza a descompressão do texto, por meio da decodificação do texto nível a nível.
 * 
 * @param text texto a ser decodificado
 * @param textSize tamanho do texto no nível atual
 * @param level nível atual
 * @param qtyLevels quantidade de níveis na gramática
 * @param fileName arquivo onde deve ser gravado o resultado da descompressão
 * @param rules0 armazena as regras do último nível da recursão, ou seja as regras que são originadas do texto plano
 * @param mod tamanho das tuplas do texto
 * @param header contém as informações da gramática, quantidade de níveis, quantidade de regras por nível
 */
void decode(uint32_t *text, int32_t textSize, int level, int qtyLevels, char *fileName, unsigned char *charRules, int mod, vector<uint32_t> &header);

/**
 * @brief Ler e decodifica arquivo nível por nível
 * 
 * @param compressedFile arquivo compactado
 * @param decompressedFile arquivo onde deve ser gravado o resultado da descompactação
 * @param header contém as informações da gramática, quantidade de níveis, quantidade de regras por nível
 * @param mod tamanho das tuplas do texto
 */
void decode(char *compressedFile, char *decompressedFile, vector<uint32_t> &header, int mod);

/**
 * @brief Cria e abre o arquivo para gravar as informações da gramática, e em seguida grava o símbolo inicial.
 * 
 * @param fileName nome do arquivo
 * @param startSymbol símbolo inicial
 * @param header contém as informações da gramática
 */
void storeStartSymbol(char *fileName, uint32_t *startSymbol, vector<uint32_t> &header);

/**
 * @brief Armazena regras geradas em cada nível
 * 
 * @param text0 contém as regras do último nível da recursão
 * @param uText contém as regras dos demais níveis
 * @param tuples array contendo os índices iniciais de cada tupla em ordem lexicográfica
 * @param rank classificação de cada tupla
 * @param nTuples quantidade de tuplas
 * @param fileName arquivo onde as regras devem ser salvas
 * @param level indica o nível em que as regras foram geradas.
 */
void storeRules(unsigned char *text0, uint32_t *uText, uint32_t *tuples, uint32_t *rank, int32_t nTuples, char *fileName, int mod, int level, int32_t qtyRules);

/**
 * @brief Decodifica o texto reduzido em determinado nível da gramática
 * 
 * @param uText conteúdo compactado lido do arquivo de entrado
 * @param xs texto reduzido a ser decodificado, ao final conterá o símbolo já decodficado
 * @param xsSize tamanho do símbolo a ser decodificado (ao final, conterá o tamanho do símbolo já decodificado)
 * @param level nível atual
 * @param start índice  em `uText` onde as regras do nível "level" começam
 * @param mod tamanho das tuplas do texto
 */
void decodeSymbol(uint32_t* uText, uint32_t *&xs, int32_t &xsSize, int level, int32_t start, int mod);

/**
 * @brief abre o arquivo de saída e grava o texto decodificado
 * 
 * @param text texto decodificado
 * @param textSize tamanho do texto
 * @param fileName arquivo de saída
 * @param charRules regras do último nível da recursão
 * @param mod tamanho das tuplas do texto
 */
void saveDecodedText(uint32_t *xs, int32_t xsSize, char *fileName, unsigned char *charRules, int module);


#endif