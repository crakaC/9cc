#!/bin/sh
assert(){ 
    expected="$1"
    input="$2"

    ./9cc "int main(){ $input }" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}
assert 3 "int x; int y; x = 3; y = &x; return *y;"

assert 1 "65535 % 65536; 1 % 2;"
assert 3 "8 % 5;"
assert 2 "13 * 2 % 4;"

assert 20 "
int sum;
int i;
int j;
sum = 0;
for(i = 1; i <= 2; i = i + 1){
    for(j = 1; j <= 4 ; j = j + 1){
        sum = sum + j;
    }
}
return sum;
"

assert 1 'if (0) { return 0; } else if (1) {return 1;} else {return 0;}'
assert 1 'if (1) { if(0) {return 0;} else {return 1;} }'

assert 10 'int a; a = 0; for(;;) if (a < 10) a = a + 1; else return a;'
assert 55 'int sum; int i; sum = 0; for(i = 1; i <= 10; i = i + 1) sum = sum + i; return sum;'

assert 10 'int a; a = 0; while(a < 10) a = a + 1; return a;'

assert 0 'int a; if (1) if (1) a = 0; else a = 1; else a = 2; return a;'
assert 1 'int a; if (1) if (0) a = 0; else a = 1; else a = 2; return a;'
assert 2 'int a; if (0) if (1) a = 0; else a = 1; else a = 2; return a;'

assert 1 'int a; a = 0; if (1) a = 1; return a;'
assert 0 'int a; a = 0; if (0) a = 1; return a;'

assert 1 'int a; if (1) a = 1; else a = 0; return a;'
assert 0 'int a; if (0) a = 1; else a = 0; return a;'

assert 1 "return 1;"
assert 3 "int a; int b; a = 1; b = 2; return a + b;"

assert 1 'int a; a = 1;'
assert 3 'int a; int b; a = b = 1 + 2;'
assert 14 "int a; int b; a = 3; b = 5 * 6 - 8; a + b / 2;"
assert 5 "int a; a = 2 + 3;"

assert 0 '0;'
assert 42 '42;'
assert 21 '5+20-4;'
assert 41 ' 12 + 34 - 5 ;'
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10+20;'
assert 10 '- -10;'
assert 10 '- - +10;'
assert 10 '+ - -10;'

assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'

assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'

assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'

echo OK