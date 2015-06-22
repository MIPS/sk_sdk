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


#ifndef _SK_DRV_NWLD_
#define _SK_DRV_NWLD_

/**
  \defgroup TEECCOMM Normal World Platform Driver
  \addtogroup TEECCOMM
  @{
*/

typedef struct _sk_nwld_teec_comm_
{
   void     *my_obj;
   uint32_t context_id;
   uint32_t sm_id;
   uint8_t  reg_init;
}sk_nwld_teec_comm;


/**
* \details
* This function is called by the TEEC API to initialize a communication channel to the SWLD for the
* given context and machine ids.
*
* \param [in] obj         -- The comm object
* \param [in] context_id  -- The context ID the TEEC user is trying to connect to
* \param [in] sm_id       -- The machine ID the TEEC user is trying to connect to
*
* \return
*     - #SK_ERROR_SUCCESS on success
*/
SKTEEC_STATUS sk_nwld_platform_comm_register   (sk_nwld_teec_comm* obj, uint32_t context_id, uint32_t sm_id);

/**
* \details
* This closes the communication channel between NWLD and SWLD
*
* \param [in] obj -- The comm object
*
* \return
*     - #SK_ERROR_SUCCESS on success
*/
SKTEEC_STATUS sk_nwld_platform_comm_unregister (sk_nwld_teec_comm* obj);


/**
* \details
* Transmits a request from NWLD to the SWLD and then receives the response back from the SWLD.  This function is part of the
* platform specific code under src/platform/...
*
* \param [in] obj
* \param [in] send_buf
* \param [in] send_buf_len
* \param [out] recv_buf
* \param [in,out] recv_buf_len
* \param [out] op
*
* \return
*     - #SK_ERROR_SUCCESS on success
*/
SKTEEC_STATUS sk_nwld_platform_comm_proc  (sk_nwld_teec_comm* obj,
                                           uint8_t* send_buf,
                                           uint32_t send_buf_len,
                                           uint8_t* recv_buf,
                                           uint32_t* recv_buf_len,
                                           sk_operation* op);

SKTEEC_STATUS sk_nwld_platform_open_memory  (uint8_t* memory_id, void* log);
SKTEEC_STATUS sk_nwld_platform_close_memory (uint8_t* memory_id, void* log);


typedef struct _sk_nwld_teec_platform_
{
   void* obj;
}sk_nwld_teec_platform;


/**
* \details
* Platform specific initialization code called by the TEEC client.
*
* \param [in] obj     -- The platform container
* \param [in] param   -- A platform specific parameter to pass to the initialization function
*
* \return
*     - #SK_ERROR_SUCCESS on success
*/
SKTEEC_STATUS sk_nwld_platform_init (sk_nwld_teec_platform* obj, void* param);


/**
* \details
* Platform specific cleanup code called by the TEEC client.
*
* \param [in] obj     -- The platform container
*
* \return
*     - #SK_ERROR_SUCCESS on success
*/
SKTEEC_STATUS sk_nwld_platform_end  (sk_nwld_teec_platform* obj);

/** @} */

/*
   maximum buffer size
*/
#define SK_DRV_MAX_MSGBUFFER_SIZE sizeof(sk_msg)

typedef struct _tSkSysContextIDInfo_
{
   uint32_t          context_id;
   sk_nwld_teec_comm   comm;
   int32_t           context_id_cnt;
   uint8_t           used;
}tSkSysContextIDInfo;

typedef struct _tSkSysSession_
{
   uint8_t          session[SK_TAPI_SESSION_HANDLE_SIZE];
   uint8_t          used;
}tSkSysSession;

typedef struct _tSkSysConnection_
{
   uint32_t         context_id;
   uint32_t         context_id_index;
   uint8_t          context[SK_TAPI_CONTEXT_HANDLE_SIZE];
   tSkSysSession    *s;
   uint32_t         max_sessions;
   uint32_t         num_sessions;
   sk_nwld_teec_comm* comm;
   uint8_t          used;
}tSkSysConnection;

typedef struct _sk_nwld_tapi_
{
   tSkSysContextIDInfo info[SK_TAPI_MAX_CONTEXT_IDS];
   tSkSysConnection    *clist;
   uint32_t            num_context_id;
   uint32_t            max_connections;
   uint32_t            num_connections;
   uint32_t            sm_id;
   void                *log;
}sk_nwld_tapi;

SKTEEC_STATUS sk_nwld_tapi_init              (sk_nwld_tapi* tapi,
                                                  uint32_t max_connections,
                                                  uint32_t max_sessions,
                                                  uint32_t sm_id,
                                                  void* comm_obj); /* this is an array of MY_COMM objects equal to SK_TAPI_MAX_CONTEXT_IDS elements */
SKTEEC_STATUS sk_nwld_tapi_end               (sk_nwld_tapi* tapi);
SKTEEC_STATUS sk_nwld_tapi_open_context      (sk_nwld_tapi* tapi,
                                                  sk_teec_user_info* user_info,
                                                  uint8_t*  send_buffer,
                                                  uint32_t   send_buffer_size,
                                                  uint8_t*  recv_buffer,
                                                  uint32_t* recv_buffer_size);
SKTEEC_STATUS sk_nwld_tapi_close_context     (sk_nwld_tapi* tapi,
                                                  sk_teec_user_info* user_info,
                                                  uint8_t*  send_buffer,
                                                  uint32_t   send_buffer_size,
                                                  uint8_t*  recv_buffer,
                                                  uint32_t* recv_buffer_size);
SKTEEC_STATUS sk_nwld_tapi_open_session      (sk_nwld_tapi* tapi,
                                                  sk_teec_user_info* user_info,
                                                  uint8_t*  send_buffer,
                                                  uint32_t   send_buffer_size,
                                                  uint8_t*  recv_buffer,
                                                  uint32_t* recv_buffer_size);
SKTEEC_STATUS sk_nwld_tapi_close_session     (sk_nwld_tapi* tapi,
                                                  sk_teec_user_info* user_info,
                                                  uint8_t*  send_buffer,
                                                  uint32_t   send_buffer_size,
                                                  uint8_t*  recv_buffer,
                                                  uint32_t* recv_buffer_size);
SKTEEC_STATUS sk_nwld_tapi_invoke_command    (sk_nwld_tapi* tapi,
                                                  sk_teec_user_info* user_info,
                                                  uint8_t*  send_buffer,
                                                  uint32_t   send_buffer_size,
                                                  uint8_t*  recv_buffer,
                                                  uint32_t* recv_buffer_size);
SKTEEC_STATUS sk_nwld_tapi_register_memory   (sk_nwld_tapi* tapi,
                                                  sk_teec_user_info* user_info,
                                                  uint8_t*  send_buffer,
                                                  uint32_t   send_buffer_size,
                                                  uint8_t*  recv_buffer,
                                                  uint32_t* recv_buffer_size);
SKTEEC_STATUS sk_nwld_tapi_unregister_memory (sk_nwld_tapi* tapi,
                                                  sk_teec_user_info* user_info,
                                                  uint8_t*  send_buffer,
                                                  uint32_t   send_buffer_size,
                                                  uint8_t*  recv_buffer,
                                                  uint32_t* recv_buffer_size);
SKTEEC_STATUS sk_nwld_tapi_print_info        (sk_nwld_tapi* tapi);

#endif
