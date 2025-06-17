// #include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
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

int EAPP_ENTRY main(int argc, char *argv[]) {
	// uint64_t time1;
    // asm volatile("rdtime %0" : "=r"(time1));

	unsigned long * args;
	EAPP_RESERVE_REG;
    memcpy(&arguement, (void *)DEFAULT_UNTRUSTED_PTR, sizeof(arguement));

	unsigned MAXSIZE;
	unsigned MAXWAVES;
	unsigned i,j;
	float *RealIn;
	float *ImagIn;
	float *RealOut;
	float *ImagOut;
	float *coeff;
	float *amp;
	int invfft=0;

	if (arguement.argc<3)
	{
		eapp_print("Usage: fft <waves> <length> -i\n");
		eapp_print("-i performs an inverse fft\n");
		eapp_print("make <waves> random sinusoids");
		eapp_print("<length> is the number of samples\n");
		EAPP_RETURN(-1);
		// exit(-1);
	}
	else if (arguement.argc==4)
		invfft = !strncmp(arguement.argv[3],"-i",2);
	MAXSIZE=atoi(arguement.argv[2]);
	MAXWAVES=atoi(arguement.argv[1]);
		
 srand(1);

eapp_print("m1\n");
 RealIn=(float*)malloc(sizeof(float)*MAXSIZE);
eapp_print("m2\n");
 ImagIn=(float*)malloc(sizeof(float)*MAXSIZE);
eapp_print("m3\n");
 RealOut=(float*)malloc(sizeof(float)*MAXSIZE);
 ImagOut=(float*)malloc(sizeof(float)*MAXSIZE);
 coeff=(float*)malloc(sizeof(float)*MAXWAVES);
 amp=(float*)malloc(sizeof(float)*MAXWAVES);
eapp_print("m end\n");

 /* Makes MAXWAVES waves of random amplitude and period */
	for(i=0;i<MAXWAVES;i++) 
	{
		coeff[i] = rand()%1000;
		amp[i] = rand()%1000;
	}
 for(i=0;i<MAXSIZE;i++) 
 {
   /*   RealIn[i]=rand();*/
	 RealIn[i]=0;
	 for(j=0;j<MAXWAVES;j++) 
	 {
		 /* randomly select sin or cos */
		 if (rand()%2)
		 {
		 		RealIn[i]+=coeff[j]*cos(amp[j]*i);
			}
		 else
		 {
		 	RealIn[i]+=coeff[j]*sin(amp[j]*i);
		 }
  	 ImagIn[i]=0;
	 }
 }

 /* regular*/
 fft_float (MAXSIZE,invfft,RealIn,ImagIn,RealOut,ImagOut);
 
//  eapp_print("RealOut:\n");
//  for (i=0;i<MAXSIZE;i++)
//    eapp_print("%f \t", RealOut[i]);
//  eapp_print("\n");

// eapp_print("ImagOut:\n");
//  for (i=0;i<MAXSIZE;i++)
//    eapp_print("%f \t", ImagOut[i]);
//    eapp_print("\n");

 free(RealIn);
 free(ImagIn);
 free(RealOut);
 free(ImagOut);
 free(coeff);
 free(amp);

//  uint64_t time2;
//  asm volatile("rdtime %0" : "=r"(time2));
//  eapp_print("speed tick: %ld\n", time2 - time1);
EAPP_RETURN(0);
//  exit(0);


}
