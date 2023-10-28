#include <iostream>
#include "compressor.hpp"
#include "utils.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    if(argc < 5) {
        cout << "\n\x1b[31m[ERROR]\x1b[0m  Number of invalid arguments! \n" <<
                "\tList of arguments:\n" <<
                "\t\tInput file  for encode or decode\n" <<
                "\t\tOutput file, contains the result of the chosen operation)\n" <<
                "\t\tOperation - for compress choose: \"c\", for decode choose: \"d\" \n" <<
                "\t\tSize of rules\n"  << endl;
        exit(EXIT_FAILURE);
    } 

    char op = argv[3][0];
    int coverage = atoi(&argv[4][0]);
    char *reportFile = nullptr, *queriesFile = nullptr;
    
    if (op=='e') {
        queriesFile = argv[5];
        if(argc==7)reportFile = argv[6];
    } else if(op != 'e' && argc == 6) {
        reportFile = argv[5];
    } else if(op == 'e' && argc < 6) {
        error("There are files missing!");
    }

    grammar(argv[1], argv[2], reportFile, queriesFile, op, coverage);

    return 0;
}