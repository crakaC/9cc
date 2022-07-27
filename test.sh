#!/bin/sh
assert(){ 
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
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

assert 21 "5+20-4"
assert 41 " 12 + 34 - 5 "
assert 47 '5+6*7'
assert 15 '5*(9-6)'
assert 4 '( 3 + 5 ) / 2'
assert 10 '-10 + 20'
assert 1 '- ( -1 + 2 ) * -1'
assert 1 '1 + 2 * 5 - 1 == 10'
assert 0 '1 == 2 + 3 + (4 + 5)'
assert 1 '1 != 0'
assert 0 '(2 * 3) != (1 + 2 + 3)'
assert 1 '1 > 0'
assert 0 '1 > 1'
assert 1 '1 + 1 >= 2'
assert 0 '(2 * 3) >= 8'
assert 1 '(5 * (3 - 2) * 2) <= 10'
assert 0 '4 + 5 * 2 <= 10'
echo OK