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


#ifndef _TAPI_CLIENT_SHARED_TYPES_
#define _TAPI_CLIENT_SHARED_TYPES_

#define SK_TAPI_MAX_CONTEXT_IDS  4

#if defined(__clang__) || defined(__GNUC__)
#define WARN_RETURN_TYPE __attribute__ ((warn_unused_result))
#else
#define WARN_RETURN_TYPE
#endif

/*
  TAPI Defines
*/
#define SK_TAPI_OPEN_CONTEXT      0
#define SK_TAPI_CLOSE_CONTEXT     1
#define SK_TAPI_OPEN_SESSION      2
#define SK_TAPI_CLOSE_SESSION     3
#define SK_TAPI_INVOKE_COMMAND    4
#define SK_TAPI_REGISTER_MEMORY   5
#define SK_TAPI_UNREGISTER_MEMORY 6
#define SK_TAPI_CHECK             7
#define SK_TAPI_SYSTEM            8

/*
  TAPI Size Requirements
*/
#define SK_TAPI_CONTEXT_HANDLE_SIZE 16
#define SK_TAPI_SESSION_HANDLE_SIZE 16
#define SK_TAPI_SESSION_TAID_SIZE   16
typedef uint8_t  SK_TAID[SK_TAPI_SESSION_TAID_SIZE];

/*
  Connection Requirements
*/
#define SK_TAPI_MAX_CONNECTIONS             128
#define SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT 128
#define SK_TAPI_MAX_SESSIONS_PER_CONTEXT    128

/*
  SK Message Request and Response Type Definition
*/
#define SK_TAPI_MSG_REQ_TYPE  0
#define SK_TAPI_MSG_RESP_TYPE 1

/*
   Memory Information
*/
#define SK_TAPI_MEMORY_ID_LENGTH 16


/*
   System Address Type
*/
#define SK_ADDRESS_TYPE uint32_t


/*
  Command type used in the message
*/
typedef uint32_t sk_tapi_cmd;

/*
  TAPI pack errors
*/
#define SKTAPIPACKERR_SUCCESS         0
#define SKTAPIPACKERR_FAILED         -1
#define SKTAPIPACKERR_INVALID_PARAMS -2

/*
   Memory Registrations Flags
*/
#define SKTEEC_MEMREF_IN    0x00
#define SKTEEC_MEMREF_OUT   0x01
#define SKTEEC_MEMREF_INOUT 0x02
#define SKTEEC_MEM_ALLOCATE 0x03 /* only used when allocating memory */

/*
   TAPI operation structure
*/
#define SK_MAXIMUM_OPERATIONS 4
typedef struct _sk_op_meminfo_
{
   uint32_t        memory_size;
   uint32_t        flag;
   SK_ADDRESS_TYPE addr;
   uint8_t         id[SK_TAPI_MEMORY_ID_LENGTH];
}sk_op_meminfo;

typedef struct _sk_op_param_
{
  sk_op_meminfo mem;
}sk_op_param;

typedef struct _sk_operation_
{
   uint32_t    num_param;
   sk_op_param param[SK_MAXIMUM_OPERATIONS];
}sk_operation;

/*
  open context message
*/
typedef struct _sk_open_context_req_
{
   uint32_t context_id;
   pid_t pid;
}sk_open_context_req;

typedef struct _sk_open_context_resp_
{
   uint8_t  context_handle[SK_TAPI_CONTEXT_HANDLE_SIZE];
   int32_t  status;
}sk_open_context_resp;

typedef union _sk_open_context_
{
   sk_open_context_req  req;
   sk_open_context_resp resp;
}sk_open_context;


/*
  close context message
*/
typedef struct _sk_close_context_req_
{
   uint8_t  context_handle[SK_TAPI_CONTEXT_HANDLE_SIZE];
}sk_close_context_req;

typedef struct _sk_close_context_resp_
{
   int32_t  status;
}sk_close_context_resp;

typedef union _sk_close_context_
{
   sk_close_context_req  req;
   sk_close_context_resp resp;
}sk_close_context;

/*
  open session message
*/
typedef struct _sk_open_session_req_
{
   uint8_t  context_handle[SK_TAPI_CONTEXT_HANDLE_SIZE];
   SK_TAID  taid;
}sk_open_session_req;

typedef struct _sk_open_session_resp_
{
   uint8_t  session_handle[SK_TAPI_SESSION_HANDLE_SIZE];
   int32_t  status;
}sk_open_session_resp;

typedef union _sk_open_session_
{
   sk_open_session_req  req;
   sk_open_session_resp resp;
}sk_open_session;

/*
  close session message
*/
typedef struct _sk_close_session_req_
{
   uint8_t  context_handle[SK_TAPI_CONTEXT_HANDLE_SIZE];
   uint8_t  session_handle[SK_TAPI_SESSION_HANDLE_SIZE];
}sk_close_session_req;

typedef struct _sk_close_session_resp_
{
   int32_t  status;
}sk_close_session_resp;

typedef union _sk_close_session_
{
   sk_close_session_req  req;
   sk_close_session_resp resp;
}sk_close_session;


/*
  invoke command message
*/
typedef struct _sk_invoke_cmd_req_
{
   uint8_t      context_handle[SK_TAPI_CONTEXT_HANDLE_SIZE];
   uint8_t      session_handle[SK_TAPI_SESSION_HANDLE_SIZE];
   uint32_t     cmd;
}sk_invoke_cmd_req;

typedef struct _sk_invoke_cmd_resp_
{
   int32_t  status;
}sk_invoke_cmd_resp;

typedef union _sk_invoke_cmd_
{
   sk_invoke_cmd_req  req;
   sk_invoke_cmd_resp resp;
}sk_invoke_cmd;

/*
  memory setup
*/
typedef struct _sk_register_memory_req_
{
   uint8_t         context_handle[SK_TAPI_CONTEXT_HANDLE_SIZE];
   uint32_t        memory_size;
   uint32_t        flag;
   SK_ADDRESS_TYPE addr;
}sk_register_memory_req;

typedef struct _sk_register_memory_resp_
{
   SK_ADDRESS_TYPE addr;
   uint8_t         memory_id[SK_TAPI_MEMORY_ID_LENGTH];
   int32_t         status;
}sk_register_memory_resp;

typedef union _sk_register_memory_
{
   sk_register_memory_req  req;
   sk_register_memory_resp resp;
}sk_register_memory;

typedef struct _sk_unregister_memory_req_
{
   uint8_t  context_handle[SK_TAPI_CONTEXT_HANDLE_SIZE];
   uint8_t  memory_id[SK_TAPI_MEMORY_ID_LENGTH];
}sk_unregister_memory_req;

typedef struct _sk_unregister_memory_resp_
{
   int32_t  status;
}sk_unregister_memory_resp;

typedef union _sk_unregister_memory_
{
   sk_unregister_memory_req  req;
   sk_unregister_memory_resp resp;
}sk_unregister_memory;

typedef struct _sk_check_req_
{
}sk_check_req;

typedef struct _sk_check_resp_
{
   int32_t  status;
}sk_check_resp;

typedef union _sk_check_
{
   sk_check_req  req;
   sk_check_resp resp;
}sk_check;

typedef struct _sk_tapi_resp_
{
   int32_t  status;
}sk_tapi_resp;

typedef union _sk_tapi_
{
   sk_tapi_resp resp;
}sk_tapi;

/* used a reference to the context id being applied */
typedef struct _sk_header_
{
   uint32_t    context_id;
   uint32_t    sm_id;
   sk_tapi_cmd cmd;
}sk_header;

typedef struct _sk_msg_
{
   sk_header hdr;
   union
   {
      sk_open_context      open_context;
      sk_close_context     close_context;
      sk_open_session      open_session;
      sk_close_session     close_session;
      sk_invoke_cmd        invoke_cmd;
      sk_register_memory   register_memory;
      sk_unregister_memory unregister_memory;
      sk_check             sys_check;
      sk_tapi              sys_tapi;
   }type;
}sk_msg;

/*
  Macros
*/

/*
  Functions
*/
STATUS sk_tapi_header_unpack            (const uint8_t* pack, uint32_t pack_len, sk_header* hdr) WARN_RETURN_TYPE;
STATUS sk_tapi_sys_tapi_pack            (const sk_msg* msg,   uint8_t* pack,     uint32_t* pack_len, uint8_t type) WARN_RETURN_TYPE;
STATUS sk_tapi_sys_tapi_unpack          (const uint8_t* pack, uint32_t pack_len, sk_msg* msg,        uint8_t type) WARN_RETURN_TYPE;
STATUS sk_tapi_open_context_pack        (const sk_msg* msg,   uint8_t* pack,     uint32_t* pack_len, uint8_t type) WARN_RETURN_TYPE;
STATUS sk_tapi_open_context_unpack      (const uint8_t* pack, uint32_t pack_len, sk_msg* msg,        uint8_t type) WARN_RETURN_TYPE;
STATUS sk_tapi_close_context_pack       (const sk_msg* msg,   uint8_t* pack,     uint32_t* pack_len, uint8_t type) WARN_RETURN_TYPE;
STATUS sk_tapi_close_context_unpack     (const uint8_t* pack, uint32_t pack_len, sk_msg* msg,        uint8_t type) WARN_RETURN_TYPE;
STATUS sk_tapi_open_session_pack        (const sk_msg* msg,   uint8_t* pack,     uint32_t* pack_len, uint8_t type) WARN_RETURN_TYPE;
STATUS sk_tapi_open_session_unpack      (const uint8_t* pack, uint32_t pack_len, sk_msg* msg,        uint8_t type) WARN_RETURN_TYPE;
STATUS sk_tapi_close_session_pack       (const sk_msg* msg,   uint8_t* pack,     uint32_t* pack_len, uint8_t type) WARN_RETURN_TYPE;
STATUS sk_tapi_close_session_unpack     (const uint8_t* pack, uint32_t pack_len, sk_msg* msg,        uint8_t type) WARN_RETURN_TYPE;
STATUS sk_tapi_invoke_command_pack      (const sk_msg* msg,   uint8_t* pack,     uint32_t* pack_len, uint8_t type) WARN_RETURN_TYPE;
STATUS sk_tapi_invoke_command_unpack    (const uint8_t* pack, uint32_t pack_len, sk_msg* msg,        uint8_t type) WARN_RETURN_TYPE;
STATUS sk_tapi_register_memory_pack     (const sk_msg* msg,   uint8_t* pack,     uint32_t* pack_len, uint8_t type) WARN_RETURN_TYPE;
STATUS sk_tapi_register_memory_unpack   (const uint8_t* pack, uint32_t pack_len, sk_msg* msg,        uint8_t type) WARN_RETURN_TYPE;
STATUS sk_tapi_unregister_memory_pack   (const sk_msg* msg,   uint8_t* pack,     uint32_t* pack_len, uint8_t type) WARN_RETURN_TYPE;
STATUS sk_tapi_unregister_memory_unpack (const uint8_t* pack, uint32_t pack_len, sk_msg* msg,        uint8_t type) WARN_RETURN_TYPE;
STATUS sk_tapi_check_pack               (const sk_msg* msg,   uint8_t* pack,     uint32_t* pack_len, uint8_t type) WARN_RETURN_TYPE;
STATUS sk_tapi_check_unpack             (const uint8_t* pack, uint32_t pack_len, sk_msg* msg,        uint8_t type) WARN_RETURN_TYPE;


#endif /* _TAPI_CLIENT_SHARED_TYPES_ */







