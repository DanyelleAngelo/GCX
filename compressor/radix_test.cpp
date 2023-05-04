#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <string>
#include "radix.hpp"

using namespace std; 
typedef unsigned char BYTE;
char *fileName;

class GrammarFixtureTest : public ::testing::Test{
    public:
        unsigned char *smallText;
        unsigned int * saSmallText;
        unsigned char *reducedStrExpected;
        unsigned char *expectedGrammar;
        unsigned int *sa;
        unsigned char *text;
        unsigned int *rank;
        size_t smallTextSize = 16;
        size_t saSmallTextSize = 6;
        size_t textSize;
        size_t saSize;

        virtual void SetUp() override{
            readText();
            mockForSmallText();
            mockForLargeText();
        }

        virtual void TearDown() override{
            free(rank);
            free(sa);
            delete [] text;
        }

    private:
        void readText() {
            FILE*  file= fopen(fileName,"r");
            fseek(file, 0, SEEK_END);
            textSize = ftell(file);
            size_t i = textSize, nSentries;
            if(textSize %3 == 1) nSentries =2;
            else if(textSize %3 ==2) nSentries=1;
            else nSentries =3;
            textSize += nSentries;
            text = new unsigned char[textSize];
            while(i < textSize) {
                text[i] =0;
                i++;
            }
            
            if(file == NULL) {
                cout << "An error occurred while opening the file" << endl;
                exit(EXIT_FAILURE);
            }

            fseek(file, 0, SEEK_SET);
            fread(text, 1, textSize-nSentries, file);

            fclose(file);
        }

        void mockForSmallText() {
            smallText = new unsigned char[smallTextSize] {'b','a','n', 'a', 'a', 'n', 'a', 'n', 'a', 'a','n', 'a', 'n', 'a', 1, 0};
            saSmallText = new unsigned[saSmallTextSize] {0, 3, 6, 9, 12, 15};
            reducedStrExpected = new unsigned char[saSmallTextSize]{'4', '2', '3', '3', '5', '1'};
        }

        void mockForLargeText() {
            rank = (unsigned int*) calloc(textSize, sizeof(unsigned int*));
            saSize = ceil((double)textSize/3);
            sa = (unsigned int*) malloc(saSize * sizeof(unsigned int*));
            for(int i=0, j=0; j < saSize; i+=3)sa[j++] = i;
        }

        template <typename T>
        void print(T v[], int n){
            cout << *(v);
            for(int i=1; i < n ; i++) cout  << ", " << *(v+i);
            cout << endl;
        } 
};

TEST_F(GrammarFixtureTest, receive_a_text_and_returns_an_array_of_sorted_triples) {
    radixSort(text, textSize, saSize, sa);

    for(int i=1; i < saSize; i++) {
        EXPECT_LE(memcmp(&text[sa[i-1]],&text[sa[i]], module),0) << " o sufixo iniciado em " << sa[i-1] << " é maior do que o sufixo iniciado em " << sa[i] << ". Com i = " << i<< endl;
    }
}

TEST_F(GrammarFixtureTest, receive_a_partially_sorted_suffix_array_and_create_its_lexName) {
    int lastRank = 1, i=0;
    
    radixSort(text, textSize, saSize, sa);
    lex_names(text, sa, rank, textSize, saSize);
    ASSERT_EQ(rank[sa[i++]], lastRank) << " o rank do último sentinela deve ser 1.";
    while(i < saSize) {
        if(memcmp(&text[sa[i-1]],&text[sa[i]], module) == 0) {
            ASSERT_EQ(rank[sa[i]], lastRank) << " as trincas são iguais, mas os ranks são diferentes.";
        } else {
            ASSERT_EQ(rank[sa[i]], ++lastRank) << " o rank do sufixo iniciado na posição " << sa[i] << " deveria ser igual ao rank do sufixo iniciado na posição " << sa[i-1] << " + 1 ( " << rank[sa[i-1]] + 1 << ").";
        }
        i++;
    }
}

TEST_F(GrammarFixtureTest, receive_a_rank_array_and_creates_grammar_rules) {
    char fileName[] = "grammar-test";
    int level = 0;
    unsigned int rankExample[smallTextSize];
    unsigned char expectedGramatic[] = {0,
                                         0, 0, 0, 
                                        'a',  1,   0, 
                                        'a', 'a', 'n',
                                        'a', 'n', 'a',
                                        'b', 'a', 'n',
                                        'n', 'a',  1};
    
    radixSort(smallText, smallTextSize, saSmallTextSize, saSmallText);
    int uniqueRules = lex_names(smallText, saSmallText, rankExample, smallTextSize, saSmallTextSize);
    createRules(smallText, saSmallText, rankExample, smallTextSize, saSmallTextSize, fileName, 6);

    ifstream file;
    file.open(fileName, ios::in | ios::binary);
    
    ASSERT_TRUE(file.is_open());
    //validar quantidade de regras no arquivo: quantidade de caracteres -2 (level e endl), dividido por 3
    

    file.close();
    remove(fileName);
}

TEST_F(GrammarFixtureTest, should_receive_text_suffixArray_and_rankOfSuffixes_and_creates_reducedStr){
    int level = 0;
    unsigned char reducedStr[saSmallTextSize];
    unsigned int rankExample[smallTextSize];
    
    radixSort(smallText, smallTextSize, saSmallTextSize, saSmallText);
    lex_names(smallText, saSmallText, rankExample, smallTextSize, saSmallTextSize);
    createReducedStr(smallText, saSmallText, rankExample, reducedStr, smallTextSize,saSmallTextSize, saSmallTextSize);
    
    ASSERT_EQ(memcmp(reducedStr, reducedStrExpected, saSmallTextSize),0) << " o conteúdo da string reduzida é diferente do esperado!";
}

TEST_F(GrammarFixtureTest, receives_a_small_text_and_generates_the_grammar_that_compresses_the_text){
    int level = 0;
    char fileName[] = "grammar-test";

    encode(smallText, smallTextSize, fileName, level);
}

TEST_F(GrammarFixtureTest, receives_a_compressed_text){
    int level = 0;
    char fileName[] = "grammar-test";

    readCompressedText(fileName);
}

int main(int argc, char **argv){
    ::testing::InitGoogleTest(&argc, argv);
    if(argc < 2) {
        cout << "The name of the file containing the text is missing." <<endl;
        exit(EXIT_FAILURE);
    }
    fileName = argv[1];
    
    testing::GTEST_FLAG(filter) = "*receives_a_small_text_and_generates_the_grammar_that_compresses_the_text";
    return RUN_ALL_TESTS();
}