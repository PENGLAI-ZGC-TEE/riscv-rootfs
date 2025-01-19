// #include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "blowfish.h"
#include "eapp.h"
#include "ocall.h"

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

char* fin;
char* fout;

typedef char FILE;

int EAPP_ENTRY main(int argc, char *argv[])
{
	// arguement init
	unsigned long * args;
	EAPP_RESERVE_REG;

    memcpy(&arguement, (void *)DEFAULT_UNTRUSTED_PTR, sizeof(arguement));
    fin = (char *)(DEFAULT_UNTRUSTED_PTR + 0x1000);
    fout = (char *)(DEFAULT_UNTRUSTED_PTR + 0x200000);
    eapp_print("argc: %d:\n", arguement.argc);
    for (int i = 0; i < arguement.argc; i ++) {
        eapp_print("%s\n", arguement.argv[i]);
    }

	BF_KEY key;
	unsigned char ukey[8];
	unsigned char indata[40],outdata[40],ivec[8];
	int num;
	int by=0,i=0;
	int encordec=-1;
	char *cp,ch;
	FILE *fp,*fp2;

if (arguement.argc<3)
{
	printf("Usage: blowfish {e|d} <intput> <output> key\n");
	exit(-1);
}

if (*arguement.argv[1]=='e' || *arguement.argv[1]=='E')
	encordec = 1;
else if (*arguement.argv[1]=='d' || *arguement.argv[1]=='D')
	encordec = 0;
else
{
	printf("Usage: blowfish {e|d} <intput> <output> key\n");
	exit(-1);
}
					

/* Read the key */
cp = arguement.argv[4];
while(i < 64 && *cp)    /* the maximum key length is 32 bytes and   */
{                       /* hence at most 64 hexadecimal digits      */
	ch = toupper(*cp++);            /* process a hexadecimal digit  */
	if(ch >= '0' && ch <= '9')
		by = (by << 4) + ch - '0';
	else if(ch >= 'A' && ch <= 'F')
		by = (by << 4) + ch - 'A' + 10;
	else                            /* error if not hexadecimal     */
	{
		printf("key must be in hexadecimal notation\n");
		exit(-1);
	}

	/* store a key byte for each pair of hexadecimal digits         */
	if(i++ & 1)
		ukey[i / 2 - 1] = by & 0xff;
}

BF_set_key(&key,8,ukey);

if(*cp)
{
	printf("Bad key value.\n");
	exit(-1);
}

/* open the input and output files */
if ((fp = fopen(arguement.argv[2],"r"))==0)
{
	printf("Usage: blowfish {e|d} <intput> <output> key\n");
	exit(-1);
};
if ((fp2 = fopen(arguement.argv[3],"w"))==0)
{
	printf("Usage: blowfish {e|d} <intput> <output> key\n");
	exit(-1);
};

i=0;
while(!feof(fp))
{
	int j;
	while(!feof(fp) && i<40)
		indata[i++]=getc(fp);

	BF_cfb64_encrypt(indata,outdata,i,&key,ivec,&num,encordec);

	for(j=0;j<i;j++)
	{
		/*printf("%c",outdata[j]);*/
		fputc(outdata[j],fp2);
	}
	i=0;
}

close(fp);
close(fp2);

EAPP_RETURN(0);
}



