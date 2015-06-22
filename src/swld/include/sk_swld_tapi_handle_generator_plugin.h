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

#ifndef TAPI_HANDLE_GENERATOR_PLUGIN_H
#define TAPI_HANDLE_GENERATOR_PLUGIN_H

#include <sk_both.h>

#define MYRC4ERR_SUCCESS  0
#define MYRC4ERR_FAILED  -1

typedef struct _rc4_prng_state_
{
   uint32_t x, y;
   uint8_t buf[256];
}rc4_prng_state;

int32_t  my_rc4_prng_init  (rc4_prng_state *prng);
int32_t  my_rc4_prng_seed  (rc4_prng_state *prng, const uint8_t *in,  int32_t inlen);
uint32_t my_rc4_prng_read  (rc4_prng_state *prng, uint8_t *out, int32_t outlen);
int32_t  my_rc4_prng_end   (rc4_prng_state *prng);
int32_t my_rc4_prng_ready(rc4_prng_state *prng);

#define MY_HANDLE_GEN_MAX_ENTROPY_SIZE 16

typedef struct _tMyHandlerPluginObj_
{
   rc4_prng_state prng;
}tMyHandlerPluginObj;

typedef struct _tMyHandlerPluginInputParam_
{
   uint8_t  entropy[MY_HANDLE_GEN_MAX_ENTROPY_SIZE];
   uint32_t entropy_len;
}tMyHandlerPluginInputParam;

#endif
