#include <iostream>
#include <vector>
#include <gtest/gtest.h>
#include <radix_sort.hpp>

using namespace std;

vector<char*> ch = {(char*)"l$E8", (char*)"i63R",
                    (char*)"Y7 z", (char*)"Dy7$",
                    (char*)"zR72",  (char*)"voEw"};

vector<int> sa(6);//apenas para auxliar

int words = ch.size();
int positions[] = {1, 5, 9, 16};

TEST(radix_sort, calculate_frequency_in_round_2) {
    int expected[] = {1, 1, 2, 2}; //space, 3, 7, e
    fill_vectors();

    calculate_frequency(ch, 2, words);

    for(int i=0; i < sizeof(positions)/sizeof(positions[0]); i++){
        int p = positions[i];
        EXPECT_EQ(expected[i], get_freq(p));
    }
}

TEST(radix_sort, calculate_frequency_of_predecessors_in_round_2) {
    int expected[] = {0, 1, 2, 4}; //space, 3, 7, e
    fill_vectors();

    calculate_frequency(ch, 2, words);
    calculate_frequency_of_predecessors(2);

    for(int i=0; i < sizeof(positions)/sizeof(positions[0]); i++){
        int p = positions[i];
        EXPECT_EQ(expected[i], get_predecesor(p));
    }
}

TEST(radix_sort, DISABLED_sort_by_frequency_in_the_round_2) {
    vector<char*> expected = {(char*)"Y7 z", (char*)"i63R",
                             (char*)"Dy7$", (char*)"zR72",
                              (char*)"l$E8", (char*)"voEw"};
    fill_vectors();

    calculate_frequency(ch, 3, words);
    calculate_frequency_of_predecessors(3);
    sort_by_frequency_in_the_round(ch, 3, words, sa);
    
    fill_vectors();
    calculate_frequency(ch, 2, words);
    calculate_frequency_of_predecessors(2);
    sort_by_frequency_in_the_round(ch, 2, words, sa);

    for(int i=0; i < ch.size(); i++){
        EXPECT_TRUE(expected[i] == ch[i]);
    }
}

TEST(radix_sort, radix_sort) {
    vector<char*> expected = {(char*)"Dy7$", (char*)"i63R",
                              (char*)"l$E8", (char*)"voEw",
                              (char*)"Y7 z", (char*)"zR72" };
    
    radix_sort(ch, 4, sa);
    for(int i=0; i < ch.size(); i++)cout << ch[i] << " ";
    cout <<endl;
    for(int i=0; i < ch.size(); i++){
        EXPECT_TRUE(expected[i] == ch[i]);
    }
}