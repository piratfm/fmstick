//================================================== 
// Texas Instruments Strictly Private 
// Copyright 1999, Texas Instruments Inc. 
//================================================== 
#ifndef _TYPES_H
#define	_TYPES_H

#ifndef BYTE_WORD_TYPEDEF
#define BYTE_WORD_TYPEDEF 
typedef unsigned char byte;
typedef unsigned int  word;
typedef unsigned char BYTE;
typedef unsigned int WORD;
#endif

#ifndef HIGH_LOW_TYPEDEF
#define LOW(x) (*((byte*)&(##x)+1))              
#define HIGH(x) (*((byte*)&(##x)+0))
#endif

#ifndef NULL
#define NULL    ((void *)0)
#endif

//
// Get the most significant byte of a word.
//
#define MSB(s)                  ((byte)((0xFF00 & ((word)(s))) >> 8))

//
// Get the least significant byte of a word.
//
#define LSB(s)                  ((byte)(0x00FF & ((word)(s))))

//
// Makes word from two bytes.
//
#define MAKE_WORD(msb, lsb)     ((((word)(msb)) << 8) | ((word)(lsb)))

//
// Reverse the endian of a word
//
#define REVERSE_ENDIAN(s)       MAKE_WORD(LSB(s), MSB(s))

//
// Make a little endian word from a big endian word
//
#define MAKE_LITTLE_ENDIAN(s)   REVERSE_ENDIAN(s)

//
// Make a little endian word from a big endian word
//
#define MAKE_BIG_ENDIAN(s)   REVERSE_ENDIAN(s)



#ifndef TRUE_FALSE_TYPEDEF
#define TRUE_FALSE_TYPEDEF
#define TRUE 1
#define FALSE 0
#endif

// For handle the bit operations in a byte
#define SET_BIT(val, bit)   (val |= bit)
#define CLEAR_BIT(val, bit) (val &= ~bit)
#define GET_BIT(val, bit) (val & bit) 

#define VAR_NOT_USED(s)     ((s) = (s))
#define BIT(x)  (1 << x)
#endif

