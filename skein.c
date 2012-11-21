#include <string.h> 
#include <stdio.h>
#include "skein.h"
// Rotation parameterss used by threefish
int rotation[8][4] = {
  { 46, 36, 19, 37 },
  { 33, 27, 14, 42 },
  { 17, 49, 36, 39 },
  { 44,  9, 54, 56 },
  { 39, 30, 34, 24 },
  { 13, 50, 10, 17 },
  { 25, 29, 39, 43 },
  {  8, 35, 56, 22 }
};
#define rotateleft(value, shift) ((value << shift) | (value >> (64 - shift)))
void init(hctx *ctx) 
{
// Declaration of Configuration String, Key, Tweakconfig
 unsigned char k[64];
 uint8_t tcfg[16];
 // Assigining the value for K,Tweak and Configuration String
 memset(k,0,64); // setting the value of key
 memset(ctx->c,0,64); // setting the value of configuration string
 memset(ctx->msg,0,64); // setting the value of configuration string
 ctx->msg_len = 0; // setting the value of configuration string
 ctx->flag = 0; // setting the value of configuration string
 ctx->bytes_encrypted = 0; 
 memset(tcfg,0,16); // setting value of tweak configuration
 tcfg[15] = 4; // Type value
 ctx->c[0] = 0x53; ctx->c[1] = 0x48; ctx->c[2] = 0x41; ctx->c[3] = 0x33; // assigning config string
 ctx->c[4] = 1;
 unsigned long i = 512; 
 //memset(&ctx->c[8],i,8);
 ctx->c[9] = 0x02;

 tcfg[15] |=  0b01000000; //Final value 
 tcfg[15] |=  0b10000000; //First
 tcfg[0] = 32; //position
 ubi(k,tcfg,ctx->c,ctx); //calling the UBI function
 memcpy(ctx->g0,ctx->output,64); // copying the output to g0 for utilizing in update function

 }
// ubi function 
void ubi(unsigned char *k,unsigned char *twk,unsigned char *message, hctx *ctx)
{
 init_threefish(k, twk, ctx); //call to the threefish init function
 threefish(ctx->output ,message, ctx); //call to the three fish function
 for(int i=0;i<64;i++)
 ctx->output[i] = ctx->output[i] ^ message[i]; 
}

// update a hash context with len bytes at address a.
void update(unsigned char *a, int len, hctx *ctx) 
{
 
 uint8_t tmsg[16];
 // Assigining the value for Tweak string and Configuration String
 memset(ctx->c,0,64); 
 memset(tmsg,0,16);
 tmsg[15] = 0x30;
 if (ctx->flag == 0)
 tmsg[15] |=  0b01000000; //First

 // keep storing the message bytes untill we have atleast a block of 64 bytes to enctrypt
 if(len + ctx->msg_len <= 64){
     memcpy(ctx->msg + ctx->msg_len, a, len);
       ctx->msg_len = ctx->msg_len +len;
 }
 else if ((ctx->msg_len + len)%64 == 0) {
    int loop_count = (ctx->msg_len + len)/64 -1;
    int bytes_encrypted = 64 - ctx->msg_len;
    int num=0;
     int offset = 0;
    for(num=0; num< loop_count; num++) {
     memcpy(ctx->msg + ctx->msg_len, a + num*64 + offset, bytes_encrypted);
     ctx->msg_len = 0;
     if (bytes_encrypted != 64) 
     offset = bytes_encrypted;
     bytes_encrypted = 64;
     ctx->bytes_encrypted = ctx->bytes_encrypted + 64;
     // copying the position into tweak
     unsigned long tmp = ctx->bytes_encrypted + 64;
     memcpy(tmsg, &tmp, 8);
     memcpy(ctx->c, ctx->msg, 64);
     ubi(ctx->g0,tmsg,ctx->c,ctx);
     ctx->flag = 1;
     memcpy(ctx->g1,ctx->output,64); //copying the output to be used for finalize function
    }

    memset(ctx->msg,0,64); // setting the value of configuration string
     memcpy(ctx->msg, a + num*64 + offset, 64);
     ctx->msg_len = 64;
 }
 else {
    int loop_count = (ctx->msg_len + len)/64;
    int bytes_encrypted = 64 - ctx->msg_len;
    int ttt= ctx->msg_len;
    int num;
     int offset = 0;
    for(num=0; num< loop_count; num++) {
     memcpy(ctx->msg + ctx->msg_len, a + num*64 + offset, bytes_encrypted);
     ctx->msg_len = 0;
     if (bytes_encrypted != 64) 
     offset = bytes_encrypted;
     bytes_encrypted = 64;
     ctx->bytes_encrypted = ctx->bytes_encrypted + 64; 
     // copying the position into tweak
     unsigned long tmp = ctx->bytes_encrypted + 64;
     memcpy(tmsg, &tmp, 8); 
     memcpy(ctx->c, ctx->msg, 64);
     ubi(ctx->g0,tmsg,ctx->c,ctx);
     ctx->flag = 1;
     memcpy(ctx->g1,ctx->output,64); //copying the output to be used for finalize function
    }
    memset(ctx->msg,0,64); // setting the value of configuration string
    memcpy(ctx->msg, a + num*64 + offset , len - (num*64 + offset));
    ctx->msg_len = len - (num*64 + offset);
 }
}

// this is the final call to threefish with final bit = 1
void final_threefish(hctx *ctx)
{
 uint8_t tmsg[16];
 // Assigining the value for Tweak string and Configuration String
 memset(ctx->c,0,64); 
 memset(tmsg,0,16);
 tmsg[15] = 0x30;
 tmsg[15] |=  0b10000000; //Final
 if (ctx->flag == 0)
   tmsg[15] |=  0b01000000; //First

 // copying the position into tweak
 unsigned long tmp = ctx->bytes_encrypted + ctx->msg_len;
 memcpy(tmsg, &tmp, 8); 
 memcpy(ctx->c,ctx->msg,ctx->msg_len); //copying the output to be used for finalize function
 ubi(ctx->g0,tmsg,ctx->c,ctx);
 memcpy(ctx->g1,ctx->output,64); //copying the output to be used for finalize function
}
// finalize a hash context and output the hash value in a.
void finalize(unsigned char *a, hctx *ctx) 
{
 uint8_t tout[16];
 // Assigining the value for K,Tweak and Configuration String
 memset(ctx->c,0,64);
 memset(tout,0,16);
 tout[15] = 0x3F;
 // Filling the tweak value
 
 tout[15] |=  0b01000000; //Final
 tout[15] |=  0b10000000; //First
 memset(&tout[0],8,1); //set position


 final_threefish(ctx);

 memset(ctx->c,0,64);
 ubi(ctx->g1,tout,ctx->c,ctx);
 memcpy(a,ctx->output,64);
}

// Three Fish Program
void init_threefish(unsigned char *k,unsigned char *t, hctx *ctx)
{//init function
  int d,s=0;
  unsigned char t2[8];
  unsigned long key[9],tweak[3],xor,knw,mix[8],kk[8],p[8],y[8],x[8],temp[8],temp_y[8];
  memcpy(key,k,64); // Copying the Key and Tweak 
  memcpy(tweak,t,16);
  /* Calculation of t2 (t2=t0 xor t1) */
  tweak[2]= tweak[0] ^ tweak[1];
  for(int keyposition=0;keyposition<8;keyposition++)
  {
    if(keyposition==0)
      xor=key[keyposition];
    else
      xor=key[keyposition]^xor;
  }
  knw=0x5555555555555555LL^xor;

  key[8] = knw;
  // Loop for 19 Sub Keys.Calculation of Keys 
  for(s=0;s<19;s++)
  {
    for(int i=0;i<8;i++)
    {
      if(i ==0 || i ==1 || i ==2 || i ==3 || i ==4)
        ctx->keyschedule[s][i] = key[(s+i)%9];
      else if(i==5)
        ctx->keyschedule[s][i] = ((key[(s+i)%9])+tweak[s%3]);
      else if(i==6)
        ctx->keyschedule[s][i] = ((key[(s+i)%9])+tweak[(s+1)%3] );
      else
        ctx->keyschedule[s][i] = ((key[(s+i)%9])+s);
    }
  }
}
// using context ctx, encrypt len bytes of plaintext p and store the result in b.
void threefish(unsigned char *b, unsigned char *p,hctx *ctx) {
  unsigned long key[9],xor,mix[8],kk[8],y[8],x[8],temp[8],temp_y[8];
  int d,s=0,flag=0;
  //MIX Function Calculation
  memcpy(x,p,64);
  for(int j=0;j<8;j++)
  {
     x[j]+=ctx->keyschedule[0][j];
  }

  //Calculation of Subkey 
  for(d=0;d<72;d++)
  {
    x[0]=(x[0]+x[1]);
    temp_y[0]=rotateleft(x[1],rotation[d%8][0]);
    x[1]=temp_y[0] ^ x[0];

    x[2]=(x[2]+x[3]);
    temp_y[2]=rotateleft(x[3],rotation[d%8][1]);
    x[3]=temp_y[2] ^ x[2];

    x[4]=(x[4]+x[5]);
    temp_y[4]=rotateleft(x[5],rotation[d%8][2]);
    x[5]=temp_y[4] ^ x[4];

    x[6]=(x[6]+x[7]);
    temp_y[6]=rotateleft(x[7],rotation[d%8][3]);
    x[7]=temp_y[6] ^ x[6];

   //Permutation. Copying output of each round to a temp variable for permutation.
      temp[0]=x[0];
      temp[1]=x[1];
      temp[2]=x[2];
      temp[3]=x[3];
      temp[4]=x[4];
      temp[5]=x[5];
      temp[6]=x[6];
      temp[7]=x[7];
//Permutation is done here
    uint64_t tmp0=x[0];
    uint64_t tmp3=x[3];
    x[0]=x[2];
    x[1]=x[1];
    x[2]=x[4];
    x[3]=x[7];
    x[4]=x[6];
    x[5]=x[5];
    x[6]=tmp0;
    x[7]=tmp3;

    if((d+1)%4==0) //Checking the round number to add the subkey every 4th round
    {
      for(int i=0;i<8;i++)
        x[i]=x[i]+ctx->keyschedule[(d+1)/4][i]; //add new sub key to the obtained solution 
    }
  }
  memcpy(b,x,64);
}
 



