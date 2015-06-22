/*

Copyright (c) 2015, Elliptic Technologies Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <sk_swld.h>

int32_t my_rc4_prng_ready(rc4_prng_state *prng)
{
   uint8_t key[256], tmp, *s;
   int32_t keylen, x, y, j;

   SYSV_ASSERT(prng != NULL);

   /* extract the key */
   s = prng->buf;
   SYSV_MEMCPY(key, s, 256);
   keylen = prng->x;

   /* make LTC_RC4 perm and shuffle */
   for (x = 0; x < 256; x++)
   {
     s[x] = x;
   }

   for (j = x = y = 0; x < 256; x++)
   {
     y = (y + prng->buf[x] + key[j++]) & 255;

     if (j == keylen)
     {
        j = 0;
     }

     tmp  = s[x];
     s[x] = s[y];
     s[y] = tmp;
   }

   prng->x = 0;
   prng->y = 0;

   return MYRC4ERR_SUCCESS;
}

int32_t my_rc4_prng_init(rc4_prng_state *prng)
{
    SYSV_ASSERT(prng != NULL);

    /* set keysize to zero */
    prng->x = 0;

    return MYRC4ERR_SUCCESS;
}

int32_t my_rc4_prng_seed(rc4_prng_state *prng, const uint8_t *in, int32_t inlen)
{
    SYSV_ASSERT(in  != NULL);
    SYSV_ASSERT(prng != NULL);

    /* trim as required */
    if ((prng->x + inlen) > 256)
    {
       if (prng->x == 256)
       {
          /* I can't possibly accept another byte, ok maybe a mint wafer... */
          return MYRC4ERR_SUCCESS;
       }
       else
       {
          /* only accept part of it */
          inlen = 256 - prng->x;
       }
    }

    while (inlen--)
    {
       prng->buf[prng->x++] = (*in);
       in++;
    }

    //return my_rc4_prng_ready(prng);
    return MYRC4ERR_SUCCESS;

}

uint32_t my_rc4_prng_read(rc4_prng_state *prng, uint8_t *out, int32_t outlen)
{
   uint8_t x, y, *s, tmp;
   uint32_t n;

   SYSV_ASSERT(out != NULL);
   SYSV_ASSERT(prng != NULL);

   //SYSV_MEMSET(out, 0, outlen);

   n = outlen;
   x = prng->x;
   y = prng->y;
   s = prng->buf;
   while (outlen--)
   {
      x = (x + 1) & 255;
      y = (y + s[x]) & 255;
      tmp = s[x]; s[x] = s[y]; s[y] = tmp;
      tmp = (s[x] + s[y]) & 255;
      *out++ ^= s[tmp];
   }

   prng->x = x;
   prng->y = y;

   return n;
}

int32_t my_rc4_prng_end(rc4_prng_state *prng)
{
   SYSV_ASSERT(prng != NULL);

   return MYRC4ERR_SUCCESS;
}

#ifdef RC4_TEST

/**
  PRNG self-test
  @return CRYPT_OK if successful, CRYPT_NOP if self-testing has been disabled
*/
int32_t my_rc4_prng_test(void)
{
   static const struct
   {
      uint8_t key[8], pt[8], ct[8];
   } tests[] = {
                  {
                     { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef },
                     { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef },
                     { 0x75, 0xb7, 0x87, 0x80, 0x99, 0xe0, 0xc5, 0x96 }
                  }
               };

   rc4_prng_state prng;
   uint8_t dst[8];
   int32_t err, x;

   for (x = 0; x < (int)(sizeof(tests)/sizeof(tests[0])); x++)
   {
       printf("1\n");
       if ((err = my_rc4_prng_init(&prng)) != MYRC4ERR_SUCCESS)
       {
          return err;
       }
       printf("2\n");
       if ((err = my_rc4_prng_seed(&prng, tests[x].key, 8)) != MYRC4ERR_SUCCESS)
       {
          return err;
       }
       printf("3\n");
       if ((err = my_rc4_prng_ready(&prng)) != MYRC4ERR_SUCCESS)
       {
          return err;
       }

       printf("4\n");
       SYSV_MEMCPY(dst, tests[x].pt, 8);

       printf("5\n");
       if (my_rc4_prng_read(&prng, dst, 8) != 8)
       {
          return -1;
       }
       printf("6\n");
       my_rc4_prng_end(&prng);

       printf("7\n");
       if (SYSV_MEMCMP(dst, tests[x].ct, 8))
       {
          int32_t i;

          for(i = 0; i < 8; i++)
          {
             printf("0x%.2x ", dst[i]);
          }
          printf("\n");

          for(i = 0; i < 8; i++)
          {
             printf("0x%.2x ", tests[x].ct[i]);
          }
          printf("\n");
          return -2;
       }
   }
   return MYRC4ERR_SUCCESS;
}
#endif

#if 0

struct rc4_prng {
    int x, y;
    unsigned char buf[256];
};

typedef union
{
   struct rc4_prng rc4;
}prng_state;

#define CRYPT_OK 0
#define CRYPT_INVALID_ARG -1


/**
  Start the PRNG
  @param prng     [out] The PRNG state to initialize
  @return CRYPT_OK if successful
*/
int rc4_start(prng_state *prng)
{
    SYSV_ASSERT(prng != NULL);

    /* set keysize to zero */
    prng->rc4.x = 0;

    return CRYPT_OK;
}

/**
  Add entropy to the PRNG state
  @param in       The data to add
  @param inlen    Length of the data to add
  @param prng     PRNG state to update
  @return CRYPT_OK if successful
*/
int rc4_add_entropy(const unsigned char *in, unsigned long inlen, prng_state *prng)
{
    SYSV_ASSERT(in  != NULL);
    SYSV_ASSERT(prng != NULL);

    /* trim as required */
    if (prng->rc4.x + inlen > 256) {
       if (prng->rc4.x == 256) {
          /* I can't possibly accept another byte, ok maybe a mint wafer... */
          return CRYPT_OK;
       } else {
          /* only accept part of it */
          inlen = 256 - prng->rc4.x;
       }
    }

    while (inlen--) {
       prng->rc4.buf[prng->rc4.x++] = *in++;
       printf("prng->rc4.buf[%d] = %d\n", prng->rc4.x-1, prng->rc4.buf[prng->rc4.x-1]);
    }

    return CRYPT_OK;

}

/**
  Make the PRNG ready to read from
  @param prng   The PRNG to make active
  @return CRYPT_OK if successful
*/
int rc4_ready(prng_state *prng)
{
    unsigned char key[256], tmp, *s;
    int keylen, x, y, j;

    SYSV_ASSERT(prng != NULL);

    /* extract the key */
    s = prng->rc4.buf;
    SYSV_MEMCPY(key, s, 256);
    keylen = prng->rc4.x;

    /* make LTC_RC4 perm and shuffle */
    for (x = 0; x < 256; x++) {
        s[x] = x;
    }

    for (j = x = y = 0; x < 256; x++) {
        y = (y + prng->rc4.buf[x] + key[j++]) & 255;
        if (j == keylen) {
           j = 0;
        }
        tmp = s[x]; s[x] = s[y]; s[y] = tmp;
    }
    prng->rc4.x = 0;
    prng->rc4.y = 0;

    for (x = 0; x < 16; x++)
    {
       printf("prng->rc4.buf[%d] = %x\n", x, prng->rc4.buf[x]);
    }

#ifdef LTC_CLEAN_STACK
    zeromem(key, sizeof(key));
#endif

    return CRYPT_OK;
}

/**
  Read from the PRNG
  @param out      Destination
  @param outlen   Length of output
  @param prng     The active PRNG to read from
  @return Number of octets read
*/
unsigned long rc4_read(unsigned char *out, unsigned long outlen, prng_state *prng)
{
   unsigned char x, y, *s, tmp;
   unsigned long n;

   SYSV_ASSERT(out != NULL);
   SYSV_ASSERT(prng != NULL);

#ifdef LTC_VALGRIND
   zeromem(out, outlen);
#endif

   n = outlen;
   x = prng->rc4.x;
   y = prng->rc4.y;
   s = prng->rc4.buf;
   while (outlen--) {
      x = (x + 1) & 255;
      y = (y + s[x]) & 255;
      tmp = s[x]; s[x] = s[y]; s[y] = tmp;
      tmp = (s[x] + s[y]) & 255;
      *out++ ^= s[tmp];
   }
   prng->rc4.x = x;
   prng->rc4.y = y;
   return n;
}

/**
  Terminate the PRNG
  @param prng   The PRNG to terminate
  @return CRYPT_OK if successful
*/
int rc4_done(prng_state *prng)
{
   SYSV_ASSERT(prng != NULL);
   return CRYPT_OK;
}

#define CRYPT_BUFFER_OVERFLOW -3
#define CRYPT_ERROR_READPRNG -4

/**
  Export the PRNG state
  @param out       [out] Destination
  @param outlen    [in/out] Max size and resulting size of the state
  @param prng      The PRNG to export
  @return CRYPT_OK if successful
*/
int rc4_export(unsigned char *out, unsigned long *outlen, prng_state *prng)
{
   SYSV_ASSERT(outlen != NULL);
   SYSV_ASSERT(out    != NULL);
   SYSV_ASSERT(prng   != NULL);

   if (*outlen < 32) {
      *outlen = 32;
      return CRYPT_BUFFER_OVERFLOW;
   }

   if (rc4_read(out, 32, prng) != 32) {
      return CRYPT_ERROR_READPRNG;
   }
   *outlen = 32;

   return CRYPT_OK;
}

/**
  Import a PRNG state
  @param in       The PRNG state
  @param inlen    Size of the state
  @param prng     The PRNG to import
  @return CRYPT_OK if successful
*/
int rc4_import(const unsigned char *in, unsigned long inlen, prng_state *prng)
{
   int err;
   SYSV_ASSERT(in   != NULL);
   SYSV_ASSERT(prng != NULL);

   if (inlen != 32) {
      return CRYPT_INVALID_ARG;
   }

   if ((err = rc4_start(prng)) != CRYPT_OK) {
      return err;
   }
   return rc4_add_entropy(in, 32, prng);
}

/**
  PRNG self-test
  @return CRYPT_OK if successful, CRYPT_NOP if self-testing has been disabled
*/
int rc4_test(void)
{
   static const struct {
      unsigned char key[8], pt[8], ct[8];
   } tests[] = {
{
   { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef },
   { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef },
   { 0x75, 0xb7, 0x87, 0x80, 0x99, 0xe0, 0xc5, 0x96 }
}
};
   prng_state prng;
   unsigned char dst[8];
   int err, x;

   for (x = 0; x < (int)(sizeof(tests)/sizeof(tests[0])); x++) {
       if ((err = rc4_start(&prng)) != CRYPT_OK) {
          return err;
       }
       if ((err = rc4_add_entropy(tests[x].key, 8, &prng)) != CRYPT_OK) {
          return err;
       }
       if ((err = rc4_ready(&prng)) != CRYPT_OK) {
          return err;
       }
       SYSV_MEMCPY(dst, tests[x].pt, 8);
       if (rc4_read(dst, 8, &prng) != 8) {
          return -1;
       }
       rc4_done(&prng);
       if (SYSV_MEMCMP(dst, tests[x].ct, 8)) {
#if 1
          int y;
          printf("\n\nLTC_RC4 failed, I got:\n");
          for (y = 0; y < 8; y++) printf("%02x ", dst[y]);
          printf("\n");
#endif
          return -2;
       }
   }
   return CRYPT_OK;
}
#endif
