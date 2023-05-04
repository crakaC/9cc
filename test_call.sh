#!/bin/sh
assert(){ 
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -c -o tmp.o tmp.s
    cc -g -o tmp tmp.o foo.o
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

assert 7 'int main(){ foo(1, 2, 4); }'
assert 3 'int sum(int x, int y){return x + y;} int main(){return sum(1, 2);}'
assert 55 "
int fib(int x){
    if(x <= 2){
        return 1;
    } else {
        return fib(x - 1) + fib(x - 2);
    }
}
int main(){
    return fib(10);
}"
assert 1 "
int func1(int y){ return y;}
int func2(int x, int y){return x;}
int main(){return func2(1, 2);}
"

echo OK