/* +++Date last modified: 05-Jul-1997 */

/*
**  BITCNTS.C - Test program for bit counting functions
**
**  public domain by Bob Stout & Auke Reitsma
*/

// #include <stdio.h>
#include <stdlib.h>
#include "conio.h"
#include <limits.h>
// #include <time.h>
#include <float.h>
#include "bitops.h"
#include "eapp.h"
#include "ocall.h"
#include <stdint.h>

#define FUNCS  7

#define CLOCKS_PER_SEC 1000000

static int CDECL bit_shifter(long int x);

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

int EAPP_ENTRY main(int argc, char *argv[])
{
  unsigned long * args;
	EAPP_RESERVE_REG;
  memcpy(&arguement, (void *)DEFAULT_UNTRUSTED_PTR, sizeof(arguement));

  uint64_t start, stop;
  uint64_t ct, cmin = DBL_MAX, cmax = 0;
  int i, cminix, cmaxix;
  long j, n, seed;
  int iterations;
  static int (* CDECL pBitCntFunc[FUNCS])(long) = {
    bit_count,
    bitcount,
    ntbl_bitcnt,
    ntbl_bitcount,
    /*            btbl_bitcnt, DOESNT WORK*/
    BW_btbl_bitcount,
    AR_btbl_bitcount,
    bit_shifter
  };
  static char *text[FUNCS] = {
    "Optimized 1 bit/loop counter",
    "Ratko's mystery algorithm",
    "Recursive bit count by nybbles",
    "Non-recursive bit count by nybbles",
    /*            "Recursive bit count by bytes",*/
    "Non-recursive bit count by bytes (BW)",
    "Non-recursive bit count by bytes (AR)",
    "Shift and count bits"
  };
  if (arguement.argc<2) {
    eapp_print("Usage: bitcnts <iterations>\n");
    EAPP_RETURN(-1);
    // exit(-1);
	}
  iterations=atoi(arguement.argv[1]);
  
  eapp_print("Bit counter algorithm benchmark\n");
  
  for (i = 0; i < FUNCS; i++) {
    // start = clock();
    asm volatile("rdtime %0" : "=r"(start));
    // eapp_print("t\n");
    
    for (j = n = 0, seed = rand(); j < iterations; j++, seed += 13)
	 n += pBitCntFunc[i](seed);
    
    
    // stop = clock();
    asm volatile("rdtime %0" : "=r"(stop));
    // eapp_print("stop time\n");
    // ct = (stop - start) / (double)CLOCKS_PER_SEC;
    ct = (stop - start) / CLOCKS_PER_SEC;
    // eapp_print("clock\n");
    if (ct < cmin) {
	 cmin = ct;
	 cminix = i;
    }
    if (ct > cmax) {
	 cmax = ct;
	 cmaxix = i;
    }
    
    // eapp_print("%-38s> Time: %ld sec.; Bits: %ld\n", text[i], ct, n);
  }
  eapp_print("\nBest  > %s\n", text[cminix]);
  eapp_print("Worst > %s\n", text[cmaxix]);
	EAPP_RETURN(0);
  // return 0;
}

static int CDECL bit_shifter(long int x)
{
  int i, n;
  
  for (i = n = 0; x && (i < (sizeof(long) * CHAR_BIT)); ++i, x >>= 1)
    n += (int)(x & 1L);
  return n;
}
