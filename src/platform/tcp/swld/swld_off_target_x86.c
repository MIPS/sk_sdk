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

#include <sk_both.h>
#include <sk_swld.h>
#include "sk_drv_server_comm.h"
/*
   Shared information
*/
#define CONTEXT_ID      0x1010
#define SM_ID           0x2020
#define MAX_CONNECTIONS 1
#define MAX_SESSIONS    64

#define ELP_PLATFORM_TARGET_PRINT  printf
#define ELP_PLATFORM_TARGET_MALLOC malloc
#define ELP_PLATFORM_TARGET_FREE   free

/*
   TAPI core memory structure
*/
#define MAX_CORE_MEM_SIZE 1024

/*
  Communication Driver
*/
#define COMM_PORT 6030
#define COMM_MAX_BUFFER_SIZE 2048

extern char *ramdisk_path;

int32_t sw_main (void)
{
   int32_t                    err;
   sk_swld_tapi                    *tapi;
   tMyHandlerPluginInputParam param;
   uint32_t                   port;
   tSkSysSrvCommOffTargetObj  myComm;
   sk_nwld_to_swld_msg_api        msgApi;
   sk_swld_tapi_kernel_access   kaApi;

   ramdisk_path = "./";

   err  = SK_ERROR_SUCCESS;
   port = 6000;

   if ((tapi = ELP_PLATFORM_TARGET_MALLOC(sizeof(sk_swld_tapi))) == 0)
   {
      ELP_PLATFORM_TARGET_PRINT("Error allocating memory for TAPI object");
      err = -1;
      goto DONE;
   }

   ELP_PLATFORM_TARGET_PRINT("Waiting for Connection ...\n");
   if ((err = sk_swld_platform_comm_init(&myComm, port)) != 0)
   {
      ELP_PLATFORM_TARGET_PRINT("Error connecting to IP address [%d]\n", err);
      goto DONE;
   }

   ELP_PLATFORM_TARGET_PRINT("Client Connected\n");

   param.entropy_len = MY_HANDLE_GEN_MAX_ENTROPY_SIZE;

   msgApi.mObj          = (void *)&myComm;
   msgApi.tapi_recv_msg = sk_swld_platform_comm_recv;
   msgApi.tapi_send_msg = sk_swld_platform_comm_send;

   if ((err = sk_swld_tapi_init(tapi, CONTEXT_ID, SM_ID, MAX_CORE_MEM_SIZE, MAX_CORE_MEM_SIZE, &msgApi, &kaApi, (void *)&param)) != SK_ERROR_SUCCESS)
   {
      ELP_PLATFORM_TARGET_PRINT("Error opening TAPI [%d]", err);
      goto DONE;
   }

   if ((err = sk_swld_tapi_dispatch(tapi)) != SK_ERROR_SUCCESS)
   {
      ELP_PLATFORM_TARGET_PRINT("Error processing TAPI [%d]", err);
      goto DONE;
   }

   sk_swld_tapi_close(tapi);

   sk_swld_platform_comm_end(&myComm);

   ELP_PLATFORM_TARGET_FREE(tapi);

DONE:
   return 0;
}
