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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SK_STATUS sk_drv_swld_memory_config_allocate_memory(uint8_t *mem, uint32_t mem_size, uint32_t flag, uint8_t* memory_id, uint32_t* memory_id_length, void* info, void* log)
{
   SK_STATUS err;
   uint32_t  i;
   (void)info;
   (void)log;

   err = SK_ERROR_SUCCESS;

   for(i=0; i<(*memory_id_length);i++)
   {
      memory_id[i] = rand()%256;
   }

   printf("flag = 0x%x\n", flag);
   printf("mem_size = 0x%x\n", mem_size);
   printf("mem = 0x%p\n", mem);

   return err;
}

SK_STATUS sk_drv_swld_memory_config_free_memory(uint8_t *mem, void* info, void* log)
{
   SK_STATUS err;

   (void)mem;
   (void)info;
   (void)log;

   err = SK_ERROR_SUCCESS;

   return err;
}
