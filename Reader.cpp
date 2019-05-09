/*
    File:   Reader.cpp
    Author: ***PSI***
    Date:   Mon Jul 19 20:34:10 1999

    Description:
       Psil reader.

       XXX -- Features Currently Missing:
       ----------------------------------
        - Support for backslash, backquote, etc.
        - Support for reading dotted lists.
        - Support for reading strings.
        - Support for readtables.

    Copyright (c) 1999-2019 Joseph J. Mankoski ***PSI***
*/


// Include declarations files.


#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#ifdef __sparc
#include <alloca.h>
#endif
#include "Psil.h"


// Define constants.


// Initial size of the symbol hash table.
static const int kInitialSymbolHashTableSize = 101;


// Define global variables.


// Has the Psil reader been initialized?
static bool ReaderInitialized = false;

static const char *kNIL    = "NIL";
static const char *kT      = "T";
static const char *kQUOTE  = "QUOTE";
static const char *kLAMBDA = "LAMBDA";
static const char *kIF     = "IF";
static const char *kSETQ   = "SETQ";
static const char *kDEFINE = "DEFINE";

Form *SymbolNIL;
Form *SymbolT;
Form *SymbolQUOTE;
Form *SymbolLAMBDA;
Form *SymbolIF;
Form *SymbolSETQ;
Form *SymbolDEFINE;

// Has an EOF character been read?
bool ReadEOF = false;

// Define functions.


void MakeStandardSymbols(void)
{
    SymbolNIL = Intern(kNIL);
    SymbolT = Intern(kT);
    SymbolQUOTE = Intern(kQUOTE);
    SymbolLAMBDA = Intern(kLAMBDA);
    SymbolIF = Intern(kIF);
    SymbolSETQ = Intern(kSETQ);
    SymbolDEFINE = Intern(kDEFINE);
}

int InitializeReader(void)
{
    if (hcreate(kInitialSymbolHashTableSize)) {
        MakeStandardSymbols();
        ReaderInitialized = true;
        return kPsilOK;
    } else
      return Error("InitializeReader():  Failed to create token hash table!\n");
}

void DeInitializeReader(void)
{
    if (ReaderInitialized) {
        // XXX -- Should also release the "strdup()"'d tokens here!
        hdestroy();
        ReaderInitialized = false;
    }
}

Form *Intern(const char *token)
{
    // Normalize the token by making a temporary copy on the stack....
    int tokenLen = strlen(token);
    char *tokenUppercase = (char *) alloca(tokenLen + 1);
    char *tokenUppercasePtr = tokenUppercase;

    // And converting it to uppercase.
    while ((*tokenUppercasePtr++ = toupper(*token++)))
      ;

    // Search for a symbol having the normalized token as its pname.
    ENTRY tokenEntry, *tokenEntryPtr;
    tokenEntry.key = tokenUppercase;
    if ((tokenEntryPtr = hsearch(tokenEntry, FIND)) == NULL) {
        // If not found, make a new symbol and add it to the symbol hash table.
        tokenEntry.key = strdup(tokenUppercase);
        tokenEntry.data = (char *) MakeSymbol((const char *) tokenEntry.key);
        if ((tokenEntryPtr = hsearch(tokenEntry, ENTER)) == NULL)
          ErrorOut("Intern():  Failed to add token \"%s\" to the symbol hash table!\n");
    }

    // Return the unique symbol.
    return (Form *) tokenEntryPtr->data;
}

bool ParseInteger(const char *token, int &integer)
{
    bool success = false, sawDigit = false;
    const char *ptr = token;

    // Look for optional leading sign.
    if ((*ptr == '+') || (*ptr == '-'))
      ptr++;

    // Look for a sequence of decimal digits.
    while (*ptr && isdigit(*ptr)) {
        sawDigit = true;
        ptr++;
    }

    // Look for an optional trailing decimal point.
    if (sawDigit && (*ptr == '.'))
      ptr++;

    // Check for successfully reaching the end of the token.
    if (sawDigit && !*ptr) {
        success = true;
        integer = atoi(token);
    }

    return success;
}

bool ParseFlonum(const char *token, double &flonum)
{
    bool success = false, sawDigit = false, sawPoint = false;
    const char *ptr = token;

    // Look for optional leading sign.
    if ((*ptr == '+') || (*ptr == '-'))
      ptr++;

    // Look for a sequence of decimal digits, with an optional decimal point.
    while (*ptr && (isdigit(*ptr) || (*ptr == '.'))) {
        // Look for one and only one optional decimal point.
        if (*ptr == '.') {
            if (sawPoint)
              return false;
            else
              sawPoint = true;
        } else {
            sawDigit = true;
        }
        ptr++;
    }

    // Look for optional exponent.
    if (sawDigit && ((*ptr == 'e') || (*ptr == 'E'))) {
        ptr++;

        // Look for optional exponent sign.
        if ((*ptr == '+') || (*ptr == '-'))
          ptr++;

        // Require at least one digit in the exponent.
        if (!isdigit(*ptr))
          return false;

        // XXX -- This allows any size of exponent > 1 digit!
        while (*ptr && isdigit(*ptr))
          ptr++;
    }

    // Check for successfully reaching the end of the token.
    if (sawDigit && !*ptr) {
        success = true;
        flonum = atof(token);
    }

    return success;
}

// XXX -- Comments are not really NOPs, since they should
//         actually terminate any current token.
char ReadChar(FILE *instream)
{
    char c;

 StartOver:
    ReadEOF = ((c = fgetc(instream)) == EOF);

    // If a comment character is found, read it until the next newline.
    if (c == kCommentChar) {
        while (((c = fgetc(instream)) != '\n') && (c != EOF))
          ;
        if (c == EOF)
          return c;
        else
          goto StartOver;
    }

    return c;
}

char ReadToken(FILE *instream, char *token)
{
    char c;
    int index = 0;

    token[index] = '\0';

    // Read any leading whitespace.
    while (((c = ReadChar(instream)) != EOF)
           && ((c == kSpace) || (c == kTab) || (c == kReturn) || (c == kNewline)))
      ;

    if (c == EOF)
      return EOF;

    // XXX -- Still need to handle backslash, backquote, etc.

    // Accumulate the character.
    token[index++] = c;

    // Return token if we have it all now.
    if ((c == kLeftParen) || (c == kRightParen) || (c == kQuote)) {
        token[index] = '\0';
        return c;
    }

    while (((c = ReadChar(instream)) != EOF) && (c != kSpace) && (c != kTab)
           && (c != kReturn) && (c != kNewline) && (c != kLeftParen)
           && (c != kRightParen) && (c != kQuote))
      token[index++] = c;

    // Push the last character back on the input stream.
    ungetc(c, instream);

    token[index] = '\0';

    // XXX -- What should this case return?
    return '\0';
}


Form *ReadAtom(const char *token)
{
    int integer;
    double flonum;
    Form *form;

    if (token == NULL) {
        return SymbolNIL;
    } else if (ParseInteger(token, integer)) {
        form = MakeInteger(integer);
    } else if (ParseFlonum(token, flonum)) {
        form = MakeFlonum(flonum);
    } else {
        form = Intern(token);
    }

    return form;
}


Form *ReadList(FILE *instream)
{
    char token[kMaxTokenLen], c;

    c = ReadToken(instream, token);

    DPrintf("ReadList():  ***Found token: |%s|***\n", token);

    if (c == EOF) {
        Error("ReadList():  EOF within list!\n");
        return SymbolNIL;
    } else if (c == kQuote) {
        DPrintf("ReadList():  Encountered quote.\n");
        // N.B.:  Need to read before consing to guarantee proper ordering.
        Form *x = Read(instream);
        Form *y = ReadList(instream);
        return Cons(Cons(SymbolQUOTE, Cons(x, SymbolNIL)), y);
    } else if (c == kLeftParen) {
        DPrintf("Sublist ReadList()...\n");
        // Start a new sub-list.
        // N.B.:  Need to read before consing to guarantee proper ordering.
        Form *x = ReadList(instream);
        Form *y = ReadList(instream);
        return Cons(x, y);
    } else if (c == kRightParen) {
        DPrintf("Endlist ReadList()...\n");
        // End the current list.
        return SymbolNIL;
    } else {
        DPrintf("Recursive ReadList()...\n");
        return Cons(ReadAtom(token), ReadList(instream));
    }
}

Form *Read(FILE *instream)
{
    char token[kMaxTokenLen], c;

    c = ReadToken(instream, token);

    if (c == EOF) {
        if (IsInteractive)
          Message("\n");
        longjmp(TopLevelJmpBuf, kExitInterpreter);
    }

    DPrintf("Read():  ***Found token: |%s|***\n", token);

    if (!strcasecmp(token, "NIL"))
      return SymbolNIL;

    if (c == kQuote) {
        DPrintf("Read():  Encountered quote.\n");
        return Cons(SymbolQUOTE, Cons(Read(instream), SymbolNIL));
    } else if (c == kLeftParen) {
        return ReadList(instream);
    } else if (c == kRightParen) {
        // XXX -- Need to handle error conditions better than this.
        Error("Read():  Read ')' outside of list!\n");
        return SymbolNIL;
    } else
      return ReadAtom(token);
}
