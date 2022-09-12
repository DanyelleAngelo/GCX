#include "../include/merge_sort.hpp"

void merge_sort(int *v, int l, int r){
    if(l >= r)return;
    int m = (l+r)/2;
    merge_sort(v, l, m);
    merge_sort(v, m + 1, r);
    merge(v, l, m, r);
}

void merge(int *v, int l, int m, int r){
    int i, j, k= l;
    int nl = m - l + 1, nr = r - m;
    int left[nl], right[nr];
    
    for (i = 0; i < nl; i++) left[i] = v[l + i];
    for (j = 0; j < nr; j++) right[j] = v[m + 1 + j];

    i = 0, j = 0;
    while (i < nl && j < nr) {
        if (left[i] <= right[j]) v[k++] = left[i++];
        else v[k++] = right[j++];
    }

    while (i < nl) v[k++] = left[i++];
    while (j < nr) v[k++] = right[j++];
}