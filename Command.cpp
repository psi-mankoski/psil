/*
    File:   Command.cpp
    Author: ***PSI***
    Date:   Wed Nov 22 21:14:34 2023

    Description:
       Provide support for top level ("colon") commands.

       Commands are introduced via the colon (':') character and pass the whole line to the command function.
       The function is selected by the first whitespace-delimited token, case-independently.
*/


// Include declarations files.


#include <ctype.h>
#include <string.h>
#include "Psil.h"


// Forward references.


bool CmdHelp(char *cmd_line);
bool CmdEnv(char *cmd_line);


// Define global variables.


static PsilCommand Commands[] =
{
    {"HELP",   CmdHelp,   "Print help for Psil commands."},
    {"ENV",    CmdEnv,    "Print main Psil environments."},
    {NULL,     NULL,      NULL}
};


// Define functions.


bool DoCommand(char *cmd_line)
{
    PsilCommand *cmd = Commands;

    DPrintf("DoCommand(): Command line = \"%s\"\n", cmd_line);

    while (cmd->name != NULL) {
        if (!strncmp(cmd->name, cmd_line, strlen(cmd->name))) {
            return cmd->func(cmd_line);
        }
        cmd++;
    }
    Error("Unknown command: \"%s\"!\n", cmd_line);
    return false;
}

char *ReadCommand(FILE *instream, char *cmd_line)
{
    char c;
    int index = 0;

    cmd_line[index] = '\0';

    while (((c = fgetc(instream)) != EOF) && (index < kMaxTokenLen)
           && (c != kReturn) && (c != kNewline))
      cmd_line[index++] = c;
    cmd_line[index] = '\0';

    char *chr = cmd_line;
    while (*chr && (*chr != kSpace) && (*chr != kTab)) {
        *chr = toupper(*chr);
        chr++;
    }

    return cmd_line;
}

bool CmdHelp(char *cmd_line)
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

bool CmdEnv(char *cmd_line)
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
