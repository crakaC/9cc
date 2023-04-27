#!/bin/sh
assert(){ 
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -c -o tmp.o tmp.s
    cc -o tmp tmp.o foo.o
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
cc $CFLAGS -c -o foo.o test/foo.c

assert 0 'foo();'
assert 0 '1; foo();'
