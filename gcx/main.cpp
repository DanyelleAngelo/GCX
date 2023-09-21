#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "malloc_count.h"
#include "stack_count.h"
#include "compressor-int.hpp"
#include "compressor.hpp"

using namespace std;

void generateReport(char *fileName, double duration, void *base) {
    FILE *file = fopen(fileName, "a");
    isFileOpen(file, "Unable to open report to enter time and memory consumption informatio");

    long long int peak = malloc_count_peak();
    long long int stack = stack_count_usage(base);

    fprintf(file, "%lld|%lld|%5.4lf", peak,stack,duration);
    fclose(file);
}

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
    if(argv[3][0] == 'e' && argc < 6) {
        cout << "\n\x1b[31m[ERROR]\x1b[0m  Number of invalid arguments! \n" <<
                "\tTo perform the extract, you need to provide the file contains interval [l,r].\n"  << endl;
        exit(EXIT_FAILURE);
    }

    clock_t start, finish;
    double duration;
    int32_t l=0, r=0;
    char op = argv[3][0];
    int ruleSize = atoi(&argv[4][0]);
    
    void* base = stack_count_clear();

    if(op=='e') {
        FILE *query = fopen(argv[5], "r");
        isFileOpen(query, "Unable to open file with intervals");
        start = clock();
        while(fscanf(query, "%d %d", &l, &r) == 2) {
            grammarInteger(argv[1], argv[2], op, l, r, ruleSize);
        }
        finish = clock();
        fclose(query);
        duration = (double)(finish - start) / CLOCKS_PER_SEC;
        #if REPORT == 1
            generateReport(argv[6], duration, base);
        #endif
    } else {
        start = clock();
        grammarInteger(argv[1], argv[2], op, l,r, ruleSize);
        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC;
        if(argc == 6)generateReport(argv[5], duration, base);
    }

    printf("Time: %5.2lf(s)\n",duration);
}