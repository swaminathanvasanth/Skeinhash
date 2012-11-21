// this file gets replaced with our own driver when we grade your submission.
// So do what you want here but realize it won't persist when we grade it.

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "skein.h"

uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ __volatile__ (
      "xorl %%eax, %%eax\n"
      "cpuid\n"
      "rdtsc\n"
      : "=a" (lo), "=d" (hi)
      :
      : "%ebx", "%ecx" );
    return (uint64_t)hi << 32 | lo;
}

#define BBLOCKS 8192

void benchmark() {
	hctx ctx;
	unsigned char b[64];
	init(&ctx);
	uint64_t t0 = rdtsc();
	for(int i=0; i<BBLOCKS; i++) update(b, 64, &ctx);
	finalize(b, &ctx);
	uint64_t t1 = rdtsc();
	printf("%ld %.2f\n",t1-t0,(t1-t0)/(BBLOCKS*64.0));

}

#define CLOCK(x) (x >> 8 | (x ^ (x >> 7) ^ (x >> 6) ^ (x >> 2)) << 24)
void rnd(unsigned char *b, int len) {
	static uint32_t s = 0xFEEDFACE;
	for(int i=0; i<len; i++, s=CLOCK(s)) b[i] = (unsigned char)s;
}

int main() {

	hctx ctx;
	unsigned char h[64];

	unsigned char m0[] = {0xFF};
	unsigned char h0[] = {
0x42,0xAA,0x6B,0xD9,0xCA,0x92,0xE9,0x0E,0xA2,0x8D,0xF6,0xF6,0xF2,0xD0,0xD9,0xB8,
0x5A,0x2D,0x19,0x07,0xEE,0x4D,0xC1,0xB1,0x71,0xAC,0xE7,0xEB,0x11,0x59,0xBE,0x3B,
0xD1,0xBC,0x56,0x58,0x6D,0x92,0x49,0x2B,0x6E,0xFF,0x9B,0xE0,0x33,0x06,0x99,0x4C,
0x65,0xA3,0x32,0xC4,0xC2,0x41,0x60,0xF4,0x66,0x55,0x04,0x0E,0x55,0x8E,0x83,0x29 };
	init(&ctx);
	update(m0, 1, &ctx);
	finalize(h, &ctx);
	printf("%s\n", !memcmp(h, h0, 64) ? "PASS" : "FAIL");

	unsigned char m1[] = {
0xFF,0xFE,0xFD,0xFC,0xFB,0xFA,0xF9,0xF8,0xF7,0xF6,0xF5,0xF4,0xF3,0xF2,0xF1,0xF0,
0xEF,0xEE,0xED,0xEC,0xEB,0xEA,0xE9,0xE8,0xE7,0xE6,0xE5,0xE4,0xE3,0xE2,0xE1,0xE0,
0xDF,0xDE,0xDD,0xDC,0xDB,0xDA,0xD9,0xD8,0xD7,0xD6,0xD5,0xD4,0xD3,0xD2,0xD1,0xD0,
0xCF,0xCE,0xCD,0xCC,0xCB,0xCA,0xC9,0xC8,0xC7,0xC6,0xC5,0xC4,0xC3,0xC2,0xC1,0xC0 };
	unsigned char h1[] = {
0x04,0xF9,0x6C,0x6F,0x61,0xB3,0xE2,0x37,0xA4,0xFA,0x77,0x55,0xEE,0x4A,0xCF,0x34,
0x49,0x42,0x22,0x96,0x89,0x54,0xF4,0x95,0xAD,0x14,0x7A,0x1A,0x71,0x5F,0x7A,0x73,
0xEB,0xEC,0xFA,0x1E,0xF2,0x75,0xBE,0xD8,0x7D,0xC6,0x0B,0xD1,0xA0,0xBC,0x60,0x21,
0x06,0xFA,0x98,0xF8,0xE7,0x23,0x7B,0xD1,0xAC,0x09,0x58,0xE7,0x6D,0x30,0x66,0x78 };
	init(&ctx);
	update(m1, 2, &ctx);
	update(m1+2, 62, &ctx);
	finalize(h, &ctx);
	printf("%s\n", !memcmp(h, h1, 64) ? "PASS" : "FAIL");

	unsigned char m2[] = {
0xFF,0xFE,0xFD,0xFC,0xFB,0xFA,0xF9,0xF8,0xF7,0xF6,0xF5,0xF4,0xF3,0xF2,0xF1,0xF0,
0xEF,0xEE,0xED,0xEC,0xEB,0xEA,0xE9,0xE8,0xE7,0xE6,0xE5,0xE4,0xE3,0xE2,0xE1,0xE0,
0xDF,0xDE,0xDD,0xDC,0xDB,0xDA,0xD9,0xD8,0xD7,0xD6,0xD5,0xD4,0xD3,0xD2,0xD1,0xD0,
0xCF,0xCE,0xCD,0xCC,0xCB,0xCA,0xC9,0xC8,0xC7,0xC6,0xC5,0xC4,0xC3,0xC2,0xC1,0xC0,
0xBF,0xBE,0xBD,0xBC,0xBB,0xBA,0xB9,0xB8,0xB7,0xB6,0xB5,0xB4,0xB3,0xB2,0xB1,0xB0,
0xAF,0xAE,0xAD,0xAC,0xAB,0xAA,0xA9,0xA8,0xA7,0xA6,0xA5,0xA4,0xA3,0xA2,0xA1,0xA0,
0x9F,0x9E,0x9D,0x9C,0x9B,0x9A,0x99,0x98,0x97,0x96,0x95,0x94,0x93,0x92,0x91,0x90,
0x8F,0x8E,0x8D,0x8C,0x8B,0x8A,0x89,0x88,0x87,0x86,0x85,0x84,0x83,0x82,0x81,0x80 };
	unsigned char h2[] = {
0xB4,0x84,0xAE,0x9F,0xB7,0x3E,0x66,0x20,0xB1,0x0D,0x52,0xE4,0x92,0x60,0xAD,0x26,
0x62,0x0D,0xB2,0x88,0x3E,0xBA,0xFA,0x21,0x0D,0x70,0x19,0x22,0xAC,0xA8,0x53,0x68,
0x08,0x81,0x44,0xBD,0xF4,0xEF,0x3D,0x98,0x98,0xD4,0x7C,0x34,0xF1,0x30,0x03,0x1B,
0x0A,0x09,0x92,0xF0,0x9F,0x62,0xDD,0x78,0xB3,0x29,0x52,0x5A,0x77,0x7D,0xAF,0x7D };
	init(&ctx);
	update(m2, 67, &ctx);
	update(m2+67, 61, &ctx);
	finalize(h, &ctx);
	printf("%s\n", !memcmp(h, h2, 64) ? "PASS" : "FAIL");

	benchmark();

	return 0;
}

/*
kosh ~/assignment3_model 65 % make clean ; make
/bin/rm -f skein.o skein_driver *.o core *~
cc -std=c99 -I.  -O2 -fomit-frame-pointer -funroll-loops -c skein.c 
cc -std=c99 -I.  -O2 -fomit-frame-pointer -funroll-loops -o skein_driver skein_driver.c skein.o 
kosh ~/assignment3_model 66 % ./skein_driver
PASS
PASS
PASS
17033481 32.49
kosh ~/assignment3_model 67 % 
*/

