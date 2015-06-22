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

/* dependencies */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int32_t sysv_memcmp(const void* s1, const void* s2, uint32_t n)
{
   return memcmp(s1, s2, n);
}

void *sysv_memset(void* s, int32_t c, uint32_t n)
{
   return memset(s,c,n);
}

void *sysv_memcpy(void *dest, const void *src, uint32_t n)
{
   return memcpy(dest,src,n);
}

void *sysv_memmove(void *dest, const void *src, uint32_t n)
{
   return memmove(dest,src,n);
}

uint8_t *sysv_strcpy(void* dest, const void* src)
{
   return (uint8_t *) strcpy(dest, src);
}

uint8_t *sysv_strncpy(void* dest, const void* src, uint32_t n)
{
   return (uint8_t *) strncpy(dest, src, n);
}

uint32_t sysv_strlen(const void* str)
{
   return strlen(str);
}

uint8_t *sysv_strncat(void *dest, void *src, uint32_t n)
{
   return (uint8_t *) strncat(dest, src, n);
}

int32_t sysv_strcmp(const void* str1, const void* str2)
{
   return strcmp(str1, str2);
}

void *sysv_malloc(uint32_t size)
{
   return malloc(size);
}

void sysv_free(void* ptr)
{
   free(ptr);
}

uint32_t sysv_system_prng(void *out, uint32_t outlen)
{
   (void)out;
   (void)outlen;
   return 0;
}

void *sysv_calloc(uint32_t nmemb, uint32_t size)
{
   (void)nmemb;
   (void)size;
   return 0;
}

/* this is a dangerous function, if realloc allocates to a larger memory, there will
   be a buffer overrun */
void *sysv_realloc(void *ptr, uint32_t size)
{
   (void)ptr;
   (void)size;
   return 0;
}

uint32_t sysv_htonl(uint32_t hostlong)
{
  return htonl(hostlong);
}

uint16_t sysv_htons(uint16_t hostshort)
{
   return htons(hostshort);
}

uint32_t sysv_ntohl(uint32_t netlong)
{
  return ntohl(netlong);
}

uint16_t sysv_ntohs(uint16_t netshort)
{
  return ntohs(netshort);
}

void sysv_assert2(uint8_t x, uint8_t const* func_name, uint8_t const* file_name, uint32_t line_number)
{
   if (!x)
   {
      printf("SYSV ASSERTION: %s / %s() / %d\n", file_name, func_name, line_number);
      exit(EXIT_FAILURE);
   }
}

/* assume the incoming data is in BIG ENDIAN format */
uint16_t sysv_btohs(uint8_t* c) /* converts BYTES to HOST */
{
   uint16_t num;

   num = *c;    /* start with MSB at lower address */
   num <<= 8;
   c++;
   num |= (*c); /* followed by LSB at higher address */

   return num;
}

void sysv_htobs(uint16_t val, uint8_t* c) /* converts HOST to BYTES */
{
   *c = ((val & 0xFF00) >> 8); /* start with MSB at lower address */
   c++;
   *c = (val & 0x00FF);        /* followed by LSB at higher address */
}

extern int32_t sk_nwld_platform_tapi_ioctl_proc(int32_t d, uint32_t request, void* arg);

int32_t sysv_ioctl_1(int32_t d, uint32_t request, void* arg)
{
   (void)d;
   (void)request;
   (void)arg;
#ifdef SK_NWLD
   return sk_nwld_platform_tapi_ioctl_proc(d, request, arg);
#else
   return 0;
#endif
}

int32_t sysv_open(const char *pathname, int32_t flags)
{
   (void)pathname;
   (void)flags;
   return 0;
}
