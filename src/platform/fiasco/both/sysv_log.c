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

#include <sk_both.h>
#include <sysv_platform_specific_system.h>

/* USER additional logging flags will be on the top 16 bytes */

void sysv_log_init(void **log, uint32_t mask, const uint8_t* name, void* log_param)
{
   (void)log;
   (void)mask;
   (void)name;
   (void)log_param;
}

void sysv_log_end(void *log)
{
   (void)log;
}




void sysv_log_write(uint32_t tag, void *log, uint8_t* file, uint32_t line, const char *format, ...)
{
   va_list args;
   (void)log;

   va_start (args, format);

   switch(tag)
   {

     case SYSV_LOG_EXTERNAL:
        printf("[%s]:EXT:[%s:%d]:", "simx86", file, line);
         break;
      case SYSV_LOG_INFO:
         printf("[%s]:INFO:[%s:%d]:", "simx86", file, line);
         break;
      case SYSV_LOG_TIME:
         printf("[%s]:INFO:[%s:%d]:", "simx86", file, line);
         break;
      case SYSV_LOG_WARNING_ERR:
         printf("[%s]:WARN:[%s:%d]:", "simx86", file, line);
         break;
      default:
         printf("[%s]:ERRO:[%s:%d]:", "simx86", file, line);
         break;
   };
   vprintf(format, args);

   printf("\n");

   va_end(args);
}

void sysv_log_dump(void *log, uint8_t* string, uint8_t* table, uint32_t size)
{
   uint32_t i;
   (void)log;

   if (size == (uint32_t)(-1))
   {
      return;
   }

   if (table != NULL)
   {
      printf("[%s]\n", string);

      for(i=0; i < size; i++)
      {
         printf("%.2x ", table[i]);

         if ((i >0) && (((i + 1)%16) == 0))
         {
            printf("\n");
         }
      }

      printf("\n");
   }
}
