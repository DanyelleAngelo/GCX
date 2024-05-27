#include "gcis.hpp"
#include "gcis_eliasfano.hpp"
#include "gcis_eliasfano_no_lcp.hpp"
#include "gcis_gap.hpp"
#include "gcis_s8b.hpp"
#include "gcis_unary.hpp"
#include "sais.h"
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
//to dcx
#include "../external/malloc_count/malloc_count.h"
#include "../external/malloc_count/stack_count.h"

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

void load_string_from_file(char *&str, char *filename) {
    std::ifstream f(filename, std::ios::binary);
    f.seekg(0, std::ios::end);
    uint64_t size = f.tellg();
    f.seekg(0, std::ios::beg);
    str = new char[size + 1];
    f.read(str, size);
    str[size] = 0;
    f.close();
};

int main(int argc, char *argv[]) {

#ifdef MEM_MONITOR
    mm.event("GC-IS Init");
#endif

    if (argc != 6) {
        std::cerr << "Usage: \n"
                  << "./gc-is-codec -c <file_to_be_encoded> <output> <codec flag>\n"
                  << "./gc-is-codec -d <file_to_be_decoded> <output> <codec flag>\n"
                  << "./gc-is-codec -s <file_to_be_decoded> <output> <codec flag>\n"
                  << "./gc-is-codec -l <file_to_be_decoded> <output> <codec flag>\n"
                  << "./gc-is-codec -e <encoded_file> <query file> <codec flag>\n";

        exit(EXIT_FAILURE);
    }

    // Dictionary type
    string codec_flag(argv[4]);
    gcis_interface* d;

    //To DCX
    char * file_dcx = argv[5];
    void* base = stack_count_clear();
    double duration = 0.0;
    clock_t clock_time;

    if(codec_flag == "-s8b"){
        d = new gcis_dictionary<gcis_s8b_codec>();
    }
    else if(codec_flag == "-ef"){
        d = new gcis_dictionary<gcis_eliasfano_codec>();
    }
    else{
        cerr << "Invalid CODEC." << endl;
        cerr << "Use -s8b for Simple8b or -ef for Elias-Fano" << endl;
        return 0;
    }

    char *mode = argv[1];
    
    if (strcmp(mode, "-c") == 0) {
        char *str;
        load_string_from_file(str, argv[2]);
        std::ofstream output(argv[3], std::ios::binary);

#ifdef MEM_MONITOR
        mm.event("GC-IS Compress");
#endif

        clock_time = clock();
        d->encode(str);
        clock_time = clock() - clock_time;
        duration = ((double)clock_time)/CLOCKS_PER_SEC;


#ifdef MEM_MONITOR
        mm.event("GC-IS Save");
#endif

        cout << "input:\t" << strlen(str) << " bytes" << endl;
        cout << "output:\t" << d->size_in_bytes() << " bytes" << endl;
        cout << "time: " << duration << " seconds" << endl;

        d->serialize(output);
        output.close();
        delete[] str;
    } else if (strcmp(mode, "-d") == 0) {
        std::ifstream input(argv[2]);
        std::ofstream output(argv[3], std::ios::binary);

#ifdef MEM_MONITOR
        mm.event("GC-IS Load");
#endif

        d->load(input);

#ifdef MEM_MONITOR
        mm.event("GC-IS Decompress");
#endif
        clock_time = clock();
        char *str = d->decode();
        clock_time = clock() - clock_t;
        duration = ((double)clock_time)/CLOCKS_PER_SEC;

        cout << "input:\t" << d->size_in_bytes() << " bytes" << endl;
        cout << "output:\t" << strlen(str) << " bytes" << endl;
        cout << "time: " << duration << setprecision(2) << fixed << " seconds" << endl;

        output.write(str, strlen(str));
        input.close();
        output.close();
    } else if (strcmp(mode, "-s") == 0) {

        std::ifstream input(argv[2]);
        string outfile1 = argv[3];
        string outfile2 = outfile1 + ".sa";
        std::ofstream output1(outfile1, std::ios::binary);
        std::ofstream output2(outfile2, std::ios::binary);

#ifdef MEM_MONITOR
        mm.event("GC-IS/SACA Load");
#endif

        d->load(input);

#ifdef MEM_MONITOR
        mm.event("GC-IS/SACA Decompress");
#endif

        uint_t *SA;
        std::cout << "Building SA under decoding." << std::endl;
        auto start = timer::now();
        unsigned char *str = d->decode_saca(&SA);
        auto stop = timer::now();

        size_t n = strlen((char *)str) + 1;

#if CHECK
        if (!d->suffix_array_check(SA, (unsigned char *)str, (uint_t)n,
                                  sizeof(char), 0))
            std::cout << "isNotSorted!!\n";
        else
            std::cout << "isSorted!!\n";
#endif

        cout << "input:\t" << d->size_in_bytes() << " bytes" << endl;
        cout << "output:\t" << n - 1 << " bytes" << endl;
        cout << "SA:\t" << n * sizeof(uint_t) << " bytes" << endl;
        std::cout << "time: "
                  << (double)duration_cast<seconds>(stop - start).count()
                  << " seconds" << endl;

        size_t real_n = n - 1;
        output1.write((const char *)&real_n, sizeof(real_n));
        output1.write((const char *)str, (real_n) * sizeof(char));
        output2.write((const char *)&real_n, sizeof(real_n));
        output2.write((const char *)&SA[1], sizeof(sa_int32_t) * real_n);
        output1.close();
        output2.close();
        // input.close();
        delete[] SA;
    } else if (strcmp(mode, "-l") == 0) {

        std::ifstream input(argv[2]);

#ifdef MEM_MONITOR
        mm.event("GC-IS/SACA+LCP Load");
#endif

        d->load(input);

#ifdef MEM_MONITOR
        mm.event("GC-IS/SACA_LCP Decompress");
#endif

        uint_t *SA;
        int_t *LCP;
        std::cout << "Building SA+LCP under decoding." << std::endl;
        auto start = timer::now();
        unsigned char *str = d->decode_saca_lcp(&SA, &LCP);
        auto stop = timer::now();

        size_t n = strlen((char *)str) + 1;

#if CHECK
        if (!d->suffix_array_check(SA, (unsigned char *)str, (uint_t)n,
                                  sizeof(char), 0))
            std::cout << "isNotSorted!!\n";
        else
            std::cout << "isSorted!!\n";
        if (!d->lcp_array_check(SA, LCP, (unsigned char *)str, (uint_t)n,
                               sizeof(char), 0))
            std::cout << "isNotLCP!!\n";
        else
            std::cout << "isLCP!!\n";
#endif

        cout << "input:\t" << d->size_in_bytes() << " bytes" << endl;
        cout << "output:\t" << n - 1 << " bytes" << endl;
        cout << "SA:\t" << n * sizeof(uint_t) << " bytes" << endl;
        cout << "LCP:\t" << n * sizeof(uint_t) << " bytes" << endl;
        std::cout << "time: "
                  << (double)duration_cast<seconds>(stop - start).count()
                  << " seconds" << endl;

        string ouf_basename(argv[3]);
        string outfile1(ouf_basename + ".txt");
        string outfile2 = ouf_basename + ".sa";
        string outfile3 = ouf_basename + ".lcp";
        std::ofstream output1(outfile1, std::ios::binary);
        std::ofstream output2(outfile2, std::ios::binary);
        std::ofstream output3(outfile3, std::ios::binary);

        n--;
        output1.write((const char *)str, (n) * sizeof(char));
        output2.write((const char *)&n, sizeof(n));
        output2.write((const char *)&SA[1], sizeof(uint_t) * n);
        output3.write((const char *)&n, sizeof(n));
        output3.write((const char *)&LCP[1], sizeof(int_t) * n);

        output1.close();
        output2.close();
        output3.close();
        input.close();
        delete[] SA;
        delete[] LCP;
    } 
    else if (strcmp(mode, "-e") == 0) {
        std::ifstream input(argv[2], std::ios::binary);
        std::ifstream query(argv[3]);

#ifdef MEM_MONITOR
        mm.event("GC-IS Load");
#endif

        d->load(input);

#ifdef MEM_MONITOR
        mm.event("GC-IS Extract");
#endif
        vector<pair<int, int>> v_query;
        uint64_t l, r, number,len;
        query >> number >> len;
        while (number--) {
            query >> l >> r;
            v_query.push_back(make_pair(l, r));
        }
        duration = d->extract_batch(v_query);
    } else {
        std::cerr << "Invalid mode, use: " << endl
                  << "-c for compression;" << endl
                  << "-d for decompression;" << endl
                  << "-e for extraction;" << endl
                  << "-s for building SA under decompression" << endl
                  << "-l for building SA+LCP under decompression" << endl;

        exit(EXIT_FAILURE);
    }

#ifdef MEM_MONITOR
    mm.event("GC-IS Finish");
#endif

    //To DCX
    FILE *report_dcx = fopen(file_dcx, "a");
    long long int peak = malloc_count_peak();
    long long int stack = stack_count_usage(base);
    fprintf(report_dcx, "%lld|%lld|%5.4lf|", peak,stack,duration);
    printf("Time inserted into the DCX report: %5.4lf\n", duration);
    fclose(report_dcx);
    return 0;
}
