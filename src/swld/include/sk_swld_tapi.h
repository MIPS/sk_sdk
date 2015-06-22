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

#ifndef _TAPI_H_
#define _TAPI_H_

#include <sk_both.h>


#define TAPI_MAXIMUM_CONTEXT_CONNECTIONS             256
#define TAPI_MAXIMUM_SESSION_CONNECTIONS             256
#define TAPI_MAXIMUM_SESSION_CONNECTIONS_PER_CONTEXT 256

typedef struct _tApiRegMemory_
{
   uint8_t  id[SK_TAPI_MEMORY_ID_LENGTH];
   uint32_t flag;
   uint32_t memory_size;
   uint8_t* mem_ptr;
   void*   info;

   /*
      keep a link list of all the memories allocated by this context
   */
   void* next;
   void* prev;
}tApiRegMemory;

typedef struct _tApiRegMemoryTable_
{
   tApiRegMemory *m;
   int32_t       mem_region_count;
}tApiRegMemoryTable;


/*
  Session Table
*/
typedef struct _tApiSessionTable_
{
   /*
      operational
   */
   uint8_t active;

   /*
      session handle
   */
   uint8_t* session_handle;

   /*
      TAID
   */
   SK_TAID taid;

   /*
      index (for debug)
   */
   uint32_t idx;

   /*
      timestamp of last access
   */
   uint32_t time_access;

   /*
      TAPP instances
   */
   sk_swld_tapp_instance  tapp;
   void             *tapp_session;  /* provides an instance handle to the TApp */

   void* next;
   void* prev;
}tApiSessionTable;

/*
  Context Table
*/
typedef struct _tApiContextTable_
{
   /*
      operational
   */
   uint8_t active;

   /*
     store the information of the PID attached to this Context
   */
   pid_t pid;

   /*
      context handle
   */
   uint8_t*  context_handle;

   /*
     list of the tables
   */
   tApiSessionTable* start;

   /*
      number of ACTIVE connected sessions
   */
   uint32_t num_connected_sessions;

   /*
      timestamp of last access
   */
   uint32_t time_access;

}tApiContextTable;

/*
   Connection Table
   - use linear tables such as "cthandles" and "sthandles" to allow for clean abstraction of handle generator object
*/
typedef struct _tApiConnectionTable_
{
   /*
     store the Context Handles
   */
   tApiContextTable ctable[SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT];
   uint8_t          cthandles[(SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT*SK_TAPI_CONTEXT_HANDLE_SIZE)];
   uint32_t         num_ctable_elements;

   /*
     store the Session Handles
   */
   tApiSessionTable stable[SK_TAPI_MAX_SESSIONS_PER_CONTEXT];
   uint8_t          sthandles[(SK_TAPI_MAX_SESSIONS_PER_CONTEXT*SK_TAPI_SESSION_HANDLE_SIZE)];
   uint32_t         num_stable_elements;

}tApiConnectionTable;

typedef struct _sk_nwld_to_swld_msg_api_
{
   void* mObj;
   /**
   * \details
   * Receive an out-of-band message plus a sk_operation from the NWLD
   *
   * \param [in] obj
   * \param [in] buf
   * \param [in] buf_len
   * \param [in] op
   *
   * \return
   *     - #SK_ERROR_SUCCESS on success
   */
   int32_t (*tapi_recv_msg)(void* obj, uint8_t* buf, uint32_t* buf_len, sk_operation* op);

   /**
   * \details
   * Transmit an out-of-band message plus a sk_operation back to the NWLD
   *
   * \param [in] obj
   * \param [in] buf
   * \param [in] buf_len
   * \param [in] op
   *
   * \return
   *     - #SK_ERROR_SUCCESS on success
   */
   int32_t (*tapi_send_msg)(void* obj, uint8_t* buf, uint32_t buf_len, sk_operation* op);
}sk_nwld_to_swld_msg_api;

typedef struct _tApiHandlerPluginObj_
{
   void* instance;
   void* log;
}tApiHandlerPluginObj;

/**
  \defgroup TAPIAPI Secure World Trusted API
  \addtogroup TAPIAPI
  @{
*/

typedef struct _sk_swld_tapi_
{
   uint32_t                 context_id;
   uint32_t                 sm_id;
   sk_nwld_to_swld_msg_api*     msg;
   uint8_t*                 msend;
   uint32_t                 msend_size;
   uint32_t                 max_msend_size;
   uint8_t*                 mrecv;
   uint32_t                 mrecv_size;
   uint32_t                 max_mrecv_size;
   sk_operation             op;

   /*
     track the memory regions
   */
   tApiRegMemoryTable       rmem;

   tApiConnectionTable      conn;
   tApiHandlerPluginObj     hp;
   sk_swld_tapi_kernel_access ka;
   void                     *log;
}sk_swld_tapi;

/**
* \details
* Initialize a Trusted Server Context
*
* \param [in] tObjParam
* \param [in] context_id
* \param [in] sm_id
* \param [in] max_recv_mem_size
* \param [in] max_send_mem_size
* \param [in] msgInt
* \param [in] kaInt
* \param [in] hp_plugin
*
* \return
*     - #SK_ERROR_SUCCESS on success
*/
SK_STATUS sk_swld_tapi_init(
                         sk_swld_tapi*             tObjParam,
                         uint32_t                  context_id,
                         uint32_t                  sm_id,
                         uint32_t                  max_recv_mem_size,
                         uint32_t                  max_send_mem_size,
                         sk_nwld_to_swld_msg_api*      msgInt,             /* message interface definition provided by system */
                         sk_swld_tapi_kernel_access* kaInt,
                         void*                     hp_plugin);

/**
* \details
* Run the Trusted Server message loop (blocks thread)
*
* \param [in] tObjParam
*
* \return
*     - #SK_ERROR_SUCCESS on success
*/
SK_STATUS sk_swld_tapi_dispatch (sk_swld_tapi* tObjParam);

/**
* \details
* Free resources allocated by #sk_swld_tapi_init call
*
* \param [in] tObjParam
*
* \return
*     - #SK_ERROR_SUCCESS on success
*/
SK_STATUS sk_swld_tapi_close(sk_swld_tapi* tObj);

/**
  @}
*/

/*
  Interface definition for the SEND functions to the contexts
*/
typedef struct _tApiMsgSendInterface_
{
   void* mObj;
   int32_t (*tapi_send_msg_to_context)(void* obj, uint8_t* buf, uint32_t buf_len);
   int32_t (*tapi_recv_msg_from_context)(void* obj, uint8_t* buf, uint32_t* buf_len);
}tApiMsgSendInterface;

/*
  Interface definition for the RECV function from the RPC driver
*/
typedef struct _tApiMsgDriverRecvInterface_
{
   void* mObj;
   int32_t (*tapi_recv_msg)(void* obj, uint8_t* buf, uint32_t* buf_len);
   int32_t (*tapi_send_msg)(void* obj, uint8_t* buf, uint32_t buf_len);
}tApiMsgDriverRecvInterface;

typedef struct _tApiMsgIntObj_
{
   uint32_t                    num_context_ids;
   uint32_t                    context_ids[SK_TAPI_MAX_CONTEXT_IDS];
   tApiMsgSendInterface       *mi_send_context[SK_TAPI_MAX_CONTEXT_IDS];
   tApiMsgDriverRecvInterface *mi_rx_intf;
   uint32_t                    max_msg_size;
   uint8_t                    *msg;
   void                       *log;
}tApiMsgIntObj;


SK_STATUS sk_swld_tapi_handle_generator_plugin_init                    (tApiHandlerPluginObj* tObj, void* param);
SK_STATUS sk_swld_tapi_handle_generator_plugin_end                     (tApiHandlerPluginObj* tObj);
SK_STATUS sk_swld_tapi_handle_generator_plugin_generate_context_handle (tApiHandlerPluginObj* tObj, uint8_t* handle, uint32_t handle_size);
SK_STATUS sk_swld_tapi_handle_generator_plugin_find_context_handle     (tApiHandlerPluginObj* tObj, uint32_t table_size, uint8_t* table, uint8_t* context_handle,
                                                                uint32_t handle_size, uint32_t* idx);
SK_STATUS sk_swld_tapi_handle_generator_plugin_generate_session_handle (tApiHandlerPluginObj* tObj, uint8_t* handle, uint32_t handle_size);
SK_STATUS sk_swld_tapi_handle_generator_plugin_find_session_handle     (tApiHandlerPluginObj* tObj, uint32_t table_size, uint8_t* table, uint8_t* session_handle,
                                                                uint32_t handle_size, uint32_t* idx);

SK_STATUS sk_swld_tapi_message_interface_init   (tApiMsgIntObj*              tObjParam,
                                         uint32_t                    num_context_ids,
                                         uint32_t*                   context_ids,
                                         tApiMsgSendInterface*       msg_context_interface[SK_TAPI_MAX_CONTEXT_IDS],
                                         tApiMsgDriverRecvInterface* msg_rx_interface,
                                         uint32_t                    max_msg_size);
SK_STATUS sk_swld_tapi_message_interface_end    (tApiMsgIntObj* tObjParam);
SK_STATUS sk_swld_tapi_message_interface_process(tApiMsgIntObj* tObjParam);


#endif /*_TAPI_H_*/
