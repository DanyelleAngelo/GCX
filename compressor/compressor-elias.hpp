#ifndef COMPRESSOR_ELIAS_H
#define COMPRESSOR_ELIAS_H

#include <iostream>
#include <vector>
#include <sdsl/int_vector.hpp>
using namespace std;
using namespace sdsl;


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
 * @param decoded vetor de inteiros usado para armazenar as regras (e cabeçalho, incluindo símbolo inicial) da gramática 
 * @param levels quantidade de níveis na gramática
 */
void readCompressedFile(char *fileName, int_vector<32> &decoded, int &levels);

/**
 * @brief realiza a compactação do texto por meio de sucessivas chamadas recursivas, onde são geradas regras (por meio da ordenação do texto) que codificam o texto.
 * 
 * @param text0 texto plano
 * @param uText texto a ser reduzido, codificado através de um inteiro
 * @param textSize tamanho do texto
 * @param level nível atual
 * @param module tamanho das regras do texto
 * @param grammarInfo contém as informações da gramática, quantidade de níveis, quantidade de regras por nível
 */
void encode(unsigned char *text0, uint32_t *uText, int32_t textSize, int level, int module, vector<uint32_t> &grammarInfo);

/**
 * @brief realiza a descompressão do texto, por meio da decodificação do texto nível a nível.
 * 
 * @param decoded informações do texto compactado, incluindo cabeçalho e símbolo inicial
 * @param level nível atual
 * @param qtyLevels quantidade de níveis na gramática
 * @param module tamanho das tuplas do texto
 * @param fileName nome do arquivo onde deve ser salvo o texto decodificado
 */
void decode(int_vector<32> decoded, int level, int qtyLevels, int module, char *fileName);

/**
 * @brief Verifica as regras geradas em cada nível, e grava em um vetor apenas as regras únicas
 * 
 * @param text0 texto de entrada, usado no último nível da recursão
 * @param uText texto representado em inteiro que representa a regra que deve ser armazenada 
 * @param tuples array contendo os índices iniciais de cada trinca em ordem
 * @param rank classificação de cada trinca
 * @param module tamanho de cada regra
 * @param nTuples quantidade de trincas
 * @param level indica o nível em que as regras foram geradas, útil para saber se o texto deve ser armazenado em um char ou inteiro.
 * @param grammarInfo contém as informações da gramática, quantidade de níveis, quantidade de regras por nível
 */
void createRules(unsigned char *text0, uint32_t *uText, uint32_t *tuples, uint32_t *rank, int module, int32_t nTuples, int level, vector<uint32_t> &grammarInfo);

/**
 * @brief Codifica a gramática no formato Elias Gama e salva em um arquivo de texto usando as funções da SDSL
 * 
 * @param fileName nome do arquivo que conterá o texto compactado
 * @param textSize tamanho do texto compactado na sua forma plana (sem elias gama)
 * @param module tamanho de cada regra
 * @param grammarInfo texto compactado (incluindo informações de níveis)
 */
void encodeTextWithEliasAndSave(char *fileName, uint32_t textSize, int module, vector<uint32_t> &grammarInfo);

/**
 * @brief Decodifica o texto reduzido em determinado nível da gramática
 * 
 * @param decoded texto compactado lido do arquivo de entrado
 * @param symbol símbolo a ser decodificado (ao final conterá o símbolo já decodficado)
 * @param xsSize tamanho do símbolo a ser decodificado (ao final, conterá o tamanho do símbolo já decodificado)
 * @param level nível atual
 * @param start índice onde as regras do nível "l" começam
 * @param module tamanho das tuplas do texto
 */
void decodeSymbol(int_vector<32> decoded, uint32_t *&symbol, int32_t &xsSize, int level, int startRules, int module);

/**
 * @brief abre o arquivo de saída e grava o texto decodificado
 * 
 * @param decoded texto codificado, usado para obter informações da gramática
 * @param symbol texto decodificado
 * @param xsSize tamanho do texto decodificado
 * @param module tamanho das tuplas do texto
 * @param fileName arquivo de saída
 */
void saveDecodedText(int_vector<32> decoded, uint32_t *symbol, int xsSize, int module, char *fileName);

#endif
