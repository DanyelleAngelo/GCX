#include "dc3.hpp"
#include <cstdlib>
#include <iostream>
#include <fstream> 

using namespace std;

void print(int v[], int n){
    cout << *(v);
    for(int i=1; i < n ; i++) cout  << ", " << *(v+i);
    cout << endl;
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        cout << "The name of the file containing the text is missing." <<endl;
        exit(EXIT_FAILURE);
    }

    ifstream file(argv[1], ios::in);

    if(!file.is_open()) {
        cout << "An error occurred while opening the file" << endl;
        exit(EXIT_FAILURE);
    }
    
    while (!file.eof()){
        string str;
        getline(file, str);
        str.append("$");

        int *sa = dc3(&str[0], str.size());
        
        cout << "Input text: "  << str << endl;
        cout << "Suffix Array: ";
        print(sa, str.size());
        cout << endl;
    }

    file.close();
    return EXIT_SUCCESS;
}