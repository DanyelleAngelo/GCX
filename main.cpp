#include <iostream>
#include <fstream>
#include "compressor-int.hpp"
#include "compressor-elias.hpp"
#include "compressor.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    if(argc < 5) {
        cout << "\n >> Number of invalid arguments! << \n" <<
                "\tList of arguments:\n" <<
                "\t\tInput file  for encode or decode\n" <<
                "\t\tOutput file, contains the result of the chosen operation)\n" <<
                "\t\tOperation - for encode choose: \"e\", for decode choose: \"d\" " <<
                "\t\tSize of rules\n" << endl;
        exit(EXIT_FAILURE);
    }

    char op = argv[3][0];
    int ruleSize = atoi(&argv[4][0]);
    grammar(argv[1], argv[2], op, ruleSize);
}