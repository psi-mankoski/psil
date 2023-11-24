/*
    File:   PsilTypes.h
    Author: ***PSI***
    Date:   Mon Jul 19 20:33:06 1999

    Description:
       Psil types declarations file.

    Copyright (c) 1999-2019 Joseph J. Mankoski ***PSI***
*/


#ifndef PsilTypes_h
#define PsilTypes_h


// Include declarations files.


// Define constants.


const int   kPsilOK      = 0;
const int   kPsilError   = -1;

const char  kCommentChar = ';';
const char  kLeftParen   = '(';
const char  kRightParen  = ')';
const char  kPoint       = '.';
const char  kPlus        = '+';
const char  kMinus       = '-';
const char  kSpace       = ' ';
const char  kReturn      = '\r';
const char  kNewline     = '\n';
const char  kTab         = '\t';
const char  kQuote       = '\'';
const char  kDoubleQuote = '"';
const char  kBackQuote   = '`';
const char  kSharp       = '#';
const char  kAtSign      = '@';
const char  kComma       = ',';
const char  kSlash       = '/';
const char  kBackSlash   = '\\';
const char  kColon       = ':';

const int   kMaxTokenLen = 256;


// Define types.


typedef int boolean;

typedef struct Environment Environment;

typedef enum PsilType {
    kPsilSymbol,
    kPsilInteger,
    kPsilFlonum,
    kPsilString,
    kPsilCons,
    kPsilFunc,
    kPsilLambda
} PsilType;

typedef struct PsilSymbol {
    const char  *pname;
    struct Form *value;
#if 0
    // (This would be used if Psil were not a 1-LISP.)
    struct Form *functionValue;
    // (Not yet used.)
    struct Form *plist;
    // (Not yet used.)
    struct Form *package;
#endif
} PsilSymbol;

typedef const char *PsilString;

typedef int PsilInteger;

#ifdef FLONUM_IS_DOUBLE
// (Note:  This increases the size of the Form structure by 50% on 32-bit architectures.)
typedef double PsilFlonum;
#else
typedef float PsilFlonum;
#endif // FLONUM_IS_DOUBLE

typedef struct PsilCons {
    struct Form *car;
    struct Form *cdr;
} PsilCons;

typedef Form *(PrimitiveFunc)(void);

typedef struct PsilFunc {
    const char    *name;
    PrimitiveFunc *func;
    int            nargs;
} PsilFunc;

typedef struct PsilLambda {
    struct Form *arglist;
    struct Form *body;
    Environment *env;
} PsilLambda;

// Note:  This structure is large enough to contain an
//         entire CONS cell and its type.  It could be
//         made smaller if we instead stored a pointer
//         to CONS cells instead.  And the type could
//         potentially be determined by where the form
//         is allocated in memory.
typedef struct Form {
    union {
        PsilSymbol  *symbol;
        PsilInteger  integer;
        PsilFlonum   flonum;
        PsilString   string;
        PsilCons     list;
        PsilFunc    *func;
        PsilLambda  *lambda;
    } value;
    PsilType type;
} Form;

typedef bool (CmdFunc)(void);

typedef struct PsilCommand {
    const char *name;
    CmdFunc    *func;
    const char *doc;
} Command;


#endif // !defined(PsilTypes_h)
