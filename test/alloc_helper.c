#include<stdlib.h>
#include<stdio.h>
int* alloc4(int** p, int a, int b, int c, int d) {
    int* q = malloc(sizeof(int) * 4);
    q[0] = a;
    q[1] = b;
    q[2] = c;
    q[3] = d;
    *p = q;
}