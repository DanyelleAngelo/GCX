#ifndef DC3_H
#define DC3_H

/**
 * @brief computa o array ordenado de sufixos de um texto
 * 
 * @param text texto de entrada
 * @param ts tamanho do texto
 * @return array contendo os sufixos do texto de forma ordenada
 */
void dc3(long int *text, long int *sa, long int ts, long int level, int module, long int sigma);

/**
 * @brief calcula o array ordenado dos sufixos do texto que iniciam na posição `i` tal que i % 3 ≠ 0 
 * 
 * @param text texto
 * @param ts tamanho do texto
 * @param sa12_size quantidade de sufixos que atendem a regra acima, ou seja ts - ceil(ts/3)
 * @return array ordenado dos sufixos iniciados na posição i % 3 ≠ 0 
 */
void orderSA12(long int *text, long int ts,long int *sa12, long int sa12_size, long int level, int module, long int sigma);

/**
 * @brief calcula o array ordenado dos sufixos do texto que iniciam na posição `i` tal que i % 3 = 0
 * 
 * @param text texto
 * @param sa12 array de sufixos dos elementos que iniciam na posição `i` tal que i % 3 ≠ 0 
 * @param ts tamanho do texto
 * @param size_sa12 tamanho do array sa12
 * @return array ordenado dos sufixos iniciados na posição i % 3 = 0 
 */
void orderSA0(long int *text, long int *sa12, long int ts,long int *sa0, long int sa0_size, int module, long int level, long int sigma);

/**
 * @brief Mescla o array de sufixos que contém os elementos iniciados em i%3 = 0 com o array de sufixos dos elementos iniciados em i%3≠0. A explicação do processo pode ser encontrada em .rules_for_the_merge.txt
 * 
 * @param text texto que origina SA12 e SA0
 * @param sa12 array de sufixos dos elementos que iniciam na posição `i` tal que i % 3 ≠ 0 
 * @param sa0 array de sufixos dos elementos que iniciam na posição `i` tal que i % 3 = 0 
 * @param ts  tamanho do texto
 * @param sa12_size tamanho de SA12
 * @param sa0_size tamanho de SA0
 * @return array com o tamanho do texto, contendo todos os sufixos de forma ordenada
 */
void merge(long int *sa, long int *text,long int *sa12,long int *sa0,  long int sa12_size, long int sa0_size, int module, long int level, long int textSize);

/**
 * @brief usa ordenação estável para ordenar o array de sufixos com base nos n_char primeiros elementos. 
 * 
 * @param t array contendo o texto
 * @param sa array contendo os índices dos elementos que devem ser ordenados
 * @param sa_size número de elementos em sa
 * @param ts tamanho do texto
 * @param n_char quantidade de elementos a serem considerados na ordenação
 */
void radix_sort(long int *text, long int *sa, long int sa_size, int module, long int sigma);

/**
 * @brief Pecorre cada um dos elementos no array de sufixos e atribuí um lex-name para ele de acordo som sua ordem. Se houverem repetições, o último elemento da resposta será definido como -1.
 * 
 * @param text texto que gera o array de sufixos
 * @param sa array de sufixos ordenado de acordo com d letras
 * @param size_sa tamanho do array de sufixos
 * @param ts tamanho do texto
 * @return array de tamanho sa_size+1 contendo a classificação dos sufixos e um indicador de repetições.
 */
void lex_names(long int *text, long int *sa, long int *rank, long int size_sa, long int &sigma, int module);

/**
 * @brief Cria uma string reduzida, da seguinte forma: o elemento central dessa string é um sentinela que não ocorre em nenhum outro lugar. Para preencher as demais posições, pecorremos todos os elementos em SA12, e para cada elemento, se o resto da divisão por 3 desse elemento for igual à 1, o mesmo é adicionado no ínicio da primeira metade dessa string, casso contrário o elemento é inserido no ínicio da segunda metade.
 * 
 * @param sa12 array com os índices do texto com modulo 3 igual à 2 ou 1;
 * @param rank classificação de cada tripla em sa12;
 * @param u string reduzida de SA12
 * @param sa12_size tamanho do array sa12;
 */
void createStrReduced(long int *str12, long int *rank, long int *u, long int sa12_size, long int strSize, int module);

/**
 * @brief mapeia os elementos da string reduzida (string composta pelos ranks de cada sufixo) para o seu sufixo original.
 * 
 * @param reduced_sa array de sufixos da string reduzida
 * @param sa12 array de sufixos da recursão anterior, que será ordenado de acordo com a string reduzida
 * @param sa12_size tamanho de sa_12
 */
void mapReducedStringToOriginalStr(long int *saR, long int * str12,long int *sa12, long int sa12_size, long int strSize, long int level);

/**
 * @brief constroí o array inverso de um array a. Ou seja criar um array b, onde b[a[i]] = i
 * 
 * @param sa array de sufixos
 * @param sa_size tamanho do array de sufixos
 * @return char* inversa do array de sufixos
 */
void constructInverseArray(long int *sa, long int *&inverse, long int text_size, long int sa_size);

long int getSizeOfReducedString(long int sa12_size, long int module);

#endif