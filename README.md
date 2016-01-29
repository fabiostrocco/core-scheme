Core Scheme is a subset of scheme, with the purpose of illustrating how to build 
compilers and interpreters. 
This project contains a compiler from CSCheme to x86 assembly written in C++ with LLVM and a CScheme interpreter.

HOW TO COMPILE THE COMPILER
----------------------------

Please use **clang 3.4-1** and Linux Ubnutu or an x86 compatible machine
Type make to start the compilation.

Please note that earlier or later **LLVM** and **CLANG** installations may compile this code.

HOW TO USE THE COMPILER
----------------------------

You can find a file named "test.scm" in this directory.
It contains a cscheme sample file. Alternatively you can just
type test to compile the test.scm file.

The infrastructure toolchain contains the following programs:

- cschemec: the CScheme to x86 Assembly compiler (it uses LLVM).
- cscheme:  the CScheme interpreter.
- cschemea: the CScheme analyzer. It prints the input scheme program with
            lexical scoping resolution informations

The former 3 applications are based on csc-comp, that is the compiler/interpreter implementaion in C++.
You can try these tools on "test.scm".

To test the compiler write
```shell
./cschemec test.scm
./test.scm.out
3
```
it will run the program "test.scm" that asks for a numeric input and
calculates the factorial, in the example 3. The answer will be the following

```shell
6
```

Similarly, you can have test the interpreter as follows
```shell
./cscheme test.scm
3
```

You can test the formatter using the following instruction

```shell
./cschemea test.scm
```
It will pretty print "test.scm" and all the referenced programs.

CORE SCHEME GRAMMAR
----------------------------

CScheme is a minimal (turing-complete) version of scheme.
The CScheme grammar is the following:

```bnf
Program ::= Load* Definition* exp
Load ::= (load "PATH")
Definition ::= (define x exp)

exp ::= N | x | (lambda (x ... x) e) | (e ... e) | (if e e e) | (set! x e)

default names: +,-,*,/,=,<,>,read,write
```

Where 'PATH' represents a string, 'N' represets a natural number, and 'x' an identifier name 
(without numbers, only letters and some special characters are allowed).
