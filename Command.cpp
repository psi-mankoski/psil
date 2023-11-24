/*
    File:   Command.cpp
    Author: ***PSI***
    Date:   Wed Nov 22 21:14:34 2023

    Description:

*/


// Include declarations files.


#include <ctype.h>
#include <string.h>
#include "Psil.h"


// Forward references.


bool CmdHelp();
bool CmdEnv();


// Define global variables.


static PsilCommand Commands[] =
{
    {"HELP",   CmdHelp,   "Print help for Psil commands."},
    {"ENV",    CmdEnv,    "Print main Psil environments."},
    {NULL,     NULL,      NULL}
};


// Define functions.


bool DoCommand(char *cmdstr)
{
    PsilCommand *cmd = Commands;
    while (cmd->name != NULL) {
        if (!strcmp(cmd->name, cmdstr)) {
            return cmd->func();
        }
        cmd++;
    }
    Error("Unknown command: \"%s\"!\n", cmdstr);
    return false;
}

char *ReadCommand(FILE *instream)
{
    // Note:  Not re-entrant!
    static char cmd[kMaxTokenLen];
    char c;
    int index = 0;

    cmd[index] = '\0';

    while (((c = fgetc(instream)) != EOF)
           && ((c != kSpace) && (c != kTab) && (c != kReturn) && (c != kNewline)))
      cmd[index++] = c;

    cmd[index] = '\0';
    char *chr = cmd;
    while (*chr) {
        *chr = toupper(*chr);
        chr++;
    }

    return cmd;
}

bool CmdHelp()
{
    PsilCommand *cmd = Commands;
    Message("Psil Commands:\n");
    Message("  Command\t\tDocumentation\n");
    Message("---------------------------------------------------\n");
    while (cmd->name != NULL) {
        Message("  %s\t\t%s\n", cmd->name, cmd->doc);
        cmd++;
    }
    Message("---------------------------------------------------\n");

    return true;
}

bool CmdEnv()
{
    Message("Psil Environment:\n");
    Message("TopLevelEnv:\n");
    PrintEnvironment(TopLevelEnv, StandardOutput, true);
    Message("\n");
    Message("CurrentEnv:\n");
    PrintEnvironment(CurrentEnv, StandardOutput, true);
    Message("\n");
    return true;
}
