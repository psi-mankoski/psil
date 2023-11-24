/*
    File:   Psil.h
    Author: ***PSI***
    Date:   Mon Jul 19 20:31:13 1999

    Description:
       Psil declarations file.

    Copyright (c) 1999-2019 Joseph J. Mankoski ***PSI***
*/


#ifndef Psil_h
#define Psil_h


// Include declarations files.


#include <setjmp.h>
#include <stdio.h>
#include "PsilTypes.h"


// Define constants.


const char kPsilVersion[] = "1.0";

// setjmp()/longjmp() return values:
const int kNoError         =  0;
const int kCaughtError     = -1;
const int kExitInterpreter = -2;


// Define global variables.


extern Form *SymbolNIL;
extern Form *SymbolT;
extern Form *SymbolQUOTE;
extern Form *SymbolLAMBDA;
extern Form *SymbolIF;
extern Form *SymbolSETQ;
extern Form *SymbolDEFINE;

extern bool TraceEnvironment;
extern bool TraceEvaluator;
extern bool TracePrimitives;
extern bool TraceStack;

extern Environment *TopLevelEnv;
extern Environment *CurrentEnv;

extern FILE    *StandardInput;
extern FILE    *StandardOutput;
extern FILE    *StandardError;
extern jmp_buf  TopLevelJmpBuf;
extern bool     IsInteractive;
extern bool     InterpreterRunning;

extern bool ReadEOF;
extern int  ReadLevel;


// Define function prototypes.


// Error functions:


void  DPrintf(const char *format, ...);
void  Message(const char *format, ...);
int   Error(const char *format, ...);
void  ErrorOut(const char *format, ...);
Form *ErrorForm(const char *format, ...);


// Stack machine functions:


int   InitializeStack(void);
int   SaveStack(void);
void  RestoreStack(int sp);
void  ResetStack(void);
Form *Push(Form *form);
Form *Pop(void);


// Command functions:


bool  DoCommand(char *cmd);
char *ReadCommand(FILE *instream);


// Environment functions:


Environment *InitializeEnvironment(void);
void         DeInitializeEnvironment(Environment *env);
Form        *Lookup(const char *name, Environment *env);
Environment *LookupBinding(const char *name, Environment *env);
Environment *Bind(const char *name, Form *value, Environment *env);
Environment *SetBinding(const char *name, Form *value, Environment *env);
Environment *Unbind(Environment *env);
void         PrintEnvironment(Environment *env, FILE *outstream, bool verbose);


// Reader functions:


int   InitializeReader(void);
void  DeInitializeReader(void);
Form *Intern(const char *token);
Form *Read(FILE *instream);


// Evaluator functions:


Form *Eval(Form *form, Environment *env);
Form *Apply(Form *func, Form *args, Environment *env);


// Printer functions:


int Print(Form *form, FILE *outstream);


// Primitive functions:


bool IsNull(Form *form);
bool IsSymbol(Form *form);
bool IsInteger(Form *form);
bool IsFlonum(Form *form);
bool IsNumber(Form *form);
bool IsString(Form *form);
bool IsAtom(Form *form);
bool IsCons(Form *form);
bool IsFunc(Form *form);
bool IsClosure(Form *form);
bool IsLambda(Form *form);
bool IsLambdaForm(Form *form);
bool IsQuote(Form *form);
bool IsIf(Form *form);
bool IsAssignment(Form *form);
bool IsDefinition(Form *form);

const char  *SymbolName(Form *form);
int          IntegerValue(Form *form);
double       FlonumValue(Form *form);
const char  *StringValue(Form *form);
PsilFunc    *FuncValue(Form *form);
Form        *LambdaArglist(Form *form);
Form        *LambdaBody(Form *form);
Environment *LambdaEnvironment(Form *form);

bool Eq(Form *x, Form *y);
bool EqStr(Form *form, const char *string);

Form *Quote(Form *form);
Form *Car(Form *form);
Form *Cdr(Form *form);
Form *Cadr(Form *form);
Form *Cdar(Form *form);
Form *Cddr(Form *form);
Form *Caddr(Form *form);
Form *Cadddr(Form *form);
Form *MakeSymbol(const char *pname);
Form *MakeInteger(int integer);
Form *MakeFlonum(double flonum);
Form *MakeNumber(double number);
Form *MakeString(const char *string);
Form *MakeFunc(PsilFunc *func);
Form *MakeClosure(Form *arglist, Form *body, Environment *env);
Form *Cons(Form *car, Form *cdr);
Form *NumberEqual(Form *x, Form *y);
Form *Less(Form *x, Form *y);
Form *LessEqual(Form *x, Form *y);
Form *Greater(Form *x, Form *y);
Form *GreaterEqual(Form *x, Form *y);
Form *Zerop(Form *x);
Form *Plus(Form *x, Form *y);
Form *Minus(Form *x, Form *y);
Form *Times(Form *x, Form *y);
Form *Divide(Form *x, Form *y);
Form *Remainder(Form *x, Form *y);
Form *Equal(Form *x, Form *y);
Form *Set(Form *x, Form *y);
Form *Trace(Form *x, Form *y);
Form *Exit(void);

Form *FuncCar(void);
Form *FuncCdr(void);
Form *FuncCons(void);
Form *FuncAtom(void);
Form *FuncEq(void);
Form *FuncNull(void);
Form *FuncNumberp(void);
Form *FuncZerop(void);
Form *FuncNumberEqual(void);
Form *FuncLess(void);
Form *FuncLessEqual(void);
Form *FuncGreater(void);
Form *FuncGreaterEqual(void);
Form *FuncPlus(void);
Form *FuncMinus(void);
Form *FuncTimes(void);
Form *FuncDivide(void);
Form *FuncRemainder(void);
Form *FuncAdd1(void);
Form *FuncSub1(void);
Form *FuncLn(void);
Form *FuncExp(void);
Form *FuncLog(void);
Form *FuncExpt(void);
Form *FuncSin(void);
Form *FuncCos(void);
Form *FuncTan(void);
Form *FuncEqual(void);
Form *FuncSet(void);
Form *FuncTrace(void);
Form *FuncExit(void);
Form *FuncRead(void);
Form *FuncEval(void);
Form *FuncApply(void);
Form *FuncPrin1(void);
Form *FuncPrint(void);


#endif // !defined(Psil_h)
