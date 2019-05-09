/*
    File:   Environment.cpp
    Author: ***PSI***
    Date:   Sun Apr 16 22:24:41 2000

    Description:
       Psil environment implementation.

    Copyright (c) 2000-2019 Joseph J. Mankoski ***PSI***
*/


// Include declarations files.


#include <stdlib.h>
#include <string.h>
#include "Psil.h"


// Define types.


struct Environment {
    const char  *name;
    Form        *value;
    Environment *parent;
};


// Define global variables


static PsilFunc PrimitiveFuncs[] =
{
    {"CAR",        FuncCar,            1},
    {"CDR",        FuncCdr,            1},
    {"CONS",       FuncCons,           2},
    {"ATOM",       FuncAtom,           1},
    {"EQ",         FuncEq,             2},
    {"NULL",       FuncNull,           1},
    {"NOT",        FuncNull,           1},
    {"NUMBERP",    FuncNumberp,        1},
    {"ZEROP",      FuncZerop,          1},
    {"=",          FuncNumberEqual,    2},
    {"<",          FuncLess,           2},
    {"<=",         FuncLessEqual,      2},
    {">",          FuncGreater,        2},
    {">=",         FuncGreaterEqual,   2},
    {"PLUS",       FuncPlus,           2},
    {"+",          FuncPlus,           2},
    {"MINUS",      FuncMinus,          2},
    {"-",          FuncMinus,          2},
    {"TIMES",      FuncTimes,          2},
    {"*",          FuncTimes,          2},
    {"DIVIDE",     FuncDivide,         2},
    {"/",          FuncDivide,         2},
    {"REMAINDER",  FuncRemainder,      2},
    {"MOD",        FuncRemainder,      2},
    {"%",          FuncRemainder,      2},
    {"ADD1",       FuncAdd1,           1},
    {"1+",         FuncAdd1,           1},
    {"SUB1",       FuncSub1,           1},
    {"1-",         FuncSub1,           1},
    {"LN",         FuncLn,             1},
    {"EXP",        FuncExp,            1},
    {"LOG",        FuncLog,            2},
    {"EXPT",       FuncExpt,           2},
    {"SIN",        FuncSin,            1},
    {"COS",        FuncCos,            1},
    {"TAN",        FuncTan,            1},
    {"EQUAL",      FuncEqual,          2},
    {"SET",        FuncSet,            2},
    {"TRACE",      FuncTrace,          1},
    {"EXIT",       FuncExit,           0},
    {"QUIT",       FuncExit,           0},
    {"READ",       FuncRead,           0},
    {"EVAL",       FuncEval,           1},
    {"APPLY",      FuncApply,          2},
    {"PRIN1",      FuncPrin1,          1},
    {"PRINT",      FuncPrint,          1},
    {NULL,         NULL,               0}
};

Environment *TopLevelEnv = NULL;
Environment *CurrentEnv = NULL;

bool TraceEnvironment = false;


// Define functions.


Environment *CreateEnvironment(void)
{
    Environment *env;

    if (!(env = (Environment *) malloc(sizeof(Environment))))
      Error("CreateEnvironment():  calloc(%d) failed!\n", sizeof(Environment));

    return env;
}

void DestroyEnvironment(Environment *env)
{
    free((void *) env);
}

Environment *InitializeEnvironment(void)
{
    Environment *env = NULL;

    // Bind constants.
    // Note:  Nothing is guaranteeing the constancy of these values!
    env = Bind(SymbolName(SymbolT), SymbolT, env);
    env = Bind(SymbolName(SymbolNIL), SymbolNIL, env);

    // Bind primitive functions.
    PsilFunc *prim = PrimitiveFuncs;
    while (prim->name) {
        env = Bind(SymbolName(Intern(prim->name)), MakeFunc(prim), env);
        prim++;
    }

    return env;
}

void DeInitializeEnvironment(Environment *env)
{
    while ((env = Unbind(env)))
      ;
}

Form *Lookup(const char *name, Environment *env)
{
    while (env)
      if (!strcasecmp(name, env->name))
        return env->value;
      else
        env = env->parent;

    return ErrorForm("Lookup():  Unbound symbol: \"%s\"!\n", name);
}

Environment *LookupBinding(const char *name, Environment *env)
{
    while (env)
      if (!strcasecmp(name, env->name))
        return env;
      else
        env = env->parent;

    return NULL;
}

Environment *Bind(const char *name, Form *value, Environment *env)
{
    Environment *newEnv = CreateEnvironment();

    if (!newEnv)
      ErrorOut("Bind():  CreateEnvironment() failed!\n");

    newEnv->name = name;
    newEnv->value = value;
    newEnv->parent = env;

    return newEnv;
}

Environment *SetBinding(const char *name, Form *value, Environment *env)
{
    Environment *binding;

    if ((binding = LookupBinding(name, env))) {
        binding->value = value;
        return env;
    } else
      return Bind(name, value, env);
}

Environment *Unbind(Environment *env)
{
    Environment *parentEnv;

    if (!env)
      return NULL;
    else {
        parentEnv = env->parent;
        DestroyEnvironment(env);
        return parentEnv;
    }
}

void PrintEnvironment(Environment *env, FILE *outstream)
{
    if (env) {
        fprintf(outstream, "#<ENV");
        if (TraceEnvironment) {
            if (env == TopLevelEnv)
              fprintf(outstream, ": TOP-LEVEL");
            else {
                fprintf(outstream, ": [\"%s\", ", env->name);
                Print(env->value, outstream);
                fprintf(outstream, "] ");
                PrintEnvironment(env->parent, outstream);
            }
        }
        fprintf(outstream, ">");
    }
}
