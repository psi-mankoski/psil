/*
    File:   Primitives.cpp
    Author: ***PSI***
    Date:   Mon Jul 19 20:23:24 1999

    Description:
       Psil primitives.

    Copyright (c) 1999-2019 Joseph J. Mankoski ***PSI***
*/


// Include declarations files.


#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Psil.h"


// Define macros.


#define TRACE_PRIM(funcName) \
        if (TracePrimitives) \
           fprintf(StandardError, "PRIM: %s\n", funcName);


// Define global variables.


bool TracePrimitives = false;


// Define functions.


bool IsNull(Form *form)
{
    return (form == SymbolNIL);
}

bool IsSymbol(Form *form)
{
    return (form && (form->type == kPsilSymbol));
}

bool IsInteger(Form *form)
{
    return (form && (form->type == kPsilInteger));
}

bool IsFlonum(Form *form)
{
    return (form && (form->type == kPsilFlonum));
}

bool IsNumber(Form *form)
{
    return IsInteger(form) || IsFlonum(form);
}

bool IsString(Form *form)
{
    return (form && (form->type == kPsilString));
}

bool IsAtom(Form *form)
{
    return !IsCons(form);
}

bool IsCons(Form *form)
{
    return (form && (form->type == kPsilCons));
}

bool IsFunc(Form *form)
{
    return (form && (form->type == kPsilFunc));
}

bool IsClosure(Form *form)
{
    return (form && (form->type == kPsilLambda));
}

bool IsLambda(Form *form)
{
    return form == SymbolLAMBDA;
}

bool IsLambdaForm(Form *form)
{
    return IsCons(form) && IsLambda(Car(form));
}

bool IsQuote(Form *form)
{
    return (form == SymbolQUOTE) || EqStr(form, "'");
}

bool IsIf(Form *form)
{
    return form == SymbolIF;
}

bool IsAssignment(Form *form)
{
    return (Car(form) == SymbolSETQ) && IsSymbol(Cadr(form));
}

bool IsDefinition(Form *form)
{
    return (Car(form) == SymbolDEFINE) && IsSymbol(Cadr(form));
}

const char *SymbolName(Form *form)
{
    return form ? form->value.symbol->pname : "";
}

int IntegerValue(Form *form)
{
    return form ? form->value.integer : 0;
}

double FlonumValue(Form *form)
{
    return form ? form->value.flonum : 0.0;
}

double NumberValue(Form *form)
{
    double value = 0.0;

    if (form) {
        if (IsInteger(form))
          value = (double) IntegerValue(form);
        else if (IsFlonum(form))
          value = FlonumValue(form);
        else
          Error("NumberValue():  Non-numeric value!\n");
    }

    return value;
}

const char *StringValue(Form *form)
{
    return form ? form->value.string : "";
}

PsilFunc *FuncValue(Form *form)
{
    return form ? form->value.func : NULL;
}

Form *LambdaArglist(Form *form)
{
    return form ? form->value.lambda->arglist : NULL;
}

Form *LambdaBody(Form *form)
{
    return form ? form->value.lambda->body : NULL;
}

Environment *LambdaEnvironment(Form *form)
{
    return form ? form->value.lambda->env : NULL;
}

bool Eq(Form *x, Form *y)
{
    return (x == y) || (IsInteger(x) && IsInteger(y) && (IntegerValue(x) == IntegerValue(y)));
}

bool EqStr(Form *form, const char *string)
{
    return (IsSymbol(form) && !strcasecmp(SymbolName(form), string));
}

Form *Quote(Form *form)
{
    return form;
}

Form *Car(Form *form)
{
    if (IsNull(form))
      return SymbolNIL;
    else if (IsAtom(form))
      return ErrorForm("Car():  Illegal atomic argument!\n");
    else
      return form->value.list.car;
}

Form *Cdr(Form *form)
{
    if (IsNull(form))
      return SymbolNIL;
    else if (IsAtom(form))
      return ErrorForm("Cdr():  Illegal atomic argument!\n");
    else
      return form->value.list.cdr;
}

Form *Cadr(Form *form)
{
    return Car(Cdr(form));
}

Form *Cdar(Form *form)
{
    return Cdr(Car(form));
}

Form *Cddr(Form *form)
{
    return Cdr(Cdr(form));
}

Form *Caddr(Form *form)
{
    return Car(Cdr(Cdr(form)));
}

Form *Cadddr(Form *form)
{
    return Car(Cdr(Cdr(Cdr(form))));
}

Form *AllocateForm(void)
{
    return (Form *) malloc(sizeof(Form));
}

void DeallocateForm(Form *form)
{
    free((void *) form);
}

PsilSymbol *AllocateSymbol(void)
{
    return (PsilSymbol *) malloc(sizeof(PsilSymbol));
}

void DeallocateSymbol(PsilSymbol *symbol)
{
    free((void *) symbol);
}

Form *MakeSymbol(const char *pname)
{
    Form *form;

    if ((form = AllocateForm()) == NULL)
      return ErrorForm("MakeSymbol():  AllocateForm() failed!\n");

    form->type = kPsilSymbol;

    if ((form->value.symbol = AllocateSymbol()) == NULL) {
        DeallocateForm(form);
        return ErrorForm("MakeSymbol():  AllocateSymbol() failed!\n");
    }

    form->value.symbol->pname = pname;

    return form;
}

Form *MakeInteger(int integer)
{
    Form *form;

    if ((form = AllocateForm()) == NULL) {
        Error("MakeInteger():  AllocateForm() failed!\n");
        return SymbolNIL;
    }

    form->type = kPsilInteger;
    form->value.integer = integer;

    return form;
}

Form *MakeFlonum(double flonum)
{
    Form *form;

    if ((form = AllocateForm()) == NULL) {
        Error("MakeFlonum():  AllocateForm() failed!\n");
        return SymbolNIL;
    }

    form->type = kPsilFlonum;
    form->value.flonum = flonum;

    return form;
}

Form *MakeNumber(double number)
{
    if (number == (int) number)
      return MakeInteger((int) number);
    else
      return MakeFlonum(number);
}

Form *MakeString(const char *string)
{
    Form *form;

    if ((form = AllocateForm()) == NULL) {
        Error("MakeString():  AllocateForm() failed!\n");
        return SymbolNIL;
    }

    form->type = kPsilString;
    form->value.string = string;

    return form;
}

Form *MakeFunc(PsilFunc *func)
{
    Form *form;

    if ((form = AllocateForm()) == NULL) {
        Error("MakeFunc():  AllocateForm() failed!\n");
        return SymbolNIL;
    }

    form->type = kPsilFunc;
    form->value.func = func;

    return form;
}

PsilLambda *AllocateLambda(void)
{
    return (PsilLambda *) malloc(sizeof(PsilLambda));
}

void DeallocateLambda(PsilLambda *lambda)
{
    free((void *) lambda);
}

Form *MakeClosure(Form *arglist, Form *body, Environment *env)
{
    Form *form;

    if ((form = AllocateForm()) == NULL)
      return ErrorForm("MakeClosure():  AllocateForm() failed!\n");

    form->type = kPsilLambda;

    if ((form->value.lambda = AllocateLambda()) == NULL) {
        DeallocateForm(form);
        return ErrorForm("MakeClosure():  AllocateLambda() failed!\n");
    }

    form->value.lambda->arglist = arglist;
    form->value.lambda->body = body;
    form->value.lambda->env = env;

    return form;
}

Form *Cons(Form *car, Form *cdr)
{
    Form *form;

    if ((form = AllocateForm()) == NULL)
      return ErrorForm("Cons():  AllocateForm() failed!\n");

    form->type = kPsilCons;
    form->value.list.car = car;
    form->value.list.cdr = cdr;

    return form;
}

Form *Zerop(Form *x)
{
    if (!IsNumber(x))
      return ErrorForm("Zerop():  Non-numeric argument(s)!\n");

    return NumberValue(x) == 0.0 ? SymbolT : SymbolNIL;
}

Form *NumberEqual(Form *x, Form *y)
{
    if (!IsNumber(x) || !IsNumber(y))
      return ErrorForm("NumberEqual():  Non-numeric argument(s)!\n");

    return NumberValue(x) == NumberValue(y) ? SymbolT : SymbolNIL;
}

Form *Less(Form *x, Form *y)
{
    if (!IsNumber(x) || !IsNumber(y))
      return ErrorForm("Less():  Non-numeric argument(s)!\n");

    return NumberValue(x) < NumberValue(y) ? SymbolT : SymbolNIL;
}

Form *LessEqual(Form *x, Form *y)
{
    if (!IsNumber(x) || !IsNumber(y))
      return ErrorForm("LessEqual():  Non-numeric argument(s)!\n");

    return NumberValue(x) <= NumberValue(y) ? SymbolT : SymbolNIL;
}

Form *Greater(Form *x, Form *y)
{
    if (!IsNumber(x) || !IsNumber(y))
      return ErrorForm("Greater():  Non-numeric argument(s)!\n");

    return NumberValue(x) > NumberValue(y) ? SymbolT : SymbolNIL;
}

Form *GreaterEqual(Form *x, Form *y)
{
    if (!IsNumber(x) || !IsNumber(y))
      return ErrorForm("GreaterEqual():  Non-numeric argument(s)!\n");

    return NumberValue(x) >= NumberValue(y) ? SymbolT : SymbolNIL;
}

Form *Plus(Form *x, Form *y)
{
    if (!IsNumber(x) || !IsNumber(y))
      return ErrorForm("Plus():  Non-numeric argument(s)!\n");

    return MakeNumber(NumberValue(x) + NumberValue(y));
}

Form *Minus(Form *x, Form *y)
{
    if (!IsNumber(x) || !IsNumber(y))
      return ErrorForm("Minus():  Non-numeric argument(s)!\n");

    return MakeNumber(NumberValue(x) - NumberValue(y));
}

Form *Times(Form *x, Form *y)
{
    if (!IsNumber(x) || !IsNumber(y))
      return ErrorForm("Times():  Non-numeric argument(s)!\n");

    return MakeNumber(NumberValue(x) * NumberValue(y));
}

Form *Divide(Form *x, Form *y)
{
    if (!IsNumber(x) || !IsNumber(y))
      return ErrorForm("Divide():  Non-numeric argument(s)!\n");
    else if (Zerop(y) == SymbolT)
      return ErrorForm("Divide():  Division by zero!\n");

    return MakeNumber(NumberValue(x) / NumberValue(y));
}

Form *Remainder(Form *x, Form *y)
{
    if (!IsNumber(x) || !IsNumber(y))
      return ErrorForm("Remainder():  Non-numeric argument(s)!\n");
    else if (Zerop(y) == SymbolT)
      return ErrorForm("Remainder():  Division by zero!\n");

    return MakeInteger((int) NumberValue(x) % (int) NumberValue(y));
}

Form *Add1(Form *x)
{
    if (!IsNumber(x))
      return ErrorForm("Add1():  Non-numeric argument!\n");

    return MakeNumber(NumberValue(x) + 1);
}

Form *Sub1(Form *x)
{
    if (!IsNumber(x))
      return ErrorForm("Sub1():  Non-numeric argument!\n");

    return MakeNumber(NumberValue(x) - 1);
}

Form *Ln(Form *x)
{
    if (!IsNumber(x))
      return ErrorForm("Ln():  Non-numeric argument!\n");

    return MakeNumber(log(NumberValue(x)));
}

Form *Exp(Form *x)
{
    if (!IsNumber(x))
      return ErrorForm("Exp():  Non-numeric argument!\n");

    return MakeNumber(exp(NumberValue(x)));
}

Form *Log(Form *x, Form *y)
{
    if (!IsNumber(x) || !IsNumber(y))
      return ErrorForm("Log():  Non-numeric argument(s)!\n");

    return MakeNumber(log(NumberValue(x)) / log(NumberValue(y)));
}

Form *Expt(Form *x, Form *y)
{
    if (!IsNumber(x) || !IsNumber(y))
      return ErrorForm("Expt():  Non-numeric argument(s)!\n");

    return MakeNumber(pow(NumberValue(x), NumberValue(y)));
}

Form *Sine(Form *x)
{
    if (!IsNumber(x))
      return ErrorForm("Sine():  Non-numeric argument!\n");

    return MakeNumber(sin(NumberValue(x)));
}

Form *Cosine(Form *x)
{
    if (!IsNumber(x))
      return ErrorForm("Cosine():  Non-numeric argument!\n");

    return MakeNumber(cos(NumberValue(x)));
}

Form *Tangent(Form *x)
{
    if (!IsNumber(x))
      return ErrorForm("Tangent():  Non-numeric argument!\n");

    return MakeNumber(tan(NumberValue(x)));
}

Form *Equal(Form *x, Form *y)
{
    if (x == y)
      return SymbolT;
    else if (IsAtom(x) && IsAtom(y))
      if (IsFlonum(x) && IsFlonum(y))
        return NumberEqual(x, y);
      else
        return (Eq(x, y) ? SymbolT : SymbolNIL);
    else if (IsCons(x) && IsCons(y) && (Equal(Car(x), Car(y)) == SymbolT))
      return Equal(Cdr(x), Cdr(y));
    else
      return SymbolNIL;
}

Form *Set(Form *x, Form *y)
{
    if (!IsSymbol(x))
      return ErrorForm("Set():  Non-symbol first argument!\n");
    else {
        CurrentEnv = SetBinding(SymbolName(x), y, CurrentEnv);
        return y;
    }
}

Form *Trace(Form *form)
{
    bool status = false;

    if (!IsSymbol(form))
      return ErrorForm("Trace():  Non-symbol argument!\n");
    else if (EqStr(form, "environment"))
      status = (TraceEnvironment = !TraceEnvironment);
    else if (EqStr(form, "evaluator"))
      status = (TraceEvaluator = !TraceEvaluator);
    else if (EqStr(form, "primitives"))
      status = (TracePrimitives = !TracePrimitives);
    else if (EqStr(form, "stack"))
      status = (TraceStack = !TraceStack);
    else if (EqStr(form, "all") || Eq(form, SymbolT))
      status = TraceEnvironment = TraceEvaluator = TracePrimitives = TraceStack = true;
    else if (EqStr(form, "none") || Eq(form, SymbolNIL))
      status = TraceEnvironment = TraceEvaluator = TracePrimitives = TraceStack = false;
    else
      return ErrorForm("Trace():  Unknown feature: \"%s\"!\n", SymbolName(form));

    return status ? SymbolT : SymbolNIL;
}

Form *Exit(void)
{
    // Closing StandardInput will exit the top-level read-eval-print loop.
    fclose(StandardInput);

    longjmp(TopLevelJmpBuf, kExitInterpreter);

    // (Never actually happens.)
    return SymbolNIL;
}

Form *FuncCar(void)
{
    Form *x = Pop();

    TRACE_PRIM("Car");

    return Car(x);
}

Form *FuncCdr(void)
{
    Form *x = Pop();

    TRACE_PRIM("Cdr");

    return Cdr(x);
}

Form *FuncCons(void)
{
    Form *y = Pop();
    Form *x = Pop();

    TRACE_PRIM("Cons");

    return Cons(x, y);
}

Form *FuncAtom(void)
{
    Form *x = Pop();

    TRACE_PRIM("IsAtom");

    return IsAtom(x) ? SymbolT : SymbolNIL;
}

Form *FuncEq(void)
{
    Form *y = Pop();
    Form *x = Pop();

    TRACE_PRIM("Eq");

    return Eq(x, y) ? SymbolT : SymbolNIL;
}

Form *FuncNull(void)
{
    Form *x = Pop();

    TRACE_PRIM("IsNull");

    return IsNull(x) ? SymbolT : SymbolNIL;
}

Form *FuncNumberp(void)
{
    Form *x = Pop();

    TRACE_PRIM("IsNumber");

    return IsNumber(x) ? SymbolT : SymbolNIL;
}

Form *FuncZerop(void)
{
    Form *x = Pop();

    TRACE_PRIM("Zerop");

    return Zerop(x);
}

Form *FuncNumberEqual(void)
{
    Form *y = Pop();
    Form *x = Pop();

    TRACE_PRIM("NumberEqual");

    return NumberEqual(x, y);
}

Form *FuncLess(void)
{
    Form *y = Pop();
    Form *x = Pop();

    TRACE_PRIM("Less");

    return Less(x, y);
}

Form *FuncLessEqual(void)
{
    Form *y = Pop();
    Form *x = Pop();

    TRACE_PRIM("LessEqual");

    return LessEqual(x, y);
}

Form *FuncGreater(void)
{
    Form *y = Pop();
    Form *x = Pop();

    TRACE_PRIM("Greater");

    return Greater(x, y);
}

Form *FuncGreaterEqual(void)
{
    Form *y = Pop();
    Form *x = Pop();

    TRACE_PRIM("GreaterEqual");

    return GreaterEqual(x, y);
}

Form *FuncPlus(void)
{
    Form *y = Pop();
    Form *x = Pop();

    TRACE_PRIM("Plus");

    return Plus(x, y);
}

Form *FuncMinus(void)
{
    Form *y = Pop();
    Form *x = Pop();

    TRACE_PRIM("Minus");

    return Minus(x, y);
}

Form *FuncTimes(void)
{
    Form *y = Pop();
    Form *x = Pop();

    TRACE_PRIM("Times");

    return Times(x, y);
}

Form *FuncDivide(void)
{
    Form *y = Pop();
    Form *x = Pop();

    TRACE_PRIM("Divide");

    return Divide(x, y);
}

Form *FuncRemainder(void)
{
    Form *y = Pop();
    Form *x = Pop();

    TRACE_PRIM("Remainder");

    return Remainder(x, y);
}

Form *FuncAdd1(void)
{
    Form *x = Pop();

    TRACE_PRIM("Add1");

    return Add1(x);
}

Form *FuncSub1(void)
{
    Form *x = Pop();

    TRACE_PRIM("Sub1");

    return Sub1(x);
}

Form *FuncLn(void)
{
    Form *x = Pop();

    TRACE_PRIM("Ln");

    return Ln(x);
}

Form *FuncExp(void)
{
    Form *x = Pop();

    TRACE_PRIM("Exp");

    return Exp(x);
}

Form *FuncLog(void)
{
    Form *y = Pop();
    Form *x = Pop();

    TRACE_PRIM("Log");

    return Log(x, y);
}

Form *FuncExpt(void)
{
    Form *y = Pop();
    Form *x = Pop();

    TRACE_PRIM("Expt");

    return Expt(x, y);
}

Form *FuncSin(void)
{
    Form *x = Pop();

    TRACE_PRIM("Sin");

    return Sine(x);
}

Form *FuncCos(void)
{
    Form *x = Pop();

    TRACE_PRIM("Cos");

    return Cosine(x);
}

Form *FuncTan(void)
{
    Form *x = Pop();

    TRACE_PRIM("Tan");

    return Tangent(x);
}

Form *FuncEqual(void)
{
    Form *y = Pop();
    Form *x = Pop();

    TRACE_PRIM("Equal");

    return Equal(x, y);
}

Form *FuncSet(void)
{
    Form *y = Pop();
    Form *x = Pop();

    TRACE_PRIM("Set");

    return Set(x, y);
}

Form *FuncTrace(void)
{
    Form *x = Pop();

    TRACE_PRIM("Trace");

    return Trace(x);
}

Form *FuncExit(void)
{
    TRACE_PRIM("Exit");

    return Exit();
}

Form *FuncRead(void)
{
    TRACE_PRIM("Read");

    return Read(StandardInput);
}

Form *FuncEval(void)
{
    Form *x = Pop();

    TRACE_PRIM("Eval");

    // XXX -- Neither of these is quite right.  The intent of the "TopLevelEnv"
    //         here is to be the current global environment, while "CurrentEnv"
    //         is the current evaluation environment within the interpereter.
    //         That is, it includes local (i.e., lambda) bindings.  For now
    //         let's use that environment instead, since the top-level
    //         environment is really just the initial environment.  Be sure to
    //         change it back when it really is the current global environment.
    // return Eval(x, TopLevelEnv);
    return Eval(x, CurrentEnv);
}

Form *FuncApply(void)
{
    Form *args = Pop();
    Form *func = Pop();

    TRACE_PRIM("Apply");

    // XXX -- Neither of these is quite right. (See comments in "FuncEval()" above.)
    // return Apply(func, args, TopLevelEnv);
    return Apply(func, args, CurrentEnv);
}

Form *FuncPrin1(void)
{
    Form *x = Pop();

    TRACE_PRIM("Prin1");

    Print(x, StandardOutput);

    return x;
}

Form *FuncPrint(void)
{
    Form *x = Pop();

    TRACE_PRIM("Print");

    fprintf(StandardOutput, "\n");
    Print(x, StandardOutput);
    fprintf(StandardOutput, " ");

    return x;
}
