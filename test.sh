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

assert 0 0
assert 42 42
assert 21 "5+20-4"
assert 13 "5+(20-4)-8"
assert 46 "2*3+32/4*5"
assert 17 "-3+20"
assert 2 "-(3-5)"
assert 15 "-3*-5"

echo OK
