#include <stdio.h>

int foo(int x, int y, int z) {
    printf("x: %x\n", x);
    printf("y: %x\n", y);
    printf("z: %x\n", z);
    return x + y + z;
}