#!/bin/bash
assert() {
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

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 13 "5+(20-4)-8;"
assert 46 "2*3+32/4*5;"
assert 17 "-3+20;"
assert 2 "-(3-5);"
assert 15 "-3*-5;"
assert 1 "2+2 == 4;"
assert 0 "2+2 == 5;"
assert 0 "2+2 != 4;"
assert 1 "2+2 != 5;"
assert 1 "1<2;"
assert 0 "2<1;"
assert 0 "1>2;"
assert 1 "2>1;"
assert 0 "2<=1;"
assert 1 "2<=2;"
assert 1 "2<=3;"
assert 1 "2>=1;"
assert 1 "2>=2;"
assert 0 "2>=3;"
assert 3 "1;2;3;"
assert 1 "x=1;x;"
assert 1 "x=y=1;x;"
assert 1 "x=y=1;y;"
assert 3 "foo=3;bar=5;foo;"

echo OK
