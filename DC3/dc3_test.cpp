#include <gtest/gtest.h>
#include "gmock/gmock.h"
#include <cstdlib>
#include <iostream>
#include "dc3.hpp"

using namespace std;

string text = "banaananaanana$";
int expected_sa[]   = {14, 13, 8, 3, 11, 6, 1, 9, 4, 0, 12, 7, 2, 10, 5};
int expected_sa12[] = {14, 13, 8, 11, 1, 4, 7, 2, 10, 5};
int expected_sa0[]  = {3, 6, 9, 0, 12};
int text_size = text.size(), sa12_size = text_size - ceil(text_size/3), sa0_size = text_size/3;

TEST(DC3, receive_a_text_and_its_length_and_return_sa) {
    int *sa = dc3(&text[0], text_size);

    EXPECT_EQ(memcmp(expected_sa, sa, text_size * sizeof(int)), 0);
    free(sa);
}

TEST(DC3, receive_a_text_returns_sorted_array_sa12) {
    int *sa12 = orderSA12(&text[0], text_size, sa12_size);

    EXPECT_EQ(memcmp(expected_sa12, sa12, sa12_size * sizeof(int)), 0);
    free(sa12);
}

TEST(DC3, receive_a_text_and_sa12_and_returns_sorted_array_sa0) {
    int *sa0 = orderSA0(&text[0], expected_sa12, text_size, sa12_size);

    EXPECT_EQ(memcmp(expected_sa0, sa0, sa0_size * sizeof(int)), 0);
    free(sa0);
}

TEST(DC3, receive_sa12_and_sa0_and_merges_the_suffixes) {
    int *sa = merge(&text[0], expected_sa12, expected_sa0, text_size, sa12_size, sa0_size);

    EXPECT_EQ(memcmp(expected_sa, sa, text_size * sizeof(int)), 0);
    free(sa);
}

TEST(DC3, receive_sa12_and_returns_the_array_sorted_based_on_the_first_3_characters) {
    int exepected[] = {14, 13, 8, 1, 4, 11, 2, 7, 5, 10};
    int sa12[] = {1, 2, 4, 5, 7, 8, 10, 11, 13, 14};
    int *result = radix_sort(&text[0], sa12, sa12_size, text_size, 3);

    EXPECT_EQ(memcmp(exepected, result, sa12_size * sizeof(int)), 0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}