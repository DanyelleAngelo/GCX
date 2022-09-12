#include <iostream>
#include <vector>
#include <limits>

#include "radix_sort.hpp"

using namespace std;

int freq[valid_ch];
int freq_pred[valid_ch];
int iter[valid_ch]; //marca a iteração que um elemento foi computado (elimina a necessidade de esvaziar o vetor)

int get_freq(int i) {
    return freq[i];
}

int get_predecesor(int i) {
    return freq_pred[i];
}

void print_vector(vector<char*> ch){
    int i;
    for(i=0; i < ch.size()-1; i++) {
        cout << ch[i] << ", ";
    }
    cout << ch[i] << endl;
}

void fill_vectors() {
    fill(freq, freq+valid_ch, 0);
    fill(freq_pred, freq_pred+valid_ch, 0);
    fill(iter, iter+valid_ch, -1);
}

int get_pos(char ch){
    //posição 0 reservado para espaço
    if (ch >= 48 && ch <= 57) {
        return int(ch) - 46;
    } else if (ch >= 65 && ch <= 90) {
        return int(ch) - 53;
    } else if (ch >= 97 && ch <= 122) {
        return int(ch) - 85;
    } else if (ch == ' ') {
        return 1;
    } else if (ch == 36) {
        return 0;
    }
    return INT_MIN;
}

void calculate_frequency(vector<char*> ch, int ith, int n_words) {
    for (int i=0; i < n_words; i++) {
        int c = strlen(ch[i]) <= ith ? ith - strlen(ch[i]) : ith;
        if(c< strlen(ch[i])){
            int pos_ch= get_pos(ch[i][c]);
            freq[pos_ch]++;
            iter[pos_ch] = ith;
        }

    }
}

void calculate_frequency_of_predecessors(int ith) {
    freq_pred[0] = 0; //default
    int prev = 0;

    //TODO: otimização, interromper laço quando todos os caracteres da rodada tiverem sido
    for(int i=1; i < valid_ch; i++) {
        if (iter[i] == ith) { //the character corresponding to the i-th position was analyzed in the round
            freq_pred[i] = freq_pred[prev] + freq[prev];
            prev = i;
        }
    }
}

void sort_by_frequency_in_the_round(vector<char*> &ch, int ith, int n_words, vector<int>& sa) {
    vector<char*> copy = ch; //preserva a ordem inicial (é esses elementos que queremos ordenar)
    vector<int> copy_sa = sa;

    for(int j=0; j < n_words; j++) {
        int c = strlen(copy[j]) <= ith ? ith - strlen(copy[j]) : ith;
        if(c < strlen(copy[j])){
        int pos_in_freq = get_pos(copy[j][c]);//captura a posição para saber onde devemos olhar
        int pos_finaly = freq_pred[pos_in_freq];//captura a posicao em que o elemento deve ser salvo
        ch[pos_finaly] = copy[j]; //salva a palavra na posicao correta
        sa[pos_finaly] = copy_sa[j];
        freq_pred[pos_in_freq]++; //incrementa o array de posições para que o próximo elemento fique no local correto
        }

    }
}

void radix_sort(vector<char*> &ch, int rounds, vector<int> &sa) {
    int n_words = ch.size();

    for(int i=rounds -1; i >= 0; i--) {
        fill_vectors();
        calculate_frequency(ch, i, n_words);
        calculate_frequency_of_predecessors(i);
        sort_by_frequency_in_the_round(ch, i, n_words, sa);
    }
}