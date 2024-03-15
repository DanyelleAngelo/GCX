#include <iostream>
#include <sstream>
#include <map>
#include "compressor.hpp"
#include "utils.hpp"

using namespace std;

void checkArguments(char op, int argc, map<char, string> operations) {
    stringstream arguments;
    arguments   << ERROR_COLOR << "\n[ERROR] " << RESET_COLOR 
                << "Number of invalid arguments for the "
                << ERROR_COLOR << ( operations.count(op) ? operations[op] :  "<uninformed operation>" )
                << RESET_COLOR << " operation! \n"
                "\tList of arguments:\n"
                "\t\tInput file  for encode or decode;\n"
                "\t\tOutput file, contains the result of the chosen operation);\n"
                "\t\tOperation -"; 

    for(const auto& pair : operations) {
        arguments << " for " << pair.second << " choose: \"" << pair.first << "\";";
    }

    if(op == 'e')arguments << "\n\t\tIntervals file  for extract";

    if(argc < 4 || (argc ==4 && op == 'e')) {
        cout << arguments.str() << endl;
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    map<char, string> operations{ 
        {'c', "compress"},
        {'d', "decompress"},
        {'e', "extract"}
    };
    
    char op = (argc >=4) ?  argv[3][0] : 'a';
    checkArguments(op,argc,operations);

    char *reportFile = (op != 'e' && argc > 4 ) ? argv[4] : (op=='e' && argc > 5) ? argv[5] : nullptr;
    char *queriesFile = (op == 'e') ?  argv[4] : nullptr;

    cout << GREEN_COLOR << "Starting to " << operations[op] << " ...\n" << RESET_COLOR;

    grammar(argv[1], argv[2], reportFile, queriesFile, op);

    return 0;
}