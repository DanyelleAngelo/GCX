#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include "dc3.hpp"

using namespace std;

char         text[] = {'b','a','n', 'a', 'a', 'n', 'a', 'n', 'a', 'a', 'n', 'a','n', 'a', 1, 1};
int   expected_sa[] = {15, 14, 13, 8, 3, 11, 6, 1, 9, 4, 0, 12, 7, 2, 10, 5};
int expected_rank[] = {-1,3,4,-1,3,5,-1,4,2,-1,5,3,-1,1,0,-1};
int expected_sa12[] = {14, 13, 8, 11, 1, 4, 7, 2, 10, 5};
int  expected_sa0[] = {15, 3, 6, 9, 0, 12};
int text_size = 16, sa0_size = ceil((double)text_size/3), sa12_size = text_size - sa0_size;


TEST(DC3, receive_a_text_and_its_length_and_return_sa) {
    int *sa = (int*)calloc(text_size, sizeof(int));
    dc3(text, sa, text_size,0);

    for(int i=0; i< text_size;i++)
	EXPECT_EQ(expected_sa[i], sa[i]) << " ocorreu um erro para o " << i << "-th elemento.\n";
    free(sa);
}

TEST(SA12, receive_a_text_returns_sorted_array_sa12) {
    int *sa12 = (int*) calloc(sa12_size, sizeof(int));

    orderSA12(text, text_size, sa12, sa12_size, 0);

    for(int i=0; i < sa12_size; i++)
        EXPECT_EQ(expected_sa12[i], sa12[i]) << " ocorreu um erro para o " << i << "-th elemento.\n";
    free(sa12);
}

TEST(SA0, receive_a_text_and_sa12_and_returns_sorted_array_sa0) {
    int *sa0 = (int*) calloc(sa0_size, sizeof(int));
    orderSA0(text, expected_sa12,text_size, sa0, sa0_size);

    EXPECT_EQ(memcmp(expected_sa0, sa0, sa0_size * sizeof(int)), 0);
    free(sa0);
}

TEST(DC3, receive_sa12_and_sa0_and_merges_the_suffixes) {
    int *sa = (int*)calloc(text_size, sizeof(int));

    merge(sa,text, expected_sa12, expected_sa0, sa12_size, sa0_size);
    for(int i =0; i < text_size; i++) {
        EXPECT_EQ(expected_sa[i], sa[i]) << " ocorreu um erro ao computar o " << i << "-th do array do sufixos.\n";
    }
    free(sa);
}

TEST(RADIX_SORT, receive_sa12_and_returns_the_array_sorted_based_on_the_first_3_characters) {
    int exepected[] = {14, 13, 8, 1, 4, 11, 2, 7, 5, 10};
    int sa12[] = {1, 2, 4, 5, 7, 8, 10, 11, 13, 14};
    radix_sort(text, sa12, sa12_size, text_size, 3);

    EXPECT_EQ(memcmp(exepected, sa12, sa12_size * sizeof(int)), 0);
}

TEST(RADIX_SORT, should_be_able_ordering_suffix_of_array) {
    char  word[] = {'b', 'a', 't', 'a', 't', 'a', 0};
    int     sa[] = {0, 1, 2, 3, 4, 5, 6};
    int exp_sa[] = {6, 5, 1, 3, 0, 4, 2};
    radix_sort(word, sa, 7, 7, 3);

    for(int i=0; i < 7; i++) {
        EXPECT_EQ(exp_sa[i], sa[i]);
    }

}

TEST(RADIX_SORT, should_be_able_ordering_reduced_string) {
    char  word[] = {'1', '4', '3', '0', 0, '5', '1', '3' };
    int     sa[] = {1, 2, 4, 5, 7};
    int exp_sa[] = {4, 7, 2, 1, 5};
    radix_sort(word, sa, 5, 8, 3);

    for(int i=0; i < 5; i++) {
        EXPECT_EQ(exp_sa[i], sa[i]);
    }

}

TEST(REDUCED_STR, should_be_able_create_reduced_string) {
    int      sa12[] = {1, 2, 4, 5, 7, 8, 10};
    int      rank[] = {-1, 1, 5, -1, 4, 1, -1, 3, 3, -1, 0}; //-1 são posições i%3 =0
    char  exp_sa_r[] = {'1', '4', '3', '0', 0, '5', '1', '3'};
    char response[8];
    createReducedStr(sa12, rank, response, 7);

    for(int i=0; i < 8; i++) {
        EXPECT_EQ(exp_sa_r[i], response[i]) << " ocorreu um erro para o " << i << "-th elemento";
    }
}

TEST(REDUCED_STR, should_be_able_make_mapping_sorted_reduced_str_to_sa) {
    //for reference: reduced_str[] = {'3','3','2', '1', 0, '5', '5', '4', '0' }
    int            sa[] = {1, 2, 4, 5, 7, 8, 10, 11};
    int r_str_ordered[] = {4, 8, 3, 2, 1, 0, 7, 6, 5};
    int   exp_mapping[] = {11, 10, 7, 4, 1, 8, 5, 2};


    mapReducedStringToOriginalStr(r_str_ordered, sa, sa, 8);

    for(int i=0; i < 8; i++) {
        EXPECT_EQ(exp_mapping[i], sa[i]);
    }
}

TEST(LEX_NAME, should_be_able_create_lex_name_for_sa12_sorted_and_return_true_for_tie) {
    int *rank = (int*)calloc(text_size, sizeof(int));

    bool repeat = lex_names(text, expected_sa12, rank, sa12_size);

    EXPECT_TRUE(repeat);
    for(int i=0; i < sa12_size; i++) {
        EXPECT_EQ(expected_rank[expected_sa12[i]], rank[expected_sa12[i]]) << " ocorreu um erro ao calcular o rank de "<< expected_sa12[i] << " que é o "  << i << "-th elemento em SA12\n";
    }
}

TEST(DC3, receive_a_large_text_with_repetitions_and_return_suffix_array) {
    FILE*  file= fopen("text-large.txt","r");
    fseek(file, 0, SEEK_END);
    int n = ftell(file)+2;

    int *sa = (int*)calloc(n, sizeof(int));
    char * text_large = new char[n];
    text_large[n-2] = 1;
    text_large[n-1] = 1;

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_SET);
    fread(text_large, 1, n-2, file);
    fclose(file);

    EXPECT_EQ(expected_sa[0], text_size-1);
    EXPECT_EQ(expected_sa[1], text_size-2);
    for(int i=2; i < text_size; i++) {
        EXPECT_LT(strcmp(&text[expected_sa[i-1]],&text[expected_sa[i]]),0);
    }

    free(sa);
    delete[] text_large;
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
   ::testing::GTEST_FLAG(filter) = "*";
    return RUN_ALL_TESTS();
}
