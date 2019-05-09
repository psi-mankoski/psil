/*
    File:   Stack.cpp
    Author: ***PSI***
    Date:   Sat Apr 22 15:05:49 2000

    Description:
       Psil stack machine implementation.

    Copyright (c) 2000-2019 Joseph J. Mankoski ***PSI***
*/


// Include declarations files.


#include <stdlib.h>
#include "Psil.h"


// Define constants.


const int kStackSize = 100000;


// Define global variables.


static Form **Stack;

static int SP;

bool TraceStack = false;


// Define functions.


int InitializeStack(void)
{
    if ((Stack = (Form **) malloc(kStackSize * sizeof(Form *)))) {
        ResetStack();
        return kPsilOK;
    } else
      return Error("InitializeStack():  Failed to allocate a stack of size %d!\n", kStackSize);
}

int SaveStack(void)
{
    return SP;
}

void RestoreStack(int sp)
{
    SP = sp;
}

void ResetStack(void)
{
    RestoreStack(0);
}

Form *Push(Form *form)
{
    if (TraceStack) {
        fprintf(StandardError, "STACK: Push(");
        Print(form, StandardError);
        fprintf(StandardError, ")\n");
    }

    if (SP >= kStackSize)
      return ErrorForm("Push():  Stack overflow!\n");
    else
      return Stack[SP++] = form;
}

Form *Pop(void)
{
    if (TraceStack) {
        if (SP) {
            fprintf(StandardError, "STACK: Pop(");
            Print(Stack[SP - 1], StandardError);
            fprintf(StandardError, ")\n");
        } else
          fprintf(StandardError, "Pop():  *Stack Underflow!*\n");
    }

    if (SP)
      return Stack[--SP];
    else
      return ErrorForm("Pop():  Stack underflow!\n");
}
