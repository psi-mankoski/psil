# Psil:  A Simple Lexical LISP Interpreter in C++.

This repository contains the source code for a simple,
lexically-scoped LISP interpreter implemented in C++.

The code was written for expository and experimental purposes. It is
intended to be simple enough for comprehension of the core ideas in
just a few pages of C++ code (~2.5k lines, including comments) without
getting caught up in the details of compliance with any particular
existing language standard (e.g., R^nRS defining  Scheme, or the
(ANSI) Common Lisp language specification.)  Thus Psil is essentially
a Scheme-like 1-LISP.

The code implements a stack machine with built-in primitive functions
based upon the functions available in the standard C library. Since it
uses only the very basic C++ features, it is extremely portable.

## The Psil Language

The Psil language is defined over the ASCII character set.  Types are
symbol, integer, flonum (defaults to single precision floating point,
can be built as double precision by defining `FLONUM_IS_DOUBLE`),
string, cons, function, and lambda (i.e, closure, a function with
its lexical environment.)

The predefined symbols are: `T` and `NIL`.

The predefined functions are: \<TBD>. 

The built-in special forms are: `quote` (equivalently, `'`), `lambda`,
`if`, `setq`, and `define`.

## Interpreter Structure

Input defaults to reading from standard input, writing to standard
output, and writing error messages to standard error.  If a filename
is supplied on the command line, forms will be read from the specified
file.  The interpreter terminates upon reading EOF.

`setjmp()` / `longjmp()` is used to handle exceptions by throwing
control back to the top level Read-Eval-Print loop.

### Tracing

Tracing of various phases of the interpreter may be toggled using the
function `(trace <Type>)` where `<Type>` may be one of
`{"environment", "evaluator", "primitives", "stack", "all", "none"}`.

## Files

`Makefile` - GNU Makefile for Psil.

`Psil.h` - Main Psil declarations file.

`PsilTypes.h` - Declare the primitive types implementing the Psil interpreter.

`Psil.cpp` - Psil main program.  Implements top-level Read-Eval-Print loop.

`Environment.cpp` - Implements "a-list"-style environments.

`Error.cpp` - Error handling and logging utilities.

`Evaluator.cpp` - The core interpreter `Eval()` and `Apply()` functions and helpers.

`Primitives.cpp` - Define the primitive Psil functions as stack machine routines based upon the C standard library.

`Printer.cpp` - Print Lisp S-Expressions.

`Reader.cpp` - Read Lisp S-Expressions.

`Stack.cpp` - Implement the data and control stack.

## Deficiencies

There are many missing features, including garbage collection and a
richer set of types, such as bignums and strings.  The reader could be
a lot more powerful (e.g, quasiquote.)  A-list environments could be
replaced with hash tables, perhaps adaptively, based on size.  Macros
would be nice.  Non-strict evaluation would also be good to have.  So
would some sort of debugger.

The current dynamic environment is used as the implicit environment
the `eval` and `apply` functions, while using the current top-level
environment isn't quite right either, since that is actually the
initial environment.

There are questionably-handled edge cases in defining an unbound
identifier in terms of itself, e.g.,

`(define x x)` ==> `(setq x nil)`.

and

`(define a (lambda () a))` ==> `(lambda () a)`, i.e., a function returning itself.

## License

This code is published under the MIT License (i.e., the "Expat License".) Please see the file `LICENSE`.

## Enjoy!!

## References

1. <a name="ref1"></a>J. McCarthy, et al., _The LISP 1.5 Programmer's Manual_, 1962.

2. <a name="ref3"></a>G. J. Sussman and G. L. Steele, Jr., "SCHEME: An Interpreter for the Extended Lambda Calculus", MIT AI memo #349, December 1975.

3. <a name="ref2"></a>H. Abelson and G. J. Sussman with J. Sussman, _Structure and Interpretation of Computer Programs_, 2nd ed., 1996.

4. <a name="ref4"></a>C. Queinnec, _LiSP: Lisp in Small Pieces_, 1996.
