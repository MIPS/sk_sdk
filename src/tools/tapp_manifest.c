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


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sk_both.h>
#include "common_tools.h"
#include "sk_swld.h"

#define TAPP_TOOL_VERSION   0x00000001
#define MAX_ELEMENTS        8
#define MAX_FILENAME_LENGTH 256
#define TAPP_TOOL_SUCCEED   0
#define TAPP_TOOL_FAILED   -1

tapps_info_data tapp_info;
uint8_t         buffer[sizeof(tapps_info_data)];
uint32_t        buffer_size;

tparse_config_info tapp_config[MAX_ELEMENTS] =
{
   {"TAID",           ELP_CHAR_HEX,    sizeof(tapp_info.taid),           tapp_info.taid,            0,                                         0, NOT_SET},
   {"MULTI_INSTANCE", ELP_BYTE,        sizeof(tapp_info.multi_instance), &tapp_info.multi_instance, 0,                                         0, NOT_SET},
   {"MULTI_SESSION",  ELP_BYTE,        sizeof(tapp_info.multi_session),  &tapp_info.multi_session,  0,                                         0, NOT_SET},
   {"MAX_HEAP_SIZE",  ELP_LONG,        sizeof(tapp_info.max_heap_size),  0,                         (unsigned long*)&tapp_info.max_heap_size,  0, NOT_SET},
   {"MAX_STACK_SIZE", ELP_LONG,        sizeof(tapp_info.max_stack_size), 0,                         (unsigned long*)&tapp_info.max_stack_size, 0, NOT_SET},
   {"SERVICE_NAME",   ELP_CHAR_STRING, sizeof(tapp_info.service_name),   tapp_info.service_name,    0,                                         0, NOT_SET},
   {"VENDOR_NAME",    ELP_CHAR_STRING, sizeof(tapp_info.vendor_name),    tapp_info.vendor_name,     0,                                         0, NOT_SET},
   {"DECRIPTION",     ELP_CHAR_STRING, sizeof(tapp_info.description),    tapp_info.description,     0,                                         0, NOT_SET},
};

static const char usage_msg1[] =
"\n Secure Kernel TAPP Manifest Tool \n\n"\
"VERSION:     0x%.8x\n"\
"Usage: %s [-qh?] -m -i -o ...\n"\
" \n"\
" ----------------------------------------------------------------------------------------------------\n"\
" -m  manifest file  :MUST specify manifest input file \n"\
" -i  file           :MUST specify configuration input file \n"\
" -o  directory      :MUST specify output directory \n"\
" ----------------------------------------------------------------------------------------------------\n"\
" -h  --help\n";

int32_t convert_hex2ascii(uint8_t hex_digit, char* str)
{
   uint8_t nb_cnt; /* count */
   uint8_t nb;     /* nibble */

   for (nb_cnt = 0; nb_cnt < 2; nb_cnt++)
   {
      nb = 0x0f & (hex_digit >> (4*(1 - nb_cnt)));
      switch(nb)
      {
         case 0:
            str[nb_cnt] = '0';
            break;
         case 1:
            str[nb_cnt] = '1';
            break;
         case 2:
            str[nb_cnt] = '2';
            break;
         case 3:
            str[nb_cnt] = '3';
            break;
         case 4:
            str[nb_cnt] = '4';
            break;
         case 5:
            str[nb_cnt] = '5';
            break;
         case 6:
            str[nb_cnt] = '6';
            break;
         case 7:
            str[nb_cnt] = '7';
            break;
         case 8:
            str[nb_cnt] = '8';
            break;
         case 9:
            str[nb_cnt] = '9';
            break;
         case 10:
            str[nb_cnt] = 'A';
            break;
         case 11:
            str[nb_cnt] = 'B';
            break;
         case 12:
            str[nb_cnt] = 'C';
            break;
         case 13:
            str[nb_cnt] = 'D';
            break;
         case 14:
            str[nb_cnt] = 'E';
            break;
         default: /* 15 */
            str[nb_cnt] = 'F';
            break;
            break;
      };
   }

   return 0;
}

int main (int argc, char **argv)
{
   int32_t  c;
   char  input_manifest_file[MAX_FILENAME_LENGTH];
   char  input_binary_file[MAX_FILENAME_LENGTH];
   char  output_combined_file[(MAX_FILENAME_LENGTH*2)];
   char  output_filename[MAX_FILENAME_LENGTH];
   char*    ip_file;
   char*    op_file;
   char*    m_file;
   FILE*    ifp;
   FILE*    ofp;
   uint32_t i;

   ip_file = 0;
   op_file = 0;
   m_file  = 0;

   while ((c = getopt(argc, argv, "?h:i:o:m:"))!= -1)
   {
      switch (c)
      {
        case 'h':
        case '?':
           fprintf(stderr, usage_msg1, TAPP_TOOL_VERSION, argv[0]);
           return 0;
           break;
        case 'm':
           strcpy(input_manifest_file,optarg);
           m_file = &input_manifest_file[0];
           break;
        case 'i':
           strcpy(input_binary_file,optarg);
           ip_file = &input_binary_file[0];
           break;
        case 'o':
           strcpy(output_combined_file,optarg);
           op_file = &output_combined_file[0];
           break;
        default:
           printf("Bad input command %c\n", c);
           return TAPP_TOOL_FAILED;
           break;
      };
   };

   if (ip_file == 0)
   {
      printf("Error, must specify input binary file\n");
      return TAPP_TOOL_FAILED;
   }

   if (op_file == 0)
   {
      printf("Error, must specify output binary file\n");
      return TAPP_TOOL_FAILED;
   }

   if (m_file == 0)
   {
      printf("Error, must specify manifest file\n");
      return TAPP_TOOL_FAILED;
   }

   printf("INTPUT FILE:  [%s] \n", ip_file);
   printf("OUTPUT OUTTPUT: [%s] \n", op_file);
   printf("MANIFEST: [%s] \n", m_file);

   parse_config(tapp_config, MAX_ELEMENTS, m_file);

   printf("TAID: ");
   for(i = 0; i < SK_TAPI_SESSION_TAID_SIZE; i++)
   {
      printf("%.2x ", tapp_info.taid[i]);
   }
   printf("\n");
   printf("multi_instance = %d\n", tapp_info.multi_instance);
   printf("multi_session = %d\n", tapp_info.multi_session);
   printf("max_heap_size = %d\n", tapp_info.max_heap_size);
   printf("max_stack_size = %d\n", tapp_info.max_stack_size);
   printf("service_name = [%s]\n", tapp_info.service_name);
   printf("vendor_name = [%s]\n", tapp_info.vendor_name);
   printf("description = [%s]\n", tapp_info.description);

   buffer_size = sizeof(buffer);
   tapp_manifest_pack_put(&tapp_info, buffer, &buffer_size);

   if ((ifp = fopen(ip_file, "rb")) == 0)
   {
      printf("Unable to open file [%s]\n", ip_file);
      return TAPP_TOOL_FAILED;
   }

   for(i = 0; i < SK_TAPI_SESSION_TAID_SIZE; i++)
   {
      convert_hex2ascii(tapp_info.taid[i], &output_filename[(i*2)]);
   }

   output_filename[(SK_TAPI_SESSION_TAID_SIZE*2)] = 0;

   printf("output filename: [%s] [%d]\n",output_filename, strlen(output_filename));

   strcat(output_combined_file, "/");
   strcat(output_combined_file, output_filename);

   printf("output_combined_file: [%s]\n",output_combined_file);

   if ((ofp = fopen(op_file, "wb")) == 0)
   {
      printf("Unable to open file [%s]\n", op_file);
      return TAPP_TOOL_FAILED;
   }

   if (fwrite(buffer, 1, buffer_size, ofp) != buffer_size)
   {
      printf("Failed to write to manifest file [%s]\n", op_file);
      return TAPP_TOOL_FAILED;
   }

   while(!feof(ifp))
   {
      fread(buffer, 1, sizeof(buffer), ifp);
      fwrite(buffer, 1, sizeof(buffer), ofp);
   };

   fclose(ifp);
   fclose(ofp);

   return TAPP_TOOL_SUCCEED;
}






