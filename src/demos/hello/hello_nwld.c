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
#include <string.h>

#include <sk_nwld.h>
#include <sk_drv_nwld_platform.h>

static SK_TAID taid[2] = {
 { 0x12, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
 { 0x13, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
};

int main(int argc, char **argv)
{
   SKTEEC_STATUS    res;
   SKTEEC_Context   context;
   SKTEEC_Session   session;
   SKTEEC_Operation operation;

   char buf[128];
   int  n;

   (void)argv;

   my_platform_param platform_param;
   sk_nwld_teec_platform obj;

#ifdef SK_TARGET_FIASCO
   platform_param.device_name = "/dev/karma_chr_intervm";
#elif defined(SK_TARGET_TCP)
   platform_param.port    = 6000;
   platform_param.address = (127U) + (1U<<24);
#endif

   sk_nwld_platform_init(&obj, (void *)&platform_param);

   res = SKTEEC_OpenContext(&context, "/dev/karma_chr_intervm", 0x1010, NULL);
   if (res != 0) {
      printf("Failed to open a context\n");
      exit(EXIT_FAILURE);
   }

   res = SKTEEC_OpenSession(&context, &session, &taid[argc == 1 ? 0 : 1][0], NULL);
   if (res != 0) {
      printf("Failed to open a session\n");
      exit(EXIT_FAILURE);
   }

   operation.num_param                     = 2;
   operation.param[0].meminfo              = calloc(1, sizeof(*operation.param[0].meminfo));
   if (!operation.param[0].meminfo) {
      printf("Ran out of memory\n");
      exit(EXIT_FAILURE);
   }
   operation.param[0].meminfo->memory_size = sizeof(buf);
   operation.param[0].meminfo->buffer      = buf;
   operation.param[0].meminfo->flag        = SKTEEC_MEMREF_OUT;

   operation.param[1].meminfo              = calloc(1, sizeof(*operation.param[0].meminfo));
   if (!operation.param[1].meminfo) {
      printf("Ran out of memory\n");
      exit(EXIT_FAILURE);
   }
   operation.param[1].meminfo->memory_size = sizeof(buf);
   operation.param[1].meminfo->buffer      = buf;
   operation.param[1].meminfo->flag        = SKTEEC_MEMREF_IN;

   n = 0;
   do {
      sprintf(buf, "hello %d world", n++);
      res = SKTEEC_InvokeCommand(&session, 0, &operation);
      if (res != 0) {
         printf("Failed to invoke a command\n");
         exit(EXIT_FAILURE);
      }
      printf("Response(%d): [%s]\n", argc != 1, buf);
      sleep(3);
   } while (3 < 4);

   return 0;
}
