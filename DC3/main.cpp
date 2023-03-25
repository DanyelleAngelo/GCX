#include "dc3.hpp"
#include <cstdlib>
#include <iostream>
#include <fstream> 
#include <time.h>

using namespace std;

template <typename T>
void print(T v[], int n){
    cout << *(v);
    for(int i=1; i < n ; i++) cout  << ", " << *(v+i);
    cout << endl;
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        cout << "The name of the file containing the text is missing." <<endl;
        exit(EXIT_FAILURE);
    }

    FILE*  file= fopen(argv[1],"r");
    fseek(file, 0, SEEK_END);
    int n = ftell(file)+2;

    int *sa = (int*)calloc(n, sizeof(int));
    char * text = new char[n];
    text[n-2] = 0;
    text[n-1] = 0;

    if(file == NULL) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_SET);
    fread(text, 1, n-2, file);
    fclose(file);
    clock_t start, finish;
    double  duration;
    start = clock();

    dc3(text, sa, n, 0);
    finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;

    cout << "Input text: ";
    print(text, n);
    cout << "Suffix Array: ";
    print(sa, n);
    cout << endl;
    printf("Time: %5.6lf  seconds\n",duration);
    free(sa);
    delete[] text;
    return EXIT_SUCCESS;
}
