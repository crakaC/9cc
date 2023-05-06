#!/bin/sh
assert(){ 
    expected="$1"
    input="$2"

    ./9cc "int main(){ $input }" > tmp.s
    cc -c -o tmp.o tmp.s
    cc -g -o tmp tmp.o alloc_helper.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

make
CFLAGS="-std=c11 -static"
cc $CFLAGS -c -o alloc_helper.o test/alloc_helper.c

assert 8 "
    int *p;
    alloc4(&p, 1, 2, 4, 8);
    int *q;
    q = p + 3;
    return *q;
"

assert 4 "
    int *p;
    alloc4(&p, 1, 2, 4, 8);
    int *q;
    int *r;
    q = p + 3;
    r = q - 1;
    return *r;
"