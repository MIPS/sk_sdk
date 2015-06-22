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
#include <ctype.h>

#include <sk_both.h>
#include <sk_swld.h>
#include <sk_drv_server_comm.h>

int32_t sw_main (void);

SK_TA_STATUS SKTAPP_init_iface(void* session)
{
   (void)session;
   return 0;
}

SK_TA_STATUS SKTAPP_close_iface(void* session)
{
   (void)session;
   return 0;
}

SK_TA_STATUS SKTAPP_receive_operation_iface(uint8_t *taid, void* session, uint32_t cmd, sk_operation* op)
{
   uint32_t   i;
   char *r, *s;
   (void)session;
   (void)cmd;
   (void)taid;

   printf("Received[%d]: %s\n", op->param[0].mem.memory_size, (char*)op->param[0].mem.addr);

   r = (char*)op->param[0].mem.addr;
   s = (char*)op->param[1].mem.addr;
   for (i = 0; i < op->param[0].mem.memory_size; i++) {
      s[i] = toupper(r[i]);
   }
   printf("Returned[%d]: %s\n", op->param[1].mem.memory_size, (char*)op->param[1].mem.addr);
   return 0;

}

void SKTAPP_getapi(sktapp_func_def *api)
{
   api->SKTAPP_init_iface = SKTAPP_init_iface;
   api->SKTAPP_close_iface = SKTAPP_close_iface;
   api->SKTAPP_receive_operation_iface = SKTAPP_receive_operation_iface;
}

int main(void)
{
   sw_main();
}

