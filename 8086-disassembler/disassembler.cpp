#include "disassembler.h"
#include <stdio.h>
#include "fileio.h"

byte GetByte(Stream &Instructions)
{
    byte Result = *Instructions.Data;

    Instructions.Data++;
    Instructions.Size--;

    return Result;
}

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
            byte NewByte = GetByte(Instructions);
            Instructions.BitsBuffer |= NewByte << Instructions.BitsBufferCount;
            Instructions.BitsBufferCount += 8;
        }
        else
        {
            BreakHere;
            return 0;
        }
    }

    byte Bitmask = (1 << Count) - 1;
    byte Result = Instructions.BitsBuffer & Bitmask;

    Instructions.BitsBuffer = Instructions.BitsBuffer >> Count;
    Instructions.BitsBufferCount -= (u16)(Count);

    return Result;
}

i16 GetSignedValue(Stream &Instructions, b8 IsWordLength)
{
    i16 Value = 0;

    if (IsWordLength)
    {
        i16 Data = GetByte(Instructions);
        Data |= GetByte(Instructions) << 8;
        Value = Data;
    }
    else
    {
        i8 Data = GetByte(Instructions);
        Value = Data;
    }

    return Value;
}

void PrintRegister(u8 Register, b8 IsWordLength)
{
    // NOTE(valentino): Since the encoding for the registers are the same
    // I noted the cases that way so that they're easier to read for
    // both the encodings. The compiler won't care.
    switch (Register)
    {
        case AL &AX:
        {
            if (IsWordLength)
                printf("ax");
            else
                printf("al");
            break;
        }
        case CL &CX:
        {
            if (IsWordLength)
                printf("cx");
            else
                printf("cl");
            break;
        }
        case DL &DX:
        {
            if (IsWordLength)
                printf("dx");
            else
                printf("dl");
            break;
        }
        case BL &BX:
        {
            if (IsWordLength)
                printf("bx");
            else
                printf("bl");
            break;
        }
        case AH &SP:
        {
            if (IsWordLength)
                printf("sp");
            else
                printf("ah");
            break;
        }
        case CH &BP:
        {
            if (IsWordLength)
                printf("bp");
            else
                printf("ch");
            break;
        }
        case DH &SI:
        {
            if (IsWordLength)
                printf("si");
            else
                printf("dh");
            break;
        }
        case BH &DI:
        {
            if (IsWordLength)
                printf("di");
            else
                printf("bh");
            break;
        }
        default:
        {
            BreakHere;
        }
    }
}

u32 GetInstructionType(byte FirstByte)
{
    u8 HighBits = FirstByte >> 4;
    switch (HighBits)
    {
        case 0b1000:
        case 0b1100:
        case 0b1011:
        case 0b1010:
        {
            return MOV;
            break;
        }
        default:
            BreakHere;
            return NOCODE;
    }
}

void DecodeAddressCalculation(u8 RM)
{
    switch (RM)
    {
        case 0b000:
        {
            printf("bx + si");
            break;
        }
        case 0b001:
        {
            printf("bx + di");
            break;
        }
        case 0b010:
        {
            printf("bp + si");
            break;
        }
        case 0b011:
        {
            printf("bp + di");
            break;
        }
        case 0b100:
        {
            printf("si");
            break;
        }
        case 0b101:
        {
            printf("di");
            break;
        }
        case 0b110:
        {
            printf("bp");
            break;
        }
        case 0b111:
        {
            printf("bx");
            break;
        }
        default:
            BreakHere;
    }
}

void DecodeMov(Stream &Instructions, u8 FirstByte)
{
    printf("mov ");

    u8 HighBits = FirstByte >> 4;
    u8 LowBits = FirstByte & 0b1111;

    if (HighBits == 0b1000)
    {
        if ((LowBits >> 2) == 0b10)
        {
            Assert(((FirstByte >> 2) & 0b11) == 0b10);

            // NOTE(valentino): Register/memory to/from register
            b8 DestIsReg = (FirstByte & 0b10) >> 1;
            b8 IsWordLength = (FirstByte & 1);

            byte SecondByte = GetByte(Instructions);
            u8 Mod = SecondByte >> 6;
            u8 Reg = (SecondByte & 0b111000) >> 3;
            u8 RM = (SecondByte & 0b111);

            if (Mod == 0b11)
            {
                u8 Dest = DestIsReg ? Reg : RM;
                u8 Src = DestIsReg ? RM : Reg;

                // Register mode
                PrintRegister(Dest, IsWordLength);
                printf(", ");
                PrintRegister(Src, IsWordLength);
                printf("\n");
                return;
            }

            i16 Displacement = 0;
            b8 IsDirectAddress = false;
            if (Mod == 0b00)
            {
                // No displacement*
                if (RM == 0b110)
                {
                    // Direct Address
                    Displacement = GetSignedValue(Instructions, true);
                    IsDirectAddress = true;
                }
            }
            else
            {
                // Mod == 0b01 -> 8-bit displacement
                // Mod == 0b10 -> 16-bit displacement
                Displacement = GetSignedValue(Instructions, Mod == 0b10);
            }

            if (DestIsReg)
            {
                PrintRegister(Reg, IsWordLength);
                printf(", ");
            }

            printf("[");
            if (IsDirectAddress)
            {
                printf("%u", Displacement);
            }
            else
            {
                DecodeAddressCalculation(RM);
                if (Displacement > 0)
                {
                    printf(" + %i", Displacement);
                }
                else if (Displacement < 0)
                {
                    printf(" - %i", -Displacement);
                }
            }
            printf("]");

            if (!DestIsReg)
            {
                printf(", ");
                PrintRegister(Reg, IsWordLength);
            }
        }
        else
        {
            // TODO(valentino): Register/memory to segment register. Low 1110
            // TODO(valentino): Segment register to register/memory. Low 1100
            Assert((LowBits >> 2) == 0b11);
            BreakHere;
        }
    }
    if (HighBits == 0b1100)
    {
        // NOTE(valentino): Immediate to register/memory
        Assert(LowBits >> 1 == 0b011);

        b8 IsWordLength = FirstByte & 1;

        byte SecondByte = GetByte(Instructions);
        u8 Mod = SecondByte >> 6;
        u8 RM = (SecondByte & 0b111);
        Assert(((SecondByte >> 3) & 0b111) == 0);

        printf("[");
        DecodeAddressCalculation(RM);
        if (Mod == 0b01 || Mod == 0b10)
        {
            i16 Displacement = GetSignedValue(Instructions, Mod == 0b10);
            if (Displacement > 0)
            {
                printf(" + %i", Displacement);
            }
            else if (Displacement < 0)
            {
                printf(" - %i", -Displacement);
            }
        }
        printf("], ");

        char *Size = IsWordLength ? "word" : "byte";
        i16 Immediate = GetSignedValue(Instructions, IsWordLength);
        printf("%s %i", Size, Immediate);
    }
    if (HighBits == 0b1011)
    {
        // NOTE(valentino): Immediate to register
        b8 IsWordLength = (FirstByte >> 3) & 1;
        u8 Reg = FirstByte & 0b111;

        i16 Immidiate = GetSignedValue(Instructions, IsWordLength);

        PrintRegister(Reg, IsWordLength);
        printf(", %i", Immidiate);
    }
    if (HighBits == 0b1010)
    {
        u8 IsWordLength = LowBits & 1;

        if (LowBits & 0b10)
        {
            // NOTE(valentino): Accumulator to memory
            i16 Value = GetSignedValue(Instructions, IsWordLength);
            printf("[%i], ax", Value);
        }
        else
        {
            // NOTE(valentino): Memory to accumulator
            i16 Value = GetSignedValue(Instructions, IsWordLength);
            printf("ax, [%i]", Value);
        }
    }

    printf("\n");
}

void Disassemble(char *Filename)
{
    FileHandle File = GetAllDataFromFile(Filename);
    Stream Instructions = {};
    Instructions.Data = (byte *)(File.Data);
    Instructions.Size = File.Size;

    printf("bits 16\n\n");

    while (Instructions.Size)
    {
        byte FirstByte = GetByte(Instructions);
        u32 Instruction = GetInstructionType(FirstByte);
        if (Instruction == MOV)
        {
            DecodeMov(Instructions, FirstByte);
        }
        else
        {
            printf("ERROR: Failed to decode instruction!\n");
            break;
        }
    }

    FileFree(File);
}