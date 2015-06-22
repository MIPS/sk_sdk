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


#include <string.h>
#include <sk_nwld.h>
#include <sk_drv_nwld_platform.h>

static sk_nwld_tapi gtApi;
static tSkSysCltCommVmFiascoObj* gtCommObj;


int32_t sk_nwld_platform_tapi_ioctl_init(uint32_t max_connections,
                                    uint32_t max_sessions,
                                    uint32_t sm_id,
                                    char *device_name)
{
   SKTEEC_STATUS err;
   uint32_t      i;

   /*
      initialize Socket Driver
   */
   gtCommObj = malloc(sizeof(tSkSysCltCommVmFiascoObj) * SK_TAPI_MAX_CONTEXT_IDS);

   memset(gtCommObj, 0, (sizeof(tSkSysCltCommVmFiascoObj) * SK_TAPI_MAX_CONTEXT_IDS));

   for(i = 0; i < SK_TAPI_MAX_CONTEXT_IDS; i++)
   {
      strcpy(gtCommObj[i].device_name, device_name);
   }

   if ((err = sk_nwld_tapi_init(&gtApi, max_connections, max_sessions, sm_id, gtCommObj)) != SK_ERROR_SUCCESS)
   {
      printf("Error opening client API\n");
      err = SK_ERROR_FAILED;
   }

   return err;
}

int32_t sk_nwld_platform_tapi_ioctl_end(void)
{
   return SK_ERROR_SUCCESS;
}

int32_t sk_nwld_platform_tapi_ioctl_debug(void)
{
   sk_nwld_tapi_print_info(&gtApi);

   return SK_ERROR_SUCCESS;
}

int32_t sk_nwld_platform_tapi_ioctl_proc(int32_t d, uint32_t request, void* arg)
{
   SKTEEC_STATUS       err;
   sk_nwld_tapi     *tapi;
   sk_teec_user_info   *info;
   uint8_t             send_buffer[SK_DRV_MAX_MSGBUFFER_SIZE];
   uint32_t            send_buffer_size;
   uint8_t             recv_buffer[SK_DRV_MAX_MSGBUFFER_SIZE];
   uint32_t            recv_buffer_size;

   (void)d;

   /*
     default: set the error state
   */
   err              = SK_ERROR_SUCCESS;
   info             = (sk_teec_user_info *)arg;
   send_buffer_size = sizeof(send_buffer);
   recv_buffer_size = sizeof(recv_buffer);


// this is intentional to silence a warning and provoke SYSV_ERROR to crash...
   tapi = NULL;

   switch(request)
   {
      case SK_TAPI_OPEN_CONTEXT:
         if ((err = sk_nwld_tapi_open_context(&gtApi, info, send_buffer, send_buffer_size, recv_buffer, &recv_buffer_size)) != SK_ERROR_SUCCESS)
         {
            SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR,  "Error: Failed to open context [%d]", err);
         }
         break;
      case SK_TAPI_CLOSE_CONTEXT:
         if ((err = sk_nwld_tapi_close_context(&gtApi, info, send_buffer, send_buffer_size, recv_buffer, &recv_buffer_size)) != SK_ERROR_SUCCESS)
         {
            SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: Failed to close context [%d]", err);
         }
         break;
      case SK_TAPI_OPEN_SESSION:
         if ((err = sk_nwld_tapi_open_session(&gtApi, info, send_buffer, send_buffer_size, recv_buffer, &recv_buffer_size)) != SK_ERROR_SUCCESS)
         {
            SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: Failed to open session [%d]", err);
         }
         break;
      case SK_TAPI_CLOSE_SESSION:
         if ((err = sk_nwld_tapi_close_session(&gtApi, info, send_buffer, send_buffer_size, recv_buffer, &recv_buffer_size)) != SK_ERROR_SUCCESS)
         {
            SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: Failed to close session [%d]", err);
         }
         break;
      case SK_TAPI_INVOKE_COMMAND:
         if ((err = sk_nwld_tapi_invoke_command(&gtApi, info, send_buffer, send_buffer_size, recv_buffer, &recv_buffer_size)) != SK_ERROR_SUCCESS)
         {
            SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: Failed to invoke command [%d]", err);
         }
         break;
      case SK_TAPI_REGISTER_MEMORY:
         if ((err = sk_nwld_tapi_register_memory(&gtApi, info, send_buffer, send_buffer_size, recv_buffer, &recv_buffer_size)) != SK_ERROR_SUCCESS)
         {
            SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: Failed to register memory [%d]", err);
         }
         break;
      case SK_TAPI_UNREGISTER_MEMORY:
         if ((err = sk_nwld_tapi_unregister_memory(&gtApi, info, send_buffer, send_buffer_size, recv_buffer, &recv_buffer_size)) != SK_ERROR_SUCCESS)
         {
            SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: Failed to unregister memory [%d]", err);
         }
         break;
      default:
         SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: Invalid IOCTL command [%d]", request);
         break;
   }

   return err;
}










