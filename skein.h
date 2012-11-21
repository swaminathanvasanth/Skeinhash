#include <stdint.h>

/* Skein hash context. you might store:
1) Up to a block of buffered message.
2) The chaining value.
3) The tweak.
4) Anything else you might need.
*/
typedef struct {
unsigned long Nb,h[8];
unsigned char output[64];
unsigned char c[64];
unsigned char g0[64];

unsigned char g1[64];
unsigned char msg[64];
unsigned long msg_len;
unsigned char flag;
unsigned long bytes_encrypted;
unsigned long keyschedule[19][8];
} hctx;

void init(hctx *);
void update(unsigned char *, int, hctx *);
void finalize(unsigned char *, hctx *);
void ubi(unsigned char *, unsigned char *, unsigned char *, hctx *);

// Three Fish Program
void init_threefish(unsigned char *, unsigned char *, hctx *);
void threefish(unsigned char *, unsigned char *,hctx *); 
