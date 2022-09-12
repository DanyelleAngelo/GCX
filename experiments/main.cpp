#include <iostream>
#include <vector>
#include "dc3.hpp"
#include "merge_sort.hpp"
#include "utils.hpp"

using namespace std;

int main() {
    char s[] = "yabbadabbado$";
   // char s[] = "abracadabra$"; //size 12
    vector<int> sa = dc3(s);
    printVector(sa, sa.size());
}