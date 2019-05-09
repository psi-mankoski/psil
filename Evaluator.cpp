/*
    File:   Evaluator.cpp
    Author: ***PSI***
    Date:   Mon Jul 19 20:35:32 1999

    Description:
       Psil evaluator.

    Copyright (c) 1999-2019 Joseph J. Mankoski ***PSI***
*/


// Include declarations files.


#include "Psil.h"


// Define global variables.


bool TraceEvaluator = false;


// Define functions.


Environment *BindArgs(Form *arglist, Form *args, Environment *env)
{
    if (TraceEvaluator) {
        fprintf(StandardError, "EVAL: BindArgs(");
        Print(arglist, StandardError);
        fprintf(StandardError, ", ");
        Print(args, StandardError);
        fprintf(StandardError, ", ");
        PrintEnvironment(env, StandardError);
        fprintf(StandardError, ")\n");
    }

    if (IsNull(arglist) || IsNull(args)) {
        if (!IsNull(args))
          Error("BindArgs():  Ignoring extra arguments!\n");
        else if (!IsNull(arglist)) {
            Error("BindArgs():  Binding missing arguments to NIL!\n");
            while (!IsNull(arglist)) {
                Form *argName = Car(arglist);
                if (IsSymbol(argName))
                  env = Bind(SymbolName(argName), SymbolNIL, env);
                else
                  Error("BindArgs():  Ignoring extra non-symbol argument!\n");
                arglist = Cdr(arglist);
            }
        }
        return env;
    } else {
        Form *argName = Car(arglist);
        if (IsSymbol(argName)) {
            Form *argValue = Eval(Car(args), CurrentEnv);
            return BindArgs(Cdr(arglist), Cdr(args),
                            Bind(SymbolName(argName), argValue, env));
        } else {
            Error("BindArgs():  Non-symbol argument!\n");
            return env;
        }
    }
}

bool EvalArgs(Form *args, Environment *env)
{
    if (TraceEvaluator) {
        fprintf(StandardError, "EVAL: EvalArgs(");
        Print(args, StandardError);
        fprintf(StandardError, ", ");
        PrintEnvironment(env, StandardError);
        fprintf(StandardError, ")\n");
    }

    while (!IsNull(args)) {
        if (!IsCons(args)) {
            Error("EvalArgs():  Invalid argument list!\n");
            return false;
        }
        Push(Eval(Car(args), env));
        args = Cdr(args);
    }

    return true;
}

Form *EvalBody(Form *forms, Environment *env)
{
    Form *retval, *head, *tail;

    if (TraceEvaluator) {
        fprintf(StandardError, "EVAL: EvalBody(");
        Print(forms, StandardError);
        fprintf(StandardError, ", ");
        PrintEnvironment(env, StandardError);
        fprintf(StandardError, ")\n");
    }

    if (IsNull(forms))
      return SymbolNIL;

    head = Car(forms);
    tail = Cdr(forms);

    retval = Eval(head, env);

    if (IsNull(tail))
      return retval;

    return EvalBody(tail, env);
}

Form *Eval(Form *form, Environment *env)
{
    if (TraceEvaluator) {
        fprintf(StandardError, "EVAL: Eval(");
        Print(form, StandardError);
        fprintf(StandardError, ", ");
        PrintEnvironment(env, StandardError);
        fprintf(StandardError, ")\n");
    }

    if (IsNull(form)) {
        return SymbolNIL;
    } else if (IsAtom(form)) {
        if (IsNumber(form) || IsString(form))
          return form;
        else
          return Lookup(SymbolName(form), env);
    } else if (IsCons(form)) {
        if (IsQuote(Car(form)))
          return Cadr(form);
        else if (IsIf(Car(form)))
          return Eval((Eval(Cadr(form), env) != SymbolNIL ? Caddr(form) : Cadddr(form)), env);
        else if (IsLambda(Car(form)))
          return MakeClosure(Cadr(form), Cddr(form), env);
        else if (IsAssignment(form)) {
            Form *value = Eval(Caddr(form), env);
            CurrentEnv = SetBinding(SymbolName(Cadr(form)), value, env);
            return value;
        } else if (IsDefinition(form)) {
            // Note:  This supports recursive function definitions, but it allows:
            //        "(define x x)" ==> x := NIL, and also:
            //        "(define x 1) (define x y)" ==> x := NIL if "y" is undefined!
            env = SetBinding(SymbolName(Cadr(form)), SymbolNIL, env);
            Form *value = Eval(Caddr(form), env);
            CurrentEnv = SetBinding(SymbolName(Cadr(form)), value, env);
            return value;
        } else
          return Apply(Car(form), Cdr(form), env);
    } else {
        return ErrorForm("Eval():  Invalid form type!\n");
    }
}

Form *Apply(Form *func, Form *args, Environment *env)
{
    Form *retval;

    if (TraceEvaluator) {
        fprintf(StandardError, "EVAL: Apply(");
        Print(func, StandardError);
        fprintf(StandardError, ", ");
        Print(args, StandardError);
        fprintf(StandardError, ", ");
        PrintEnvironment(env, StandardError);
        fprintf(StandardError, ")\n");
    }

    if (IsFunc(func)) {
        PsilFunc *pfunc = FuncValue(func);
        if (!EvalArgs(args, env))
          return ErrorForm("Apply():  EvalArgs() failed!\n");
        else
          return (pfunc->func)();
    } else if (IsClosure(func)) {
        Form *arglist = LambdaArglist(func);
        Form *body = LambdaBody(func);
        Environment *lexicalEnv = LambdaEnvironment(func);
        Environment *origEnv = env;
        CurrentEnv = env;
        retval = EvalBody(body, CurrentEnv = BindArgs(arglist, args, lexicalEnv));
        CurrentEnv = origEnv;
        return retval;
    } else if (IsLambdaForm(func)) {
        Form *arglist = Cadr(func);
        Form *body = Cddr(func);
        Environment *origEnv = env;
        CurrentEnv = env;
        retval = EvalBody(body, CurrentEnv = BindArgs(arglist, args, env));
        while (CurrentEnv && (CurrentEnv != origEnv))
          CurrentEnv = Unbind(CurrentEnv);
        return retval;
    } else if (IsCons(func)) {
        return Apply(Eval(func, env), args, env);
    } else if (IsSymbol(func)) {
        Form *symbolValue = Lookup(SymbolName(func), env);
        if (symbolValue != SymbolNIL)
          return Apply(symbolValue, args, env);
        else
          return ErrorForm("Apply():  Undefined function symbol: \"%s\"!\n", SymbolName(func));
    } else
      return ErrorForm("Apply():  Invalid function object!\n");
}
