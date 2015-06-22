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

SK_STATUS sk_swld_tapi_handle_generator_plugin_init(tApiHandlerPluginObj* tObj, void* param)
{
   SK_STATUS                  err;
   tMyHandlerPluginInputParam *my_param;
   tMyHandlerPluginObj        *inst;

   if ((tObj == 0) || (param == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   my_param = (tMyHandlerPluginInputParam *)param;

   inst = (tMyHandlerPluginObj *)SYSV_MALLOC(sizeof(tMyHandlerPluginObj));

   if (inst == 0)
   {
      err = SK_ERROR_NO_MEMORY;
      goto DONE;
   }

   if (((err = my_rc4_prng_init(&inst->prng)) != MYRC4ERR_SUCCESS) ||
       ((err = my_rc4_prng_seed(&inst->prng, my_param->entropy, my_param->entropy_len)) != MYRC4ERR_SUCCESS) ||
       ((err = my_rc4_prng_ready(&inst->prng)) != MYRC4ERR_SUCCESS))
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error opening prng [%d]", err);
      err = SK_ERROR_PRNG_INIT_FAILED;
      goto DONE;
   }

   tObj->instance = inst;

DONE:
   return err;
}

SK_STATUS sk_swld_tapi_handle_generator_plugin_end(tApiHandlerPluginObj* tObj)
{
   SK_STATUS           err;
   tMyHandlerPluginObj *inst;

   if (tObj == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   err = SK_ERROR_SUCCESS;

   inst = tObj->instance;

   my_rc4_prng_end(&inst->prng);

   return err;
}

SK_STATUS sk_swld_tapi_handle_generator_plugin_generate_context_handle(tApiHandlerPluginObj* tObj, uint8_t* handle, uint32_t handle_size)
{
   SK_STATUS           err;
   tMyHandlerPluginObj *inst;

   if (tObj == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   inst = tObj->instance;

   if ((err = my_rc4_prng_read(&inst->prng, handle, handle_size)) != (int32_t) handle_size)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error generating handle [%d]", err);
      err = SK_ERROR_PRNG_READ_FAILED;
      goto DONE;
   }

DONE:
   return err;
}

SK_STATUS sk_swld_tapi_handle_generator_plugin_find_context_handle(tApiHandlerPluginObj* tObj,
                                                           uint32_t table_size,
                                                           uint8_t* table,
                                                           uint8_t* context_handle,
                                                           uint32_t handle_size,
                                                           uint32_t* idx)
{
   SK_STATUS           err;
   uint32_t            i;

   if ((tObj == 0) || (table_size == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   err = SK_ERROR_SUCCESS;

   for(i = 0; i < table_size; i++)
   {
      if (SYSV_MEMCMP(&table[(i*handle_size)], context_handle,  handle_size) == 0)
      {
         (*idx) = (i*handle_size);
         goto DONE;
      }
   }

   err = SK_ERROR_NOT_FOUND;

DONE:
   return err;
}

SK_STATUS sk_swld_tapi_handle_generator_plugin_generate_session_handle(tApiHandlerPluginObj* tObj,
                                                               uint8_t* handle,
                                                               uint32_t handle_size)
{
   SK_STATUS           err;
   tMyHandlerPluginObj *inst;

   if (tObj == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   inst = tObj->instance;

   if ((err = my_rc4_prng_read(&inst->prng, handle, handle_size)) != (int32_t)handle_size)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error generating handle [%d]", err);
      err = SK_ERROR_PRNG_READ_FAILED;
      goto DONE;
   }

DONE:
   return err;
}

SK_STATUS sk_swld_tapi_handle_generator_plugin_find_session_handle(tApiHandlerPluginObj* tObj,
                                                           uint32_t table_size,
                                                           uint8_t* table,
                                                           uint8_t* session_handle,
                                                           uint32_t handle_size,
                                                           uint32_t* idx)
{
   SK_STATUS err;
   uint32_t  i;

   if ((tObj == 0) || (table_size == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   err = SK_ERROR_SUCCESS;

   for(i = 0; i < table_size; i++)
   {
      if (SYSV_MEMCMP(&table[(i*handle_size)],session_handle,handle_size) == 0)
      {
         (*idx) = (i*handle_size);
         goto DONE;
      }
   }

   err = SK_ERROR_NOT_FOUND;

DONE:
   return err;
}






