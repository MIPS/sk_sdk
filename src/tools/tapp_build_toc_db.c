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
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#include <sk_both.h>
#include "sk_swld.h"

#define TAPP_TOC_DB_TOOL_VERSION   0x00000001
#define MAX_ELEMENTS               8
#define MAX_FILENAME_LENGTH        256
#define MAX_DIRNAME_LENGTH         256
#define MAX_BUFFER_SIZE            32768
#define MAX_CARRIER_BUFFER_SIZE    4096
#define TAPP_TOOL_SUCCEED          0
#define TAPP_TOOL_FAILED           -1


uint8_t         buffer[MAX_BUFFER_SIZE];
uint32_t        buffer_size;


static const char usage_msg1[] =
"\n Secure Kernel TAPP TOC-DB Tool \n\n"\
"VERSION:     0x%.8x\n"\
"Usage: %s [-qh?] --in ... --out ...\n"\
" \n"\
" ----------------------------------------------------------------------------------------------------\n"\
" -i  --in      file  :MUST specify intput directory \n"\
" -o  --out     file  :MUST specify output directory \n"\
" ----------------------------------------------------------------------------------------------------\n"\
" -h  --help\n";

int32_t main (int argc, char **argv)
{
   int32_t         c;
   char         intput_directory[MAX_DIRNAME_LENGTH];
   char         output_directory[MAX_FILENAME_LENGTH];
   char         tapp_file[MAX_DIRNAME_LENGTH + MAX_FILENAME_LENGTH];
   char         output_toc_file[MAX_DIRNAME_LENGTH + MAX_FILENAME_LENGTH];
   char         output_db_file[MAX_DIRNAME_LENGTH + MAX_FILENAME_LENGTH];
   uint8_t         temp_binary_buffer[MAX_CARRIER_BUFFER_SIZE];
   char*           ip_dir;
   char*           op_dir;
   char*           op_toc;
   char*           op_db;
   struct dirent*  dp;
   DIR*            dfd;
   struct stat     st;
   tapp_toc_header hdr;
   uint8_t*        b;
   FILE*           fp;
   FILE*           fp_toc;
   FILE*           fp_db;
   uint32_t        num_bytes;
   uint32_t        address_db;
   SK_STATUS       err;
   uint8_t         read_entries;
   uint8_t         address[TAPP_TOC_INDEX_ENTRY_ADDR_SIZE];

   ip_dir       = 0;
   op_dir       = 0;
   op_toc       = 0;
   op_db        = 0;
   address_db   = 0;
   read_entries = 0;

   while ((c = getopt(argc, argv, "?h:i:o:r:"))!= -1)
   {
      switch (c)
      {
        case 'h':
        case '?':
           fprintf(stderr, usage_msg1, TAPP_TOC_DB_TOOL_VERSION, argv[0]);
           return 0;
           break;
        case 'i':
           strcpy(intput_directory,optarg);
           ip_dir = &intput_directory[0];
           break;
        case 'o':
           strcpy(output_directory,optarg);
           op_dir = &output_directory[0];
           break;
        case 'r':
           printf("optarg = [%s]\n", optarg);
           strcpy(output_toc_file,optarg);
           op_toc = &output_toc_file[0];
           read_entries = 1;
           break;
        default:
           printf("Bad input command %c\n", c);
           return TAPP_TOOL_FAILED;
           break;
      };
   };


   if (read_entries == 0)
   {
      if (ip_dir == 0)
      {
         printf("Error, must specify input directory\n");
         return TAPP_TOOL_FAILED;
      }

      if (op_dir == 0)
      {
         printf("Error, must specify output directory\n");
         return TAPP_TOOL_FAILED;
      }

      printf("INTPUT DIRECTORY: [%s] \n", ip_dir);
      printf("OUTPUT DIRECTORY: [%s] \n", op_dir);

      memset(&hdr, 0, sizeof(tapp_toc_header));

      memset(output_toc_file, 0, (MAX_DIRNAME_LENGTH + MAX_FILENAME_LENGTH));
      memset(output_db_file,  0, (MAX_DIRNAME_LENGTH + MAX_FILENAME_LENGTH));

      strcat(output_toc_file, op_dir);
      strcat(output_toc_file, "/");
      strcat(output_toc_file, SK_SECURE_STORAGE_TAPP_TOC_FILE);
      strcat(output_db_file,  op_dir);
      strcat(output_db_file,  "/");
      strcat(output_db_file,  SK_SECURE_STORAGE_TAPP_DB_FILE);

      op_toc = &output_toc_file[0];
      op_db  = &output_db_file[0];

      printf("op_toc: [%s] \n", op_toc);
      printf("op_db [%s] \n", op_db);

      /*
         allocate buffer count for header
      */
      buffer_size = sizeof(buffer);
      b = &buffer[0];


      TAPP_TOC_HDR_SET_COOKIE(&hdr, TAPP_TOC_INDEX_HDR_COOKIE);
      TAPP_TOC_HDR_SET_ENTRIES(&hdr, 0);

      /*
         set the header in buffer
      */
      if ((err = tapp_toc_write_header(b, buffer_size, &hdr, 0)) != SK_ERROR_SUCCESS)
      {
         printf("Failed to write header\n");
         return TAPP_TOOL_FAILED;
      }

      //b += TAPP_TOC_INDEX_HDR_TOTAL_SIZE;
      buffer_size -= TAPP_TOC_INDEX_HDR_TOTAL_SIZE;

      /*
         open DB file
      */
      if ((fp_db = fopen(output_db_file, "wb")) == 0)
      {
         printf("Error: Can not open DB file %s\n", output_db_file);
         return TAPP_TOOL_FAILED;
      }

      /*
         read the directory and collect all the files
      */
      if ((dfd = opendir(ip_dir)) == 0)
      {
         printf("dirwalk: can't open %s\n", ip_dir);
         return TAPP_TOOL_FAILED;
      }

      while ((dp = readdir(dfd)) != 0)
      {
         if (!(strcmp(dp->d_name, ".")) || !(strcmp(dp->d_name, "..")))
         {
            /*
               skip
            */
            continue;
         }

         memset(tapp_file, 0, sizeof(tapp_file));
         strcat(tapp_file, ip_dir);
         strcat(tapp_file, "/");
         strcat(tapp_file, dp->d_name);

         stat(tapp_file, &st);

         if(!S_ISDIR(st.st_mode))
         {
            hdr.entries++;

            if ((fp = fopen(tapp_file, "rb")) == 0)
            {
               printf("Unable to open file [%s]\n", tapp_file);
               return TAPP_TOOL_FAILED;
            }

            if (fread(temp_binary_buffer, 1, sizeof(tapps_info_data), fp) != sizeof(tapps_info_data))
            {
               printf("Failed to read the correct number of bytes from file [%s]\n", tapp_file);
               return TAPP_TOOL_FAILED;
            }

            memset(address, 0, TAPP_TOC_INDEX_ENTRY_ADDR_SIZE);
            address[0] = 0xff & (address_db>>24);
            address[1] = 0xff & (address_db>>16);
            address[2] = 0xff & (address_db>>8);
            address[3] = 0xff & (address_db>>0);

            if ((err = tapp_toc_add_entry(b, buffer_size,
                                          temp_binary_buffer, sizeof(tapps_info_data),
                                          st.st_size,
                                          address,0)) != SK_ERROR_SUCCESS)
            {
               printf("Failed to write entry\n");
               return TAPP_TOOL_FAILED;
            }

            buffer_size -= TAPP_TOC_INDEX_ENTRY_TOTAL_SIZE;

            /*
              copy the binary data DB
            */
            fwrite(temp_binary_buffer, 1, sizeof(tapps_info_data), fp_db);

            address_db += sizeof(tapps_info_data);
            while(!feof(fp))
            {
               num_bytes = fread(temp_binary_buffer, 1, sizeof(temp_binary_buffer), fp);
               fwrite(temp_binary_buffer, 1, num_bytes, fp_db);

               address_db += num_bytes;
            }

            fclose(fp);
         }
      }

      closedir(dfd);

      fclose(fp_db);

      /*
         write the TOC file
      */
      if ((fp_toc = fopen(output_toc_file, "wb")) == 0)
      {
         printf("Error: Can not open TOC file %s\n", output_toc_file);
         return TAPP_TOOL_FAILED;
      }

      if (fwrite(buffer, 1, sizeof(buffer), fp_toc) != sizeof(buffer))
      {
         printf("Failed to write the TOC binary file [%s]\n", output_toc_file);
         return TAPP_TOOL_FAILED;
      }

      fclose(fp_toc);
   }
   else
   {
      /*
         read the entries in the TOC
      */
      if (op_toc == 0)
      {
         printf("Error, must specify input TOC\n");
         return TAPP_TOOL_FAILED;
      }

      if ((fp_toc = fopen(op_toc, "rb")) == 0)
      {
         printf("Error: Can not open TOC file %s\n", output_toc_file);
         return TAPP_TOOL_FAILED;
      }

      num_bytes = fread(buffer, 1, sizeof(buffer), fp_toc);

      fclose(fp_toc);

      tapp_toc_list(buffer, num_bytes, 0);

   }

   return TAPP_TOOL_SUCCEED;
}






