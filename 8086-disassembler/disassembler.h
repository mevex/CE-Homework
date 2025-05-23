#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H
#include "defines.h"

struct Stream
{
    byte *Data;
    u32 Size;

    word BitsBuffer;
    u16 BitsBufferCount;
};

enum ByteRegister
{
    AL,
    CL,
    DL,
    BL,
    AH,
    CH,
    DH,
    BH
};

enum WordRegister
{
    AX,
    CX,
    DX,
    BX,
    SP,
    BP,
    SI,
    DI
};

enum InstructionType
{
    NOCODE = 0,
    MOV
};

#endif // DISASSEMBLER_H