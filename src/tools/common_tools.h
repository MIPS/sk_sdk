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

#ifndef _COMMON_TOOLS_H_
#define _COMMON_TOOLS_H_

#define ELP_COMMONG_PARSING_LINE_SIZE 2048

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define ELP_FILE           FILE
#define ELP_FOPEN(x,y,z)   fopen(x,y)
#define ELP_FREAD(w,x,y,z) fread(w,x,y,z)
#define ELP_FCLOSE(x)      fclose(x)
#define ELP_FSCANF(x,y,z)  fscanf(x,y,z)
#define ELP_FEOF(x)        feof(x)
#define ELP_FSEEK(x,y,z)   fseek(x,y,z)
#define ELP_SEEK_SET       SEEK_SET
#define ELP_SEEK_CUR       SEEK_CUR
#define ELP_SEEK_END       SEEK_END


typedef struct parse_config_info_
{
   char *name;
   int type;
   int size;
   unsigned char*  m;
   unsigned long*  u;
   unsigned short* s;
   int set;
}tparse_config_info;

typedef enum parse_type_
{
   ELP_CHAR_HEX = 0,
   ELP_CHAR_STRING,
   ELP_LONG,
   ELP_SHORT,
   ELP_BYTE,
}parse_type;

typedef enum variable_set_
{
   NOT_SET = 0,
   SET_RANDOM,
   SET,
}variable_set;

#define ESM_KPF_SIZE          16
#define ESM_DUK_SIZE          16
#define ESM_IVW_SIZE          12
#define ESM_IV_SIZE           16
#define ESM_TOPO_INT_KEY_SIZE 16
#define KEY_FILE_PKF_TAG      "PKf"
#define KEY_FILE_DUK_TAG      "DUK"
#define KEY_FILE_IVW_TAG      "IVwm"
#define WHITE_SPACE           0x20
#define EQUAL_SIGN            0x3d
#define MAX_LIN_LENGTH        2048

int     parse_config(tparse_config_info *ptr, int size, char* filename);
int     read_bytes(ELP_FILE **in, unsigned int iplen, unsigned char *op, unsigned int *oplen);
int32_t parse_key_file(char* key_file, uint8_t* kpkf, uint32_t* kpf_size, uint8_t* duk, uint32_t* duk_size, uint8_t* ivw, uint32_t* ivw_size);

#endif /* _COMMON_TOOLS_H_ */
