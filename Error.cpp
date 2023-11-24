/*
    File:   Error.cpp
    Author: ***PSI***
    Date:   Sun Apr 16 22:36:28 2000

    Description:
       Psil error functions.

    Copyright (c) 2000-2019 Joseph J. Mankoski ***PSI***
*/


// Include declarations files.


#include <stdarg.h>
#include <stdlib.h>
#include "Psil.h"


// Define constants.


const int kFormatStringLength = 256;
const int kErrorOutExitCode   = -1;


// Define functions.


void DPrintf(const char *format, ...)
{
#if DEBUG
    va_list args;

    va_start(args, format);
    vfprintf(StandardOutput, format, args);
    va_end(args);
#endif // DEBUG
}

void Message(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    vfprintf(StandardOutput, format, args);
    va_end(args);
}

void VError(const char *format, va_list args)
{
    char errorFormat[kFormatStringLength];
    const char errmsg[] = ";;; Error!  %s";

    snprintf(errorFormat,kFormatStringLength - sizeof(errmsg), errmsg, format);
    vfprintf(StandardError, errorFormat, args);
}

int Error(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    VError(format, args);
    va_end(args);

    return kPsilError;
}

void ErrorOut(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    VError(format, args);
    va_end(args);

    exit(kErrorOutExitCode);
}

Form *ErrorForm(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    VError(format, args);
    va_end(args);

    longjmp(TopLevelJmpBuf, kCaughtError);

    // (Never actually happens.)
    return SymbolNIL;
}
