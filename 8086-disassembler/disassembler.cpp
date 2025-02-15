#include "disassembler.h"
#include <stdio.h>
#include "fileio.h"

byte GetBits(Stream &Instructions, u32 Count)
{
    // NOTE(valentino): I don't want to grab more than one byte at a time
    if (Count > 8)
    {
        BreakHere;
        return 0;
    }

    // NOTE(valentino): If the buffer doesn't hold
    if (Instructions.BitsBufferCount < Count)
    {
        if (Instructions.Size != 0)
        {
            byte *NewByte = Instructions.Data;
            Instructions.BitsBuffer |= *NewByte << Instructions.BitsBufferCount;
            Instructions.BitsBufferCount += 8;

            Instructions.Data++;
            Instructions.Size--;
        }
        else
        {
            BreakHere;
            return 0;
        }
    }

    byte Bitmask = 1 << Count;
    Bitmask--;

    byte Result = Instructions.BitsBuffer & Bitmask;

    Instructions.BitsBuffer = Instructions.BitsBuffer >> Count;
    Instructions.BitsBufferCount -= (u16)(Count);

    return Result;
}

byte GetByte(Stream &Instructions)
{
    byte Result = *Instructions.Data;

    Instructions.Data++;
    Instructions.Size--;

    return Result;
}

void PrintRegister(u8 Register, b8 Wide)
{
    // NOTE(valentino): Since the encoding for the registers are the same
    // I noted the cases that way so that they're easier to read for
    // both the encodings. The compiler won't care.
    switch (Register)
    {
        case AL &AX:
        {
            if (Wide)
                printf(Str(ax));
            else
                printf(Str(al));
            break;
        }
        case CL &CX:
        {
            if (Wide)
                printf(Str(cx));
            else
                printf(Str(cl));
            break;
        }
        case DL &DX:
        {
            if (Wide)
                printf(Str(dx));
            else
                printf(Str(dl));
            break;
        }
        case BL &BX:
        {
            if (Wide)
                printf(Str(bx));
            else
                printf(Str(bl));
            break;
        }
        case AH &SP:
        {
            if (Wide)
                printf(Str(sp));
            else
                printf(Str(ah));
            break;
        }
        case CH &BP:
        {
            if (Wide)
                printf(Str(bp));
            else
                printf(Str(ch));
            break;
        }
        case DH &SI:
        {
            if (Wide)
                printf(Str(si));
            else
                printf(Str(dh));
            break;
        }
        case BH &DI:
        {
            if (Wide)
                printf(Str(di));
            else
                printf(Str(bh));
            break;
        }
        default:
        {
            BreakHere;
        }
    }
}

void DecodeMov(u8 CurrentByte, Stream &Instructions)
{
    b8 DestIsReg = (CurrentByte & 0b10) >> 1;
    b8 Wide = (CurrentByte & 1);

    CurrentByte = GetByte(Instructions);
    u8 Mod = CurrentByte >> 6;
    u8 Reg = (CurrentByte & 0b111000) >> 3;
    u8 RM = (CurrentByte & 0b111);

    u8 Dest;
    u8 Src;
    if (DestIsReg)
    {
        Dest = Reg;
        Src = RM;
    }
    else
    {
        Dest = RM;
        Src = Reg;
    }

    if (Mod == 0b11)
    {
        // Register mode
        printf("mov ");
        PrintRegister(Dest, Wide);
        printf(", ");
        PrintRegister(Src, Wide);
        printf("\n");
    }
}

void Disassemble(char *Filename)
{
    FileHandle File = GetAllDataFromFile(Filename);
    Stream Instructions = {};
    Instructions.Data = (byte *)(File.Data);
    Instructions.Size = File.Size;

    while (Instructions.Size)
    {
        byte Byte = GetByte(Instructions);
        u8 OpCode = Byte >> 2;

        if (OpCode == MOV)
        {
            DecodeMov(Byte, Instructions);
        }
    }

    FileFree(File);
}