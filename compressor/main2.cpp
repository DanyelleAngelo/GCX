#include "uarray.h"
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <fstream>

using namespace std;

int main() {
    int n_numbers = 6;
    int nBits = ceil(log2(n_numbers));
    printf("Número máximo de bits %d\n", nBits);
    uarray *v = ua_allocz(10,62);
    printf("Número de bits de cada elemento em v eh %d \n", v->b);

    ua_put(v, 0, 999999999999999);
    ua_put(v,1,12345678901234);
    ua_put(v,2,323456789012345);
    ua_put(v,3,28765432109876);
    ua_put(v,4,98765432109876);
    ua_put(v, 5, 12345678901234);
    ua_put(v,6,41234567890123459);
    ua_put(v,7,48765432109876);
    ua_put(v,8,58765432109876);
    ua_put(v,9,68765432109876);
    ua_put(v,10,12345648901234);

    printf("1o= %lu\n ", (unsigned long)ua_get(v,0));
    printf("2o= %lu\n ", (unsigned long)ua_get(v,1));
    printf("3o= %lu\n ", (unsigned long)ua_get(v,2));
   
   FILE *file = fopen("teste.txt", "wb");
   fwrite(&v, 8, 1, file);
    fclose(file);
printf("Tamanho de v eh %lu\n", sizeof(v));

cout << "TESTEEEE: " << endl;

   uarray *v2;
   FILE *file2 = fopen("teste.txt", "rb");
   fread(&v2, 8, 1, file2);
    printf("Teste de leitura 1o= %lu\n ", (unsigned long)ua_get(v2,0));
    printf("Teste de leitura 2o= %lu\n ", (unsigned long)ua_get(v2,9));
    printf("Teste de leitura 3o= %lu\n ", (unsigned long)ua_get(v2,10));
    fclose(file2);
}