/*
    File:   Printer.cpp
    Author: ***PSI***
    Date:   Mon Jul 19 20:35:19 1999

    Description:
       Psil printer.

    Copyright (c) 1999-2019 Joseph J. Mankoski ***PSI***
*/


// Include declarations files.


#include "Psil.h"


// Define functions.


int PrintList(Form *form, FILE *outstream)
{
    Form *car = Car(form);
    Form *cdr = Cdr(form);

    fprintf(outstream, " ");

    Print(car, outstream);

    if (IsAtom(cdr) && !IsNull(cdr)) {
        fprintf(outstream, " . ");
        Print(cdr, outstream);
    } else if (!IsNull(cdr))
      PrintList(cdr, outstream);

    return kPsilOK;
}

int Print(Form *form, FILE *outstream)
{
    if (IsNull(form)) {
        fprintf(outstream, "NIL");
    } else if (IsSymbol(form)) {
        fprintf(outstream, "%s", SymbolName(form));
    } else if (IsInteger(form)) {
        fprintf(outstream, "%d", IntegerValue(form));
    } else if (IsFlonum(form)) {
        fprintf(outstream, "%f", FlonumValue(form));
    } else if (IsString(form)) {
        fprintf(outstream, "\"%s\"", StringValue(form));
    } else if (IsClosure(form)) {
        fprintf(outstream, "#<(LAMBDA ");
        Print(LambdaArglist(form), outstream);
        fprintf(outstream, " ");
        Form *lambdaBody = LambdaBody(form);
        while (!IsNull(lambdaBody)) {
            Print(Car(lambdaBody), outstream);
            lambdaBody = Cdr(lambdaBody);
        }
        fprintf(outstream, ") ");
        PrintEnvironment(LambdaEnvironment(form), outstream, TraceEnvironment);
        fprintf(outstream, ">");
    } else if (IsCons(form)) {
        fprintf(outstream, "(");
        Print(Car(form), outstream);
        Form *cdr = Cdr(form);
        if (IsNull(cdr))
          fprintf(outstream, ")");
        else if (!IsCons(cdr)) {
            fprintf(outstream, " . ");
            Print(cdr, outstream);
            fprintf(outstream, ")");
        } else {
            PrintList(cdr, outstream);
            fprintf(outstream, ")");
        }
    } else if (IsString(form)) {
        fprintf(outstream, "\"%s\"", StringValue(form));
    } else if (IsFunc(form)) {
        PsilFunc *func = FuncValue(form);
        fprintf(outstream, "#<Func %s nargs:%d>", func->name, func->nargs);
    } else
      return Error("Print():  Invalid form type!\n");

    return kPsilOK;
}
