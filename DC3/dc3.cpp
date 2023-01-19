#include "dc3.hpp"
#include <iostream>
#include <math.h>
#include <cstdlib>

using namespace std;

const int dolar = 36;
int sentinel_position_on_reduced_str = INT_MAX; //marca a posição do sentinela na string reduzida

template <typename T>
void print(T v[], int n){
    cout << *(v);
    for(int i=1; i < n ; i++) cout  << ", " << *(v+i);
    cout << endl;
}

int * dc3(char *text, int ts) {
    int sa12_size = ts - ceil((double)ts/module);

    int *sa12 = orderSA12(text, ts, sa12_size);
    int *sa0 = orderSA0(text, sa12, ts, sa12_size);
    return merge(text, sa12, sa0, ts, sa12_size, ceil((double)ts/module));
}

int * orderSA12(char *text, int ts, int sa12_size) {
    int sa12[sa12_size];

    for(int i=0,j=1; j < ts; j++) {
        if(j % module != 0) sa12[i++] = j;
    }

    int *sa12_sorted = (int*) calloc(sa12_size, sizeof(int));
    copy(sa12, sa12+sa12_size, sa12_sorted);
    sa12_sorted = radix_sort(text, sa12_sorted, sa12_size, ts, module);
    int *rank = lex_names(text,sa12_sorted, sa12_size, ts);

    //significa que há repetições de lex-names e portanto é preciso chamar DC3 recursivamente
    if(rank[ts] == -1) {
        char * reduced_str = createReducedStr(sa12, rank, sa12_size);
        int * reduced_sa = dc3(reduced_str, sa12_size+1);
        sa12_sorted = mapReducedStringToOriginalStr(reduced_sa, sa12, sa12_size);
        
        sentinel_position_on_reduced_str = INT_MAX;
        //free(reduced_str); // TODO
        //free(reduced_str_sa);
    }

    free(rank);
    return sa12_sorted;
}

int  * orderSA0(char * text, int *sa12, int ts, int sa12_size) {
    int sa0_size = ts - sa12_size;
    int *sa0 = (int*) calloc(sa0_size, sizeof(int));
    int i=0;

    //O último elemento do texto está em SA0, e já sabemos sua ordem, pois é aquele com menor ordem lexicográfica.
    if( (ts -1) % 3 == 0 )sa0[i++] = ts-1;

    //Sabemos a ordem relativa de SA0 (comparado à SA12), pois o sufixo que segue SA0[j] está em SA12, que já foi ordenado.
    for(int j=0; j < sa12_size; j++) {
        if(sa12[j] % 3 == 1) sa0[i++] = sa12[j]-1;
    }

    //Ordenamos SA0 olhando apenas para o primeiro caractere. Radix Sort é estável, por isso não perdemos o trabalho anterior.
    return radix_sort(text, sa0, sa0_size, ts, 1);
}

int * merge(char *text, int *sa12, int *sa0, int ts, int sa12_size, int sa0_size) {
    //trata-se de uma string reduzida, o último elemento do texto é o sentinela, não queremos considerar ele durante o merge
    if(sentinel_position_on_reduced_str < INT_MAX){
        sa12_size--;
        ts--;
    }

    int *sa = (int*) calloc(ts, sizeof(int));
    char *inverse_sa12 = constructInverseArray(sa12, sa12_size);
    int i = 0, j = 0, k = 0;

    while(i < sa12_size && j < sa0_size) {
        sa[k] = lessThan(text, sa12[i], sa0[j], i, j);
        if(sa[k] == -1) {
            //Os sufixos que seguem os atuais estão ambos em SA12. Usamos a matriz inversa para saber quem aparece primeiro.
            if(sa12[i] % 3 == 1) sa[k] = lessThan(inverse_sa12, sa12[i]+1, sa0[j]+1, i, j) - 1;
            else {//Os sufixos que seguem os atuais estão separados
                sa[k] = lessThan(text, sa12[i]+1, sa0[j]+1, i, j) - 1;
                //Os elementos possuem a primeira letra igual, precisamos deslocar o array em 2 posições e usar a matriz inversa
                if(sa[k] < 0) sa[k] = lessThan(inverse_sa12, sa12[i]+2, sa0[j]+2, i, j) - 2;
            }
        }
        k++;
    }

    for(; i < sa12_size; i++) sa[k++] = sa12[i];
    for(; j < sa0_size; j++) sa[k++] = sa0[j];
    
    free(inverse_sa12);
    return sa;
}

int * radix_sort(char *text, int *sa, int sa_size, int ts, int n_char) {
    int *saTemp = (int*) calloc(sa_size, sizeof(int)); //mantém os digitos ordenados com relação ao dº digito

    for(int d = n_char -1; d >=0; d--) {
        int *position = (int*) calloc(255, sizeof(int));

        for(int i=0; i < sa_size; i++) {
            if (sa[i] == sentinel_position_on_reduced_str) continue;//para string reduzida
            int chPosition = calculatesLetterPosition(text, sa[i], d, ts);
            position[chPosition+1]++;
        }

        //acrescenta na posição do próximo elemento, todos os outros que vieram antes dele
        int countDolar = position[dolar];
        position[dolar] = 0;
        for(int j = 1; j < 256; j++) {
            if(j == dolar || j == sentinel_position_on_reduced_str)continue;
            if(j < dolar) position[j] += position[j-1]  + countDolar;
            else position[j] += position[j-1];
        }

        //Cria um array temporário responsável por armazenar a ordem relativa ao digito d dos elementos de SA
        if(sentinel_position_on_reduced_str < INT_MAX) saTemp[sa_size-1] = sentinel_position_on_reduced_str;

        for(int i = 0; i < sa_size; i++) {
            if (sa[i] == sentinel_position_on_reduced_str) continue;
            int chPosition = calculatesLetterPosition(text, sa[i], d, ts);
            int index = position[chPosition]++;
            saTemp[index] = sa[i];
        }

        for(int i=0; i<sa_size; i++) sa[i] = saTemp[i];
        free(position);
    }

    return sa;
}

int * lex_names(char *text, int *sa, int sa_size, int ts) {
    int *rank = (int*) calloc(ts+1, sizeof(int)); 

    rank[0] = 0; //último elemento é o $, já sabemos sua classificação

    for(int i=1; i < sa_size; i++) {
        if(i == sentinel_position_on_reduced_str)continue;
        char c1[module+1], c2[module+1];
        c1[module] = '\0';
        c2[module] = '\0';

        int s1 = (sa[i-1]+module < ts) ? module: ts - sa[i-1]; 
        int s2 = (sa[i]+module < ts) ? module: ts - sa[i];
        memcpy(c1, text + sa[i-1], s1);
        memcpy(c2, text + sa[i], s2);
        
        if(strcmp(c1,c2) == 0) {
            rank[ts] = -1;
            rank[sa[i]] = rank[sa[i-1]];
        } else {
            rank[sa[i]] = rank[sa[i-1]]+1;
        }
    }
    return rank;
}

char * createReducedStr(int *sa12, int *rank, int sa12_size) {
    char *u = (char*) calloc(sa12_size+1, sizeof(char));
    int m = ceil(sa12_size/2);
    sentinel_position_on_reduced_str = m;
    u[m] = '#';

    for(int i=0, f=0, s=m+1; i < sa12_size; i++){
        if(sa12[i] % module == 2 && f < m) {
            u[f] = rank[sa12[i]] + '0';
            f++;
        } else {
            u[s] = rank[sa12[i]] + '0';
            s++;
        }
    }
    return u;
}

int * mapReducedStringToOriginalStr(int *reduced_sa, int * sa12, int sa12_size) {
    int index_in_sa12;
    int *sa = (int*) calloc(sa12_size, sizeof(int));

    for(int i =0; i < sa12_size; i++) {
        if(reduced_sa[i] < sentinel_position_on_reduced_str) {
            index_in_sa12 = (2*reduced_sa[i])+1;
            sa[i] = sa12[index_in_sa12];
        } else {
            index_in_sa12 = 2 * (reduced_sa[i] - sentinel_position_on_reduced_str - 1);
            sa[i] = sa12[index_in_sa12];
        }
    }
    return sa;
}

char * constructInverseArray(int *sa, int sa_size) {
    char * inverse = (char*) calloc(sa_size, sizeof(char));
    for(int i=0; i < sa_size; i++) inverse[sa[i]] = i + '0';
    return inverse;
}

int lessThan(char *text, int a, int b, int &i, int &j) {
    if(text[a] < text[b]) {
        i++;
        return a;
    } else if (text[a] > text[b] ) {
        j++;
        return b;
    }
    return -1;

}

int calculatesLetterPosition(char *text, int indexText, int shift, int ts) {
    if((indexText < sentinel_position_on_reduced_str &&  indexText + shift >= sentinel_position_on_reduced_str) || (indexText + shift >= ts))
        return dolar;
    return text[indexText + shift];
}