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


#include <sk_nwld.h>


SKTEEC_STATUS SKTEEC_OpenContext(SKTEEC_Context* context,
                                 const char* device_name,
                                 uint32_t contextID,
                                 void* opaqueCredentials)
{
   SKTEEC_STATUS             err;
   sk_teec_user_open_context oc;

   (void)opaqueCredentials;

   if ((context == 0) || (device_name == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   SYSV_MEMSET(context, 0, sizeof(SKTEEC_Context));

   context->device = SYSV_OPEN(device_name, SYSV_OPEN_RDWR);

   if (context->device == -1)
   {
      SYSV_ERROR2(0, SYSV_LOG_CRITICAL_ERR, "Error opening device [%s] [%d]", device_name, context->device);
      err = SK_ERROR_DRIVER_OPEN_FAILED;
      goto DONE;
   }

   oc.context_id = contextID;

   err = SYSV_IOCTL_1(context->device, SK_TAPI_OPEN_CONTEXT, &oc);

   if (err != 0)
   {
      SYSV_ERROR1(0, SYSV_LOG_CRITICAL_ERR, "Error calling IOCTL with command SKTEEC_OPEN_CONTEXT [%d]", err);
      err = SK_ERROR_SEND_TO_DRIVER;
      goto DONE;
   }
   else
   {
      context->handle = oc.context_handle;
   }

DONE:
   SYSV_MEMSET(&oc, 0, sizeof(sk_teec_user_open_context));
   return err;
}

SKTEEC_STATUS SKTEEC_CloseContext(SKTEEC_Context* context)
{
   SKTEEC_STATUS              err;
   sk_teec_user_close_context cc;

   if (context == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   cc.context_handle = context->handle;

   err = SYSV_IOCTL_1(context->device, SK_TAPI_CLOSE_CONTEXT, &cc);

   if (err != 0)
   {
      SYSV_ERROR1(0, SYSV_LOG_CRITICAL_ERR, "Error calling IOCTL with command SK_TAPI_CLOSE_CONTEXT [%d]", err);
      err = SK_ERROR_SEND_TO_DRIVER;
      goto DONE;
   }

   SYSV_MEMSET(context, 0, sizeof(SKTEEC_Context));

DONE:
   SYSV_MEMSET(&cc, 0, sizeof(sk_teec_user_close_context));
   return err;
}

SKTEEC_STATUS SKTEEC_OpenSession(SKTEEC_Context* context,
                                 SKTEEC_Session* session,
                                 SK_TAID TAID,
                                 void* opaqueCredentials)
{
   SKTEEC_STATUS             err;
   sk_teec_user_open_session os;

   (void)opaqueCredentials;

   if ((context == 0) || (session == 0) || (TAID == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   SYSV_MEMSET(session, 0, sizeof(SKTEEC_Session));

   os.context_handle = context->handle;
   SYSV_MEMCPY(os.taid, TAID, SK_TAPI_SESSION_TAID_SIZE);

   err = SYSV_IOCTL_1(context->device, SK_TAPI_OPEN_SESSION, &os);

   if (err != 0)
   {
      SYSV_ERROR1(0, SYSV_LOG_CRITICAL_ERR, "Error calling IOCTL with command SK_TAPI_OPEN_SESSION [%d]", err);
      err = SK_ERROR_SEND_TO_DRIVER;
      goto DONE;
   }
   else
   {
      session->handle = os.session_handle;

      /*
         capture pointer to context
      */
      session->context = context;
   }

   SYSV_MEMSET(&os, 0, sizeof(sk_teec_user_open_session));

DONE:
   return err;
}

SKTEEC_STATUS SKTEEC_CloseSession(SKTEEC_Session* session)
{
   SKTEEC_STATUS              err;
   sk_teec_user_close_session cs;
   SKTEEC_Context*            context;

   if (session == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   context           = (SKTEEC_Context *)session->context;
   cs.session_handle = session->handle;
   cs.context_handle = context->handle;

   err = SYSV_IOCTL_1(context->device, SK_TAPI_CLOSE_SESSION, &cs);

   if (err != 0)
   {
      SYSV_ERROR1(0, SYSV_LOG_CRITICAL_ERR, "Error calling IOCTL with command SK_TAPI_CLOSE_SESSION [%d]", err);
      err = SK_ERROR_SEND_TO_DRIVER;
      goto DONE;
   }

DONE:
   SYSV_MEMSET(&cs, 0, sizeof(sk_teec_user_close_session));
   return err;
}

SKTEEC_STATUS SKTEEC_InvokeCommand(SKTEEC_Session* session,
                                   uint32_t commandID,
                                   SKTEEC_Operation* operation)
{
   SKTEEC_STATUS              err;
   sk_teec_user_invoke_cmd    ic;
   SKTEEC_Context*            context;
   uint32_t                   i;

   if ((session == 0)   ||
       (operation == 0) ||
       (operation->num_param > SK_MAXIMUM_OPERATIONS))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   context           = (SKTEEC_Context *)session->context;
   ic.session_handle = session->handle;
   ic.context_handle = context->handle;
   ic.cmd            = commandID;
   ic.op.num_param   = operation->num_param;

   for(i = 0; i < ic.op.num_param; i++)
   {
      ic.op.param[i].mem.memory_size = operation->param[i].meminfo->memory_size;
      ic.op.param[i].mem.flag        = operation->param[i].meminfo->flag;
      ic.op.param[i].mem.addr        = (SK_ADDRESS_TYPE)operation->param[i].meminfo->buffer;
      SYSV_MEMCPY(ic.op.param[i].mem.id, operation->param[i].meminfo->id, SK_TAPI_MEMORY_ID_LENGTH);
   }

   err = SYSV_IOCTL_1(context->device, SK_TAPI_INVOKE_COMMAND, &ic);

   if (err != 0)
   {
      SYSV_ERROR1(0, SYSV_LOG_CRITICAL_ERR, "Error calling IOCTL with command SK_TAPI_INVOKE_COMMAND [%d]", err);
      err = SK_ERROR_SEND_TO_DRIVER;
      goto DONE;
   }

DONE:
   SYSV_MEMSET(&ic, 0, sizeof(sk_teec_user_invoke_cmd));
   return err;
}

SKTEEC_STATUS SKTEEC_AllocateSharedMemory(SKTEEC_Context* context,
                                          SKTEEC_SharedMemory* memInfo)
{
   SKTEEC_STATUS                err;
   sk_teec_user_register_memory rm;

   if ((context == 0) || (memInfo == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   rm.context_handle = context->handle;
   rm.memory_size    = memInfo->memory_size;
   rm.flag           = SKTEEC_MEM_ALLOCATE;

   err = SYSV_IOCTL_1(context->device, SK_TAPI_REGISTER_MEMORY, &rm);

   if (err != 0)
   {
      SYSV_ERROR1(0, SYSV_LOG_CRITICAL_ERR, "Error calling IOCTL with command SK_TAPI_REGISTER_MEMORY [%d]", err);
      err = SK_ERROR_SEND_TO_DRIVER;
      goto DONE;
   }

   /*
      capture the memory ID
   */
   memInfo->buffer = (void *)rm.addr;
   SYSV_MEMCPY(memInfo->id, rm.memory_id, SK_TAPI_MEMORY_ID_LENGTH);

DONE:
   SYSV_MEMSET(&rm, 0, sizeof(sk_teec_user_register_memory));
   return err;
}

SKTEEC_STATUS SKTEEC_RegisterMemory(SKTEEC_Context* context,
                                    SKTEEC_SharedMemory* memInfo)
{
   SKTEEC_STATUS                err;
   sk_teec_user_register_memory rm;

   if ((context == 0) || (memInfo == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   rm.context_handle = context->handle;
   rm.memory_size    = memInfo->memory_size;
   rm.flag           = memInfo->flag;
   rm.addr           = (SK_ADDRESS_TYPE)memInfo->buffer;

   err = SYSV_IOCTL_1(context->device, SK_TAPI_REGISTER_MEMORY, &rm);

   if (err != 0)
   {
      SYSV_ERROR1(0, SYSV_LOG_CRITICAL_ERR, "Error calling IOCTL with command SK_TAPI_REGISTER_MEMORY [%d]", err);
      err = SK_ERROR_SEND_TO_DRIVER;
      goto DONE;
   }

   SYSV_ASSERT(rm.addr == (SK_ADDRESS_TYPE)memInfo->buffer);

   /*
      capture the memory ID
   */
   SYSV_MEMCPY(memInfo->id, rm.memory_id, SK_TAPI_MEMORY_ID_LENGTH);

DONE:
   SYSV_MEMSET(&rm, 0, sizeof(sk_teec_user_register_memory));
   return err;
}

SKTEEC_STATUS SKTEEC_UnregisterMemory(SKTEEC_Context* context,
                                      SKTEEC_SharedMemory* memInfo)
{
   SKTEEC_STATUS                  err;
   sk_teec_user_unregister_memory unrm;

   if ((context == 0) || (memInfo == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   unrm.context_handle = context->handle;
   SYSV_MEMCPY(unrm.memory_id, memInfo->id, SK_TAPI_MEMORY_ID_LENGTH);
   unrm.memory_size    = memInfo->memory_size;
   unrm.addr           = (SK_ADDRESS_TYPE)memInfo->buffer;

   err = SYSV_IOCTL_1(context->device, SK_TAPI_UNREGISTER_MEMORY, &unrm);

   if (err != 0)
   {
      SYSV_ERROR1(0, SYSV_LOG_CRITICAL_ERR, "Error calling IOCTL with command SK_TAPI_REGISTER_MEMORY [%d]", err);
      err = SK_ERROR_SEND_TO_DRIVER;
      goto DONE;
   }

DONE:
   SYSV_MEMSET(&unrm, 0, sizeof(sk_teec_user_unregister_memory));
   return err;
}







