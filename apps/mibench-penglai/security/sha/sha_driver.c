/* NIST Secure Hash Algorithm */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "sha.h"
#include "eapp.h"
#include "ocall.h"

int EAPP_ENTRY main(int argc, char **argv)
{
	uint64_t time1;
    asm volatile("rdtime %0" : "=r"(time1));

	unsigned long * args;
	EAPP_RESERVE_REG;
    // FILE *fin;
    SHA_INFO sha_info;

    // if (argc < 2) {
	// fin = stdin;
	// sha_stream(&sha_info, fin);
	// sha_print(&sha_info);
    // } else {
	// while (--argc) {
	//     fin = fopen(*(++argv), "rb");
	//     if (fin == NULL) {
	// 	printf("error opening %s for reading\n", *argv);
	//     } else {
	// 	sha_stream(&sha_info, fin);
	// 	sha_print(&sha_info);
	// 	fclose(fin);
	//     }
	// }
    // }
	char *fin = ((char *)DEFAULT_UNTRUSTED_PTR)+0x1000;
	int len = *(uint64_t *)DEFAULT_UNTRUSTED_PTR;
	eapp_print("file size: %d\n", len);
	sha_stream(&sha_info, fin, len);
	sha_print(&sha_info);

	uint64_t time2;
    asm volatile("rdtime %0" : "=r"(time2));
	eapp_print("speed tick: %d\n", time2 - time1);

	EAPP_RETURN(0);
    // return(0);
}
