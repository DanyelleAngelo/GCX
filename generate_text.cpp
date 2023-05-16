#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string.h>

using namespace std;

int main(int argc, char *argv[]) {
    if(argc < 4) {
        cout << "The is parameters is missing. Parameters requireds: \n1- word that is permuted; \n2- File for write text." << endl;
        exit(EXIT_FAILURE);
    }

    ofstream file;
    file.open(argv[3],ios::trunc);

    string word = string(argv[1]);
    int n = word.size(), i=0, l,r;
    int nPermutations = atoi(argv[2]);

    file.write(&word[0], n*sizeof(char));
    while(i < nPermutations) {
        r = 2+ rand() % (n-1);
        l = rand() % (r-1);
        file.write(&word[l], (r-l)*sizeof(char));
        i++;
    }

    file.close();
}
