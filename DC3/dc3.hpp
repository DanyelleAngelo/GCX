#ifndef DC3_H
#define DC3_H

const int module = 3;

/**
 * @brief computa o array ordenado de sufixos de um texto
 * 
 * @param text texto de entrada
 * @param ts tamanho do texto
 * @return array contendo os sufixos do texto de forma ordenada
 */
int * dc3(char *text, int ts);

/**
 * @brief calcula o array ordenado dos sufixos do texto que iniciam na posição `i` tal que i % 3 ≠ 0 
 * 
 * @param text texto
 * @param ts tamanho do texto
 * @param size_sa12 quantidade de sufixos que atendem a regra acima, ou seja ts - ceil(ts/3)
 * @return array ordenado dos sufixos iniciados na posição i % 3 ≠ 0 
 */
int * orderSA12(char *text, int ts, int size_sa12);

/**
 * @brief calcula o array ordenado dos sufixos do texto que iniciam na posição `i` tal que i % 3 = 0
 * 
 * @param text texto
 * @param sa12 array de sufixos dos elementos que iniciam na posição `i` tal que i % 3 ≠ 0 
 * @param ts tamanho do texto
 * @param size_sa12 tamanho do array sa12
 * @return array ordenado dos sufixos iniciados na posição i % 3 = 0 
 */
int * orderSA0(char *text, int *sa12, int ts, int size_sa12);

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
int * merge(char *text, int *sa12, int *sa0, int ts, int sa12_size, int sa0_size);

/**
 * @brief usa ordenação estável para ordenar o array de sufixos com base nos n_char primeiros elementos. 
 * 
 * @param t array contendo o texto
 * @param sa índices dos elementos a serem ordenados
 * @param sa_size número de elementos em sa
 * @param ts tamanho do texto
 * @param n_char quantidade de elementos a serem considerados na ordenação
 * @return array de sufixos ordenados com base nos n_char primeiros caracteres.
 */
int * radix_sort(char *text, int *sa, int sa_size, int ts, int n_char);

/**
 * @brief Pecorre cada um dos elementos no array de sufixos e atribuí um lex-name para ele de acordo som sua ordem. Se houverem repetições, o último elemento da resposta será definido como -1.
 * 
 * @param text texto que gera o array de sufixos
 * @param sa array de sufixos ordenado de acordo com d letras
 * @param size_sa tamanho do array de sufixos
 * @param ts tamanho do texto
 * @return array de tamanho sa_size+1 contendo a classificação dos sufixos e um indicador de repetições.
 */
int * lex_names(char *text, int *sa, int size_sa, int ts);

/**
 * @brief Cria uma string reduzida, da seguinte forma: o elemento central dessa string é um sentinela que não ocorre em nenhum outro lugar. Para preencher as demais posições, pecorremos todos os elementos em SA12, e para cada elemento, se o resto da divisão por 3 desse elemento for igual à 2, o mesmo é adicionado no ínicio da primeira metade dessa string, casso contrário o elemento é inserido no ínicio da segunda metade.
 * 
 * @param sa12 array com os índices do texto com modulo 3 igual à 2 ou 1;
 * @param rank classificação de cada tripla em sa12;
 * @param size_sa12 tamanho do array sa12;
 * @return char* contendo a string com as características descritas acima. 
 */
char * createReducedStr(int *sa12, int *rank, int size_sa12);

/**
 * @brief mapeia os elementos da string reduzida (string composta pelos ranks de cada sufixo) para o seu sufixo original.
 * 
 * @param reduced_sa array de sufixos da string reduzida
 * @param sa12 array de sufixos da recursão anterior
 * @param sa12_size tamanho de sa_12
 * @return array contendo o mapeamnto de reduced_sa para sa12: ou seja sa12 ordenado
 */
int * mapReducedStringToOriginalStr(int *reduced_sa, int * sa12,int sa12_size);

/**
 * @brief constroí o array inverso de um array a. Ou seja criar um array b, onde b[a[i]] = i
 * 
 * @param sa array de sufixos
 * @param sa_size tamanho do array de sufixos
 * @return char* inversa do array de sufixos
 */
char * constructInverseArray(int *sa, int sa_size);

/**
 * @brief Auxilia a função de merge a decidir qual elemento é menor: SA12[i] ou SA0[j]
 * 
 * @param text texto com os elementos a serem comparados. Pode ser o texto que originou SA12 e SA0, ou a matriz de sufixos inversa de SA12 (caso em que os elementos analisados estão ambos em SA12).
 * @param a SA12[i]+k (tamanho do deslocamento, pode ser 1 ou 2)
 * @param b SA0[j]+k (tamanho do deslocamento, pode ser 1 ou 2)
 * @param i marca a posição do ponteiro em SA12
 * @param j marca a posição do ponteiro em SA0
 * @return `a` se SA12[i]+k for menor que SA0[j] + k, b caso maior, e -1 se os elementos forem iguais.
 */
int lessThan(char *text, int a, int b, int &i, int &j);

/**
 * @brief função auxiliar do radix sort. Calcula a posição de um sufixo do texto deslocado em shift posições (importante, pois em alguns momentos esse deslocamento pode romper o tamanho do texto).
 * 
 * @param text texto
 * @param indexText posição inicial do sufixo em SA
 * @param shift número de vezes que o sufixo deve ser deslocado
 * @param ts tamanho do texto
 * @return posição do primeiro símbolo do sufixo deslocado.
 */
int calculatesLetterPosition(char *text, int indexText, int shift, int ts);

#endif