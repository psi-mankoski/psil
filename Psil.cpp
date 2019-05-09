/*
    File:   Psil.cpp
    Author: ***PSI***
    Date:   Mon Jul 19 20:33:28 1999

    Description:
        Psil:  A simple LISP interpreter in C++.

    Copyright (c) 1999-2019 Joseph J. Mankoski ***PSI***
*/


// Include declarations files.


#include <string.h>
#include <unistd.h>
#include "Psil.h"


// Define global variables.


FILE *StandardInput  = NULL;
FILE *StandardOutput = NULL;
FILE *StandardError  = NULL;

jmp_buf TopLevelJmpBuf;

bool IsInteractive = true;

bool InterpreterRunning = true;


// Define functions.


int FileIsOpen(FILE *file)
{
    return !feof(file) && (fileno(file) >= 0) && !ReadEOF;
}

int ParseFile(char *filename)
{
    int status = kPsilOK;

    if (InitializeReader() != kPsilOK)
      return Error("ParseFile(\"%s\"):  InitializeReader() failed!\n");

    Environment *env = InitializeEnvironment();

    if (InitializeStack() != kPsilOK)
      return Error("ParseFile(\"%s\"):  InitializeStack() failed!\n");

    if (!strcmp(filename, "-"))
      StandardInput = stdin;
    else if ((StandardInput = fopen(filename, "r")) == NULL)
      return Error("ParseFile(\"%s\"):  Failed fopen()!\n", filename);

    StandardOutput = stdout;
    StandardError  = stderr;

    CurrentEnv = TopLevelEnv = env;

    InterpreterRunning = true;

    if ((IsInteractive = isatty(fileno(StandardInput))))
      Message(";;; Welcome to Psil v%s!\n", kPsilVersion);

    while (InterpreterRunning && FileIsOpen(StandardInput)) {
        int retval, stackPtr = SaveStack();
        if (IsInteractive)
          Message("==> ");
        if (!(retval = setjmp(TopLevelJmpBuf))) {
            status = Print(Eval(Read(StandardInput), CurrentEnv), StandardOutput);
            fprintf(StandardOutput, "\n");
        } else if (retval == kCaughtError) {
            if (IsInteractive)
              Message(";;; Psil Top-Level Restarted!\n");
            RestoreStack(stackPtr);
        } else if (retval == kExitInterpreter) {
            if (IsInteractive)
              Message(";;; Exiting Psil.\n");
            InterpreterRunning = false;
            DeInitializeEnvironment(CurrentEnv);
            CurrentEnv = TopLevelEnv = NULL;
            DeInitializeReader();
        }
    }

    if (!strcmp(filename, "-"))
      fclose(StandardInput);

    return status;
}

int main(int argc, char *argv[])
{
    char *filename;

    if (argc == 1) {
        filename = (char *) "-";
    } else if (argc == 2) {
        filename = argv[1];
    } else {
        fprintf(stderr, "Usage:  %s [<Filename>]\n", argv[0]);
        fprintf(stderr, "  Evaluate Psil forms from standard input or the contents of <Filename>, if supplied.\n");
        return kPsilError;
    }

    return ParseFile(filename);
}
