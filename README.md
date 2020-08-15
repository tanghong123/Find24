# Find24 - A program that solves the 24 game

This is a programming exercise for me to get familier with c++11 features and using Xcode as an IDE for C++.

## Usage
find24 <target> <n1> <n2> ...

It tries to find all algorithmic expressions that can calculate a specific target number (positive integer) from an arbitrary number of input numbers (positive integers).

Expressions that are equivalent under commutative or associative laws are removed. Also removed are expressions that are trivally equivalent, e.g. a - (b - c) is removed in favor of a - b + c, and a / (b / c) removed in favor of a * c / b; if a/b == b/c == 1, we only keep one version, same is for a-b=b-a=0.

## Limitations

- Intermediate results are stored as Rational numbers. However, the dividends and divisors are of int32_t type, and the program cannot handle integer overflown for both.

- Due to the combinatory nature of the problem, I don't think the actual number of input numbers can be more than 10. I have tested the program with up to 8 numbers (taking about 10 seconds on my laptop).
