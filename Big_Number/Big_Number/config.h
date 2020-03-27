
#ifndef _CONFIG_H_
#define _CONFIG_H_


#define _WORD_BITSZ_32_


/* system independent types */

typedef unsigned int UWORD;
typedef UWORD* P_UWORD;
typedef signed int SWORD;
typedef SWORD* P_SWORD;
typedef unsigned int UINT;
typedef UINT* P_UINT;
typedef signed int SINT;
typedef SINT* P_SINT;

typedef unsigned long ULONG;
typedef ULONG *P_ULONG;
typedef signed long SLONG;
typedef SLONG* P_SLONG;
typedef unsigned short USHORT;
typedef USHORT* P_USHORT;
typedef signed short SSHORT;
typedef SSHORT* P_SSHORT;
typedef unsigned char UCHAR;
typedef UCHAR* P_UCHAR;
typedef char SCHAR;
typedef SCHAR* P_SCHAR;
typedef SINT TORF;

#undef TRUE
#define TRUE	(TORF)1
#undef FALSE
#define FALSE (TORF)0

#define RET_VAL    UWORD /* return values */

/* system dependent types */

typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned int   UINT32;
typedef unsigned __int64 UINT64;
#if defined(KLIB_WIN32)
#define	UINT64				unsigned __int64
#elif defined(_LINUX_)
#define	UINT64				unsigned long long /* for gcc */
#endif

#define MASK_WW  	0xffffffff /*Whole Word*/
#define MASK_LHW	0xffff0000 /*Left Half Word*/
#define MASK_RHW	0x0000ffff /*Right Half Word*/
#define LMSB_ONE	0x80000000 /*Left Most Significant Bit One*/
#define RMSB_ONE	0x00008000 /*Right Most Significant Bit One*/
#define LLSB_ONE	0x00010000 /*Left Least Significant Bit One*/
#define RLSB_ONE	0x00000001 /*Right Least Significant Bit One*/
#define BITSZ_WW	32 /*Bit size of Whole Word*/
#define BITSZ_WW_1	31 /*Bit size of Whole Word minus 1*/
#define BITSZ_HW	16 /*Bit size of Half Word*/
#define BITSZ_HW_1	15 /*Bit size of Half Word minus 1*/
#define BYTESZ_WW   4  /* Byte size of Whole Word*/
#define BYTESZ_WW_1 3  /* Byte size of Whole Word minus 1*/


#define MEM_MARGIN_1 1
#define MEM_MARGIN_8 8

#define BINT_WORD_LEN (300 + MEM_MARGIN_1)
#define HASH_BYTE_LEN (20 + MEM_MARGIN_1)


#define MAX_BINT_LEN (300 + MEM_MARGIN_1)
#define MAX_PF_LEN (300 + MEM_MARGIN_1)
#define MAX_BFPB_LEN (50 + MEM_MARGIN_1)
#define MAX_GF_LEN (MAX_PF_LEN>MAX_BFPB_LEN?MAX_PF_LEN:MAX_BFPB_LEN)
#define MAX_HASH_BYTES (20 + MEM_MARGIN_1)
typedef SINT BOOL;


#define _NO_EXT_CODE_

#if defined(_NO_SHA1_)/*{*/
#  define _NO_RAND_X9_62_
#  define _NO_RAND_X9_63_KDF_
#  define _NO_RAND_X9_31_
#endif/*}_NO_SHA1_*/

/*
#define _CY_DEBUG_
*/

//#define _MATHEMATICA_EXPR_
#endif /*_CONFIG_H_*/
