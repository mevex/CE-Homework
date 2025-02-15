#include <stdio.h>
#include <string.h>

#include "disassembler.cpp"

int main(int ArgumentCount, char **Arguments)
{
    if (ArgumentCount == 3)
    {
        if (!strcmp(Arguments[1], "dAsm"))
        {
            Disassemble(Arguments[2]);
        }
        else
        {
            printf("Usage: %s [Program Name] [Arguments]\n", Arguments[0]);
        }
    }
    else
    {
        printf("Usage: %s [Program Name] [Arguments]\n", Arguments[0]);
    }
}