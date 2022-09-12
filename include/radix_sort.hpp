#ifndef RADIX_SORT
#define RADIX_SORT

#include <iostream>
#include <vector>

using namespace std;

const int valid_ch = 38; // alphanumeric characters + with space. Not case sensitive.

int get_freq(int i);
int get_predecesor(int i);
void fill_vectors();

/**
 * @brief Get the character position in the frequency and order vectors.
 * 
 * @param ch: alphanumeric character. ch[ith word][round]
 * @return int matches the lexicographical position of the character.
 */
int get_pos(char ch);

/**
 * @brief counts the frequency of characters in the round
 * 
 * @param ch word vector
 * @param ith i-th character to be counted
 * @param n_words number of words
 */
void calculate_frequency(vector<char*> ch, int ith, int n_words); 

/**
 * @brief calculates the position of the first occurrenche of the character
 * 
 * @param ith i-th character to be analyzed
 */
void calculate_frequency_of_predecessors(int ith);

/**
 * @brief uses the character frequency of a round to partially sort the vector
 * 
 * @param ch word vector
 * @param ith i-th charecter to be ordered
 * @param n_words number of words
 */
void sort_by_frequency_in_the_round(vector<char*> &ch, int ith, int n_words, vector<int> &sa);

/**
 * @brief sort a vector of words
 * 
 * @param ch: array of strings to be sorted
 * @param rounds: number of rounds needed to sort the vector (largest string size)
 */
void radix_sort(vector<char*> &ch, int rounds, vector<int> &sa);

#endif