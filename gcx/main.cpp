#include <iostream>
#include <sstream>
#include "compressor.hpp"
#include "utils.hpp"

using namespace std;

void checkArguments(string op, int argc) {
    stringstream arguments;
    arguments   << ERROR_COLOR << "\n[ERROR] " << RESET_COLOR 
                << "Number of invalid arguments for the "
                << ERROR_COLOR << ( operations.count(op) ? operations[op] :  "<uninformed operation>" )
                << RESET_COLOR << " operation! \n"
                "\tList of arguments (in order):\n"
                "\t\t<Operation> "; 

    for(const auto& pair : operations) {
        arguments << " for " << pair.second << " choose \"" << pair.first << "\";";
    }

    arguments << "\n\t\t<Input file> containing the base text to be used in the operation;"
                "\n\t\t<Output file> contains the result of the chosen operation;\n";
    if(op == "-e")arguments << "\t\t<Ranges file> for extraction.\n";

    if(argc < 4 || (argc ==4 && op == "-e")) {
        cout << arguments.str() << endl;
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    string op = (argc >=2) ?  argv[1] : "a";
    checkArguments(op,argc);

    char *reportFile = (op != "-e" && argc >= 4 ) ? argv[4] : (op=="-e" && argc >= 5) ? argv[5] : nullptr;
    char *queriesFile = (op == "-e") ?  argv[4] : nullptr;

    cout << GREEN_COLOR << "Starting to " << operations[op] << " ...\n" << RESET_COLOR;
    grammar(argv[2], argv[3], reportFile, queriesFile, op);

    return 0;
}