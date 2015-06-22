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


#ifndef _SK_NWLD_SYSTEM_TYPES_H_
#define _SK_NWLD_SYSTEM_TYPES_H_

typedef int SKTEEC_STATUS;

typedef uint32_t SKTEEC_CONTEXT_HANDLE;
typedef uint32_t SKTEEC_SESSION_HANDLE;

typedef struct _sk_teec_user_open_context_
{
   uint32_t context_id;
   uint32_t sm_id;
   uint32_t context_handle;
}sk_teec_user_open_context;

typedef struct _sk_teec_user_close_context_
{
   SKTEEC_CONTEXT_HANDLE context_handle;
}sk_teec_user_close_context;

typedef struct _sk_teec_user_open_session_
{
   SKTEEC_CONTEXT_HANDLE context_handle;
   SK_TAID               taid;
   SKTEEC_SESSION_HANDLE session_handle;
}sk_teec_user_open_session;


typedef struct _sk_teec_user_close_session_
{
   SKTEEC_CONTEXT_HANDLE context_handle;
   SKTEEC_SESSION_HANDLE session_handle;
}sk_teec_user_close_session;

typedef struct _sk_teec_user_invoke_cmd_
{
   SKTEEC_CONTEXT_HANDLE context_handle;
   SKTEEC_SESSION_HANDLE session_handle;
   uint32_t              cmd;
   sk_operation          op;
}sk_teec_user_invoke_cmd;

typedef struct _sk_teec_user_register_memory_
{
   SKTEEC_CONTEXT_HANDLE context_handle;
   uint32_t              memory_size;
   uint32_t              flag;
   SK_ADDRESS_TYPE       addr;

   uint8_t memory_id[SK_TAPI_MEMORY_ID_LENGTH];
}sk_teec_user_register_memory;

typedef struct _sk_teec_user_unregister_memory_
{
   SKTEEC_CONTEXT_HANDLE context_handle;
   uint32_t              memory_size;
   uint32_t              flag;
   SK_ADDRESS_TYPE       addr;
   uint8_t               memory_id[SK_TAPI_MEMORY_ID_LENGTH];
}sk_teec_user_unregister_memory;

typedef struct _sk_teec_user_info_
{
   union
   {
      sk_teec_user_open_context      open_context;
      sk_teec_user_close_context     close_context;
      sk_teec_user_open_session      open_session;
      sk_teec_user_close_session     close_session;
      sk_teec_user_invoke_cmd        invoke_cmd;
      sk_teec_user_register_memory   reg_mem;
      sk_teec_user_unregister_memory unreg_mem;
   }type;
}sk_teec_user_info;

#endif
