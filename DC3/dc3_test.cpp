#include <gtest/gtest.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <math.h>
#include "dc3.hpp"

using namespace std;
int sigma = 257;

void readFile(char *fileName, unsigned char *&text, long int *&uText, long int *&sa, long int &textSize, int module) {
    FILE*  file= fopen(fileName, "r");
    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    textSize = ftell(file)+1;

    sa = (long int*)calloc(textSize, sizeof(long int));
    text = (unsigned char*)calloc(textSize+module, sizeof(unsigned char));

    fseek(file, 0, SEEK_SET);
    fread(text, sizeof(unsigned char), textSize-1, file);
    fclose(file);

    uText = (long int*)calloc(textSize+module, sizeof(long int));
    for(int i=0; i < textSize; i++)uText[i] = (long int)text[i];
}

TEST(DC3, receive_a_small_text_and_return_suffix_array) {
    int module =3;
    unsigned char *text;
    long int *uText, *sa, textSize;
    char fileName[] = "small_text.txt";

    readFile(fileName, text, uText, sa, textSize, module);
    dc3(uText, sa, textSize, 0, module, sigma);

    ASSERT_EQ(sa[0], textSize-1) << " o menor do sufixo deveria ser T[0] = " <<  textSize -1 <<endl;
    
    for(long int i=1, j=2; j < textSize; i++, j++) {
        long int n = (sa[i] < sa[j]) ? textSize-sa[j] : textSize -sa[i];
        long int lex = memcmp(text+sa[i], text+sa[j], n);
        ASSERT_LE(lex,0);
    }

    free(sa);
    free(text);
    free(uText);
}

TEST(DC3, receive_a_large_text_and_return_suffix_array) {
    int module =3;
    unsigned char *text;
    long int *uText, *sa, textSize;
    char fileName[] = "large_text.txt";

    readFile(fileName, text, uText, sa, textSize, module);
    dc3(uText, sa, textSize, 0, module, sigma);
    
    ASSERT_EQ(sa[0], textSize-1);
    for(long int i=1, j=2; j < textSize; i++, j++) {
        long int n = (sa[i] < sa[j]) ? textSize-sa[j] : textSize -sa[i];
        long int lex = memcmp(text+sa[i], text+sa[j], n-1);
        ASSERT_LE(lex,0);
    }

    free(sa);
    free(text);
    free(uText);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
   ::testing::GTEST_FLAG(filter) = "*receive_a_large_text_and_return_suffix_array";
    return RUN_ALL_TESTS();
}
