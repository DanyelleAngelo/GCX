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

void readFile(char *fileName, unsigned char *&text,long int *&uText, long int *&sa, long int &textSize, int module) {
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

int main(int argc, char *argv[]) {
    if(argc < 2) {
        cout << "The name of the file containing the text is missing." <<endl;
        exit(EXIT_FAILURE);
    }

    int module =3;
    unsigned char *text;
    long int *uText, *sa, textSize;

    readFile(argv[1], text, uText, sa, textSize, module);

    
    clock_t start, finish;
    double  duration;
    start = clock();
    dc3(uText, sa, textSize, 0, module, 257);
    finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;

    cout << "\n-------- Successfully generated suffix array --------" << endl;
    //cout << "Input text: ";
    //print(text, textSize);
    #if DEBUG
        cout << "Suffix Array: ";
        print(sa, textSize);
        cout << endl;
    #endif
    printf("Time: %5.6lf  seconds\n",duration);
    free(sa);
    delete[] text;
    return EXIT_SUCCESS;
}
