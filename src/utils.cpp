#include <iostream>
#include <vector>
#include "../include/utils.hpp"


void printArray(int arr[], int size){
    int i;
    for(i = 0; i < size; i++){
        cout << arr[i] << " ";
    }
    cout << endl;
}

void printVector(vector<int> v, int size) {
    for(int i=0; i < size; i++){
        cout << v[i] << " ";
    }
    cout << endl;
}