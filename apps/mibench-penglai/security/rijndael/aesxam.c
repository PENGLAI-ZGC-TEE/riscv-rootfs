
 /*
   -----------------------------------------------------------------------
   Copyright (c) 2001 Dr Brian Gladman <brg@gladman.uk.net>, Worcester, UK
   
   TERMS

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   This software is provided 'as is' with no guarantees of correctness or
   fitness for purpose.
   -----------------------------------------------------------------------
 */

/* Example of the use of the AES (Rijndael) algorithm for file  */
/* encryption.  Note that this is an example application, it is */
/* not intended for real operational use.  The Command line is: */
/*                                                              */
/* aesxam input_file_name output_file_name [D|E] hexadecimalkey */
/*                                                              */
/* where E gives encryption and D decryption of the input file  */
/* into the output file using the given hexadecimal key string  */
/* The later is a hexadecimal sequence of 32, 48 or 64 digits   */
/* Examples to encrypt or decrypt aes.c into aes.enc are:       */
/*                                                              */
/* aesxam file.c file.enc E 0123456789abcdeffedcba9876543210    */
/*                                                              */
/* aesxam file.enc file2.c D 0123456789abcdeffedcba9876543210   */
/*                                                              */
/* which should return a file 'file2.c' identical to 'file.c'   */

// #include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <ctype.h>

#include "aes.h"

#include "eapp.h"
#include "ocall.h"

/* A Pseudo Random Number Generator (PRNG) used for the     */
/* Initialisation Vector. The PRNG is George Marsaglia's    */
/* Multiply-With-Carry (MWC) PRNG that concatenates two     */
/* 16-bit MWC generators:                                   */
/*     x(n)=36969 * x(n-1) + carry mod 2^16                 */ 
/*     y(n)=18000 * y(n-1) + carry mod 2^16                 */
/* to produce a combined PRNG with a period of about 2^60.  */  
/* The Pentium cycle counter is used to initialise it. This */
/* is crude but the IV does not need to be secret.          */
 
/* void cycles(unsigned long *rtn)     */
/* {                           // read the Pentium Time Stamp Counter */
/*     __asm */
/*     { */
/*     _emit   0x0f            // complete pending operations */
/*     _emit   0xa2 */
/*     _emit   0x0f            // read time stamp counter */
/*     _emit   0x31 */
/*     mov     ebx,rtn */
/*     mov     [ebx],eax */
/*     mov     [ebx+4],edx */
/*     _emit   0x0f            // complete pending operations */
/*     _emit   0xa2 */
/*     } */
/* } */

#define RAND(a,b) (((a = 36969 * (a & 65535) + (a >> 16)) << 16) + (b = 18000 * (b & 65535) + (b >> 16))  )
#define BLOCK_SIZE 16

struct arguement
{
  uint64_t file1_len;
  uint64_t file2_len;
  uint64_t argc;
  char argv[10][256];
  /* file
  void* file1; offset: 0x1000
  void* file2; offset: 0x400000
  */
} arguement;

char* fin = NULL;
char* fout = NULL;

typedef char FILE;

void fillrand(char *buf, int len)
{   static unsigned long a[2], mt = 1, count = 4;
    static char          r[4];
    int                  i;

    if(mt) { 
	 mt = 0; 
	 /*cycles(a);*/
      a[0]=0xeaf3;
	 a[1]=0x35fe;
    }

    for(i = 0; i < len; ++i)
    {
        if(count == 4)
        {
            *(unsigned long*)r = RAND(a[0], a[1]);
            count = 0;
        }

        buf[i] = r[count++];
    }
}    

int encfile(FILE *fin, FILE *fout, aes *ctx, char *fn) {
    eapp_print("start enc\n");
    size_t file_size = arguement.file1_len;
    char inbuf[BLOCK_SIZE], outbuf[BLOCK_SIZE];
    unsigned long i = 0, l = 0;

    fillrand(outbuf, BLOCK_SIZE);
    memcpy(fout, outbuf, BLOCK_SIZE);

    fillrand(inbuf, 1);
    l = 15;
    inbuf[0] = ((char)file_size & 15) | (inbuf[0] & ~15);

    char *in_ptr = fin;
    char *out_ptr = fout + BLOCK_SIZE;

    // eapp_print("start loop\n");
    while (in_ptr < fin + file_size) {
        size_t remaining_bytes = fin + file_size - in_ptr;
        if (remaining_bytes < BLOCK_SIZE) {
            memcpy(inbuf + (BLOCK_SIZE - l), in_ptr, remaining_bytes);
            in_ptr += BLOCK_SIZE - l + remaining_bytes;
            l = remaining_bytes;
            break;
        }
        memcpy(inbuf + (BLOCK_SIZE - l), in_ptr, l);
        in_ptr += BLOCK_SIZE;

        if (in_ptr != fin) {
            for (i = 0; i < BLOCK_SIZE; ++i) {
                inbuf[i] ^= outbuf[i];
            }
        }

        encrypt(inbuf, outbuf, ctx);

        memcpy(out_ptr, outbuf, BLOCK_SIZE);
        out_ptr += BLOCK_SIZE;

        l = 16;
    }

    if (l == 15) {
        ++i;
    }

    if (i) {
        while (i < BLOCK_SIZE) {
            inbuf[i++] = 0;
        }

        for (i = 0; i < BLOCK_SIZE; ++i) {
            inbuf[i] ^= outbuf[i];
        }

        encrypt(inbuf, outbuf, ctx);

        memcpy(out_ptr, outbuf, BLOCK_SIZE);
    }

    return 0;
}

int decfile(char *fin, char *fout, aes *ctx, char *ifn, char *ofn)
{
    size_t file_size = arguement.file1_len;
    char inbuf1[16], inbuf2[16], outbuf[16], *bp1, *bp2, *tp;
    int i, l, flen;
    size_t offset = 0; // 用于跟踪当前处理的文件偏移

    if (file_size < 16)  // 文件大小不够16字节，无法读取初始化向量
    {
        eapp_print("Input file too small: %s\n", ifn);
        return 9;
    }

    // 读取初始化向量
    memcpy(inbuf1, fin, 16);
    offset += 16;

    // 读取第一个加密块
    if (offset + 16 > file_size)  // 剩余数据不足16字节
    {
        eapp_print("The input file is corrupt\n");
        return -10;
    }

    memcpy(inbuf2, fin + offset, 16);
    offset += 16;

    decrypt(inbuf2, outbuf, ctx); // 解密第一个块

    for (i = 0; i < 16; ++i)     // 与初始化向量异或
        outbuf[i] ^= inbuf1[i];

    flen = outbuf[0] & 15; // 恢复最后一个块的有效字节数
    l = 15;                // 设置当前块的有效字节数为15
    bp1 = inbuf1;          // 设置缓冲区指针
    bp2 = inbuf2;

    while (offset < file_size)
    {
        if (offset + 16 > file_size)  // 剩余数据不足16字节
            break;

        memcpy(bp1, fin + offset, 16); // 读取下一个加密块
        offset += 16;

        // 写入上一个解密块的内容
        if (memcpy(fout + offset - 32, outbuf + 16 - l, l) == NULL)
        {
            eapp_print("Error writing to output file: %s\n", ofn);
            return -11;
        }

        decrypt(bp1, outbuf, ctx); // 解密当前块

        for (i = 0; i < 16; ++i)  // 与上一个加密块异或
            outbuf[i] ^= bp2[i];

        l = i; tp = bp1, bp1 = bp2, bp2 = tp; // 切换缓冲区
    }

    // 计算最后一部分的字节数
    l = (l == 15 ? 1 : 0);
    flen += 1 - l;

    if (flen)
        if (memcpy(fout + offset - l, outbuf + l, flen) == NULL)
        {
            eapp_print("Error writing to output file: %s\n", ofn);
            return -12;
        }

    return 0;
}


int entry(int argc)
{
    // char** argv = arguement.argv;
    char    *cp, ch, key[32];
    int     i=0, by=0, key_len=0, err = 0;
    aes     ctx[1];

    if(argc != 5 || (toupper(arguement.argv[3][0]) != 'D' && toupper(arguement.argv[3][0]) != 'E'))
    {
        eapp_print("usage: rijndael in_filename out_filename [d/e] key_in_hex\n"); 
        err = -1; goto exit;
    }

    cp = arguement.argv[4];   /* this is a pointer to the hexadecimal key digits  */
    i = 0;          /* this is a count for the input digits processed   */
    
    while(i < 64 && *cp)    /* the maximum key length is 32 bytes and   */
    {                       /* hence at most 64 hexadecimal digits      */
        ch = toupper(*cp++);            /* process a hexadecimal digit  */
        if(ch >= '0' && ch <= '9')
            by = (by << 4) + ch - '0';
        else if(ch >= 'A' && ch <= 'F')
            by = (by << 4) + ch - 'A' + 10;
        else                            /* error if not hexadecimal     */
        {
            eapp_print("key must be in hexadecimal notation\n"); 
            err = -2; goto exit;
        }
        
        /* store a key byte for each pair of hexadecimal digits         */
        if(i++ & 1) 
            key[i / 2 - 1] = by & 0xff; 
    }
    eapp_print("key\n");

    if(*cp)
    {
        eapp_print("The key value is too long\n"); 
        err = -3; goto exit;
    }
    else if(i < 32 || (i & 15))
    {
        eapp_print("The key length must be 32, 48 or 64 hexadecimal digits\n");
        err = -4; goto exit;
    }

    key_len = i / 2;

    // if(!(fin = fopen(argv[1], "rb")))   /* try to open the input file */
    // {
    //     eapp_print("The input file: %s could not be opened\n", argv[1]); 
    //     err = -5; goto exit;
    // }

    // if(!(fout = fopen(argv[2], "wb")))  /* try to open the output file */
    // {
    //     eapp_print("The output file: %s could not be opened\n", argv[1]); 
    //     err = -6; goto exit;
    // }

    if(toupper(arguement.argv[3][0]) == 'E')
    {                           /* encryption in Cipher Block Chaining mode */
        set_key(key, key_len, enc, ctx);

        err = encfile(fin, fout, ctx, arguement.argv[1]);
    }
    else
    {                           /* decryption in Cipher Block Chaining mode */
        set_key(key, key_len, dec, ctx);
    
        err = decfile(fin, fout, ctx, arguement.argv[1], arguement.argv[2]);
    }
exit:   
    // if(fout) 
    //     fclose(fout);
    // if(fin)
    //     fclose(fin);

    return err;
}

int EAPP_ENTRY main(int argc, char **argv)
{
    uint64_t time1;
    asm volatile("rdtime %0" : "=r"(time1));

	unsigned long * args;
	EAPP_RESERVE_REG;

    memcpy(&arguement, (void *)DEFAULT_UNTRUSTED_PTR, sizeof(arguement));
    fin = (char *)(DEFAULT_UNTRUSTED_PTR + 0x1000);
    fout = (char *)(DEFAULT_UNTRUSTED_PTR + 0x200000);
    eapp_print("argc: %d:\n", arguement.argc);
    for (int i = 0; i < arguement.argc; i ++) {
        eapp_print("%s\n", arguement.argv[i]);
    }

    int ret = entry(arguement.argc);

    uint64_t time2;
    asm volatile("rdtime %0" : "=r"(time2));
	eapp_print("speed tick: %ld\n", time2 - time1);

    EAPP_RETURN(ret);
}