#include "9cc.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "invalid number of arguments\n");
        return 1;
    }

    user_input = argv[1];
    tokenize(argv[1]);
    // fprintf(stderr, "tokenized!\n");
    program();
    // fprintf(stderr, "parsed!\n");

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    for (int i = 0; code[i]; i++) {
        gen(code[i]);
    }

    return 0;
}