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

/*
  Description:
  This function initializes the RPC connection for the CONTEXT and SECURE_MACHINE ID. Typically, if the SWld supports 2 or more CONTEXTS, then
  you call this function 2 more times for each CONTEXT instance.

*/

#define SK_DRV_CONTEXT_ID_DOES_NOT_EXISTS -1
#define SK_DRV_NO_HANDLE_CONNECTIONS      -1
#define SK_DRV_NO_HANDLE_SESSIONS         -1

static int32_t sk_nwld_tapi_prv_find_context_id (sk_nwld_tapi* tapi,   uint32_t context_id);
static int32_t sk_nwld_tapi_get_conn_handle     (tSkSysConnection* table, uint32_t max_table_size);
static int32_t sk_nwld_tapi_get_session_handle  (tSkSysSession* sess,     uint32_t max_sess_size);

/*
  private functions
*/

static int32_t sk_nwld_tapi_prv_find_context_id(sk_nwld_tapi* tapi, uint32_t context_id)
{
   uint32_t i;

   SYSV_ASSERT(tapi       != 0);
   SYSV_ASSERT(context_id != 0);

   for(i=0; i<SK_TAPI_MAX_CONTEXT_IDS; i++)
   {
      if ((tapi->info[i].used == 1) && (tapi->info[i].context_id == context_id))
      {
         return i;
      }
   }

   return SK_DRV_CONTEXT_ID_DOES_NOT_EXISTS; /* indicates that nothing was found */
}

static int32_t sk_nwld_tapi_get_conn_handle(tSkSysConnection* table, uint32_t max_table_size)
{
   uint32_t i;

   SYSV_ASSERT(table          != 0);
   SYSV_ASSERT(max_table_size != 0);

   for(i=0; i<max_table_size; i++)
   {
      if (table[i].used == 0)
      {
         return i;
      }
   }

   return SK_DRV_NO_HANDLE_CONNECTIONS; /* indicates that nothing was found */
}

static int32_t sk_nwld_tapi_get_session_handle(tSkSysSession* sess, uint32_t max_sess_size)
{
   uint32_t i;

   SYSV_ASSERT(sess          != 0);
   SYSV_ASSERT(max_sess_size != 0);

   for(i=0; i<max_sess_size; i++)
   {
      if (sess[i].used == 0)
      {
         return i;
      }
   }

   return SK_DRV_NO_HANDLE_SESSIONS; /* indicates that nothing was found */
}


/*
  Public Functions
*/

SKTEEC_STATUS sk_nwld_tapi_init(sk_nwld_tapi* tapi,
                                      uint32_t max_connections,
                                      uint32_t max_sessions,
                                      uint32_t sm_id,
                                      void* comm_obj)
{
   SKTEEC_STATUS err;
   uint32_t      i;

   if ((tapi == 0) || (max_connections == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   err = SK_ERROR_SUCCESS;

   SYSV_MEMSET(tapi, 0, sizeof(sk_nwld_tapi));

   if ((tapi->clist = (tSkSysConnection *)SYSV_MALLOC(sizeof(tSkSysConnection) * max_connections)) == 0)
   {
      SYSV_ERROR0(tapi->log, SYSV_LOG_CRITICAL_ERR, "No memory available (Connections)");
      err = SK_ERROR_NO_MEMORY;
      goto DONE;
   }

   /*
      initialize
   */
   SYSV_MEMSET(tapi->clist,    0, (sizeof(tSkSysConnection) * max_connections));

   for(i = 0; i < SK_TAPI_MAX_CONTEXT_IDS; i++)
   {
      tapi->info[i].comm.my_obj = comm_obj;
      comm_obj++;
   }

   if ((tapi->clist->s = (tSkSysSession *)SYSV_MALLOC(sizeof(tSkSysSession) * max_sessions)) == 0)
   {
      SYSV_ERROR0(tapi->log, SYSV_LOG_CRITICAL_ERR, "No memory available (Sessions)");
      err = SK_ERROR_OPEN_SESSION;
      goto DONE;
   }

   /*
      initialize
   */
   SYSV_MEMSET(tapi->clist->s, 0, (sizeof(tSkSysSession) * max_sessions));

   /*
      Set the maximum sessions
   */
   tapi->clist->max_sessions = max_sessions;

   /*
      Set the maximum connection points
   */
   tapi->max_connections = max_connections;

   /*
      Set the security machine ID
   */
   tapi->sm_id = sm_id;

   SYSV_INFO0(tapi->log, "initialize ...");
   SYSV_INFO1(tapi->log, "MAX SESSIONS = %d", tapi->clist->max_sessions);
   SYSV_INFO1(tapi->log, "MAX CONNECTIONS = %d", tapi->max_connections);
   SYSV_INFO1(tapi->log, "SM ID = 0x%.4x", tapi->sm_id);

DONE:
   return err;
}

SKTEEC_STATUS sk_nwld_tapi_end(sk_nwld_tapi* tapi)
{
   SKTEEC_STATUS err;

   if (tapi == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   err = SK_ERROR_SUCCESS;

   SYSV_MEMSET(tapi->clist->s, 0, (sizeof(tSkSysSession) * tapi->clist->max_sessions));

   SYSV_FREE(tapi->clist->s);

   SYSV_MEMSET(tapi->clist, 0, (sizeof(tSkSysConnection) * tapi->max_connections));

   SYSV_FREE(tapi->clist);

   SYSV_MEMSET(tapi, 0, sizeof(sk_nwld_tapi));

   return err;
}


SKTEEC_STATUS sk_nwld_tapi_open_context(sk_nwld_tapi* tapi,
                                            sk_teec_user_info* user_info,
                                            uint8_t*  send_buffer,
                                            uint32_t  send_buffer_size,
                                            uint8_t*  recv_buffer,
                                            uint32_t* recv_buffer_size)
{
   SKTEEC_STATUS                err;
   sk_msg                       msg;
   int32_t                      chndle;
   sk_teec_user_open_context*   poc;
   uint32_t                     i;
   int32_t                      curr_context_id;

   if ((tapi == 0)             ||
       (user_info == 0)        ||
       (send_buffer == 0)      ||
       (recv_buffer == 0)      ||
       (send_buffer_size == 0) ||
       (recv_buffer_size == 0) ||
       ((*recv_buffer_size) == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   poc = &user_info->type.open_context;

   if (poc->context_id == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   /*
     find the context ID, if it exists
   */
   curr_context_id = sk_nwld_tapi_prv_find_context_id(tapi, poc->context_id);

   if (curr_context_id == SK_DRV_CONTEXT_ID_DOES_NOT_EXISTS)
   {
      if (tapi->num_context_id < SK_TAPI_MAX_CONTEXT_IDS)
      {
         /*
            Add the new Context ID
         */
         for(i=0; i<SK_TAPI_MAX_CONTEXT_IDS; i++)
         {
            if (tapi->info[i].used == 0)
            {
               curr_context_id = i;
               break;
            }
         }

         SYSV_ASSERT(curr_context_id != SK_DRV_CONTEXT_ID_DOES_NOT_EXISTS);

         tapi->info[curr_context_id].context_id       = poc->context_id;
         tapi->info[curr_context_id].used             = 1;
         tapi->info[curr_context_id].context_id_cnt++;

         SYSV_ASSERT(tapi->num_context_id != SK_TAPI_MAX_CONTEXT_IDS);
         tapi->num_context_id++;

         SYSV_INFO0(tapi->log, "adding ...");
         SYSV_INFO2(tapi->log, "tapi->info[%d].context_id     = 0x%.4x", curr_context_id, tapi->info[curr_context_id].context_id);
         SYSV_INFO2(tapi->log, "tapi->info[%d].used           = %d", curr_context_id, tapi->info[curr_context_id].used);
         SYSV_INFO2(tapi->log, "tapi->info[%d].context_id_cnt = %d", curr_context_id, tapi->info[curr_context_id].context_id_cnt);
         SYSV_INFO1(tapi->log, "tapi->num_context_id          = %d", tapi->num_context_id);
      }
      else
      {
         SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Exceeded maximum Context Count [%d]", tapi->num_context_id);
         err = SK_ERROR_OPEN_CONTEXT;
         goto DONE;
      }

      /*
         Initialize the communication driver to support a (CONTEXT_ID, SM_ID) connection. There is only
         one instance of the COMM device per (CONTEXT_ID, SM_ID) connection.
      */
      if ((err = sk_nwld_platform_comm_register(&tapi->info[curr_context_id].comm,
                                             tapi->info[curr_context_id].context_id,
                                             tapi->sm_id)) != SK_ERROR_SUCCESS)
      {
         SYSV_ERROR3(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: opening Client Comm: Context_id=0x%.4x sm_id=0x%.4x err=%d",
                     tapi->info[curr_context_id].context_id, tapi->sm_id, err);
         err = SK_ERROR_OPEN_FAILED;
         goto DONE;
      }
      else
      {
         SYSV_INFO2(tapi->log, "Opening Context ID = 0x%.4x / 0x%.4x",
                    tapi->info[curr_context_id].context_id, tapi->sm_id);
      }
   }
   else
   {
      SYSV_INFO2(tapi->log, "Context already open: Context ID = 0x%.4x / 0x%.4x",
                 tapi->info[curr_context_id].context_id, tapi->sm_id);

      /*
         increment the context ID count
      */
      tapi->info[curr_context_id].context_id_cnt++;
   }


   /*
      Setup the message
   */
   msg.hdr.context_id                   = tapi->info[curr_context_id].context_id;
   msg.hdr.sm_id                        = tapi->sm_id;
   msg.hdr.cmd                          = SK_TAPI_OPEN_CONTEXT;
   msg.type.open_context.req.context_id = tapi->info[curr_context_id].context_id;


   /*
       Using the communication (CONTEXT_ID, SM_ID) connection, call an open CONTEXT command. You can call
       this multiple times for the same CONTEXT_ID. Each call will return a unique CONTEXT handle.
   */

   /*
      Byte pack the information back into a send message buffer
   */
   if ((err = sk_tapi_open_context_pack(&msg, send_buffer, &send_buffer_size, SK_TAPI_MSG_REQ_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }

   if ((err = sk_nwld_platform_comm_proc(&tapi->info[curr_context_id].comm,
                                    send_buffer,
                                    send_buffer_size,
                                    recv_buffer,
                                    recv_buffer_size,
                                    0)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR3(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: process Client Comm: Context_id=0x%.4x sm_id=0x%.4x err=%d",
                     tapi->info[curr_context_id].context_id, tapi->sm_id, err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }

   /*
      Byte unpack the information back into the receiving structure
   */
   if ((err = sk_tapi_open_context_unpack(recv_buffer, (*recv_buffer_size), &msg, SK_TAPI_MSG_RESP_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }


   /*
     Check Message Header
   */
   if (msg.hdr.cmd == SK_TAPI_SYSTEM)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: internal TAPI error [%d]", msg.type.sys_tapi.resp.status);
      err = SK_ERROR_FAILED;
      goto DONE;
   }
   else if (msg.hdr.cmd != SK_TAPI_OPEN_CONTEXT)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: wrong response command [%d]", msg.hdr.cmd);
      err = SK_ERROR_INVALID_CMD;
      goto DONE;
   }

   /*
      check and make sure the results pass
   */
   if (msg.type.open_context.resp.status != SKTAPIPACKERR_SUCCESS)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: context open failed [%d]", msg.type.open_context.resp.status);
      err = SK_ERROR_FAILED;
      goto DONE;
   }

   /*
     Find a connection handle
   */
   chndle = sk_nwld_tapi_get_conn_handle(tapi->clist, tapi->max_connections);

   if (chndle == SK_DRV_NO_HANDLE_CONNECTIONS)
   {
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }

   tapi->clist[chndle].context_id       = tapi->info[curr_context_id].context_id;
   tapi->clist[chndle].context_id_index = curr_context_id;
   tapi->clist[chndle].used             = 1;
   tapi->clist[chndle].comm             = &tapi->info[curr_context_id].comm;
   SYSV_MEMCPY(tapi->clist[chndle].context, msg.type.open_context.resp.context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
   tapi->num_connections++;

   /*
     return the results
   */
   poc->context_handle = chndle;

   SYSV_INFO0(tapi->log, "--- Open Context Debug Information ---");
   SYSV_INFO2(tapi->log, "tapi->clist[%d].context_id = %d", chndle, tapi->clist[chndle].context_id);
   SYSV_INFO2(tapi->log, "tapi->clist[%d].context_id_index = %d", chndle, tapi->clist[chndle].context_id_index);
   SYSV_INFO1(tapi->log, "poc->context_handle = %d", poc->context_handle);
   SYSV_LDUMP(tapi->log, "tapi:context handle", tapi->clist[chndle].context, SK_TAPI_CONTEXT_HANDLE_SIZE);

DONE:
   /* wipe */
   SYSV_MEMSET(&msg, 0, sizeof(sk_msg));
   return err;
}

SKTEEC_STATUS sk_nwld_tapi_close_context(sk_nwld_tapi* tapi,
                                             sk_teec_user_info* user_info,
                                             uint8_t* send_buffer,
                                             uint32_t  send_buffer_size,
                                             uint8_t* recv_buffer,
                                             uint32_t* recv_buffer_size)
{
   SKTEEC_STATUS                err;
   uint32_t                     chndle;
   sk_msg                       msg;
   sk_teec_user_close_context*  pcc;
   tSkSysConnection*            pconn;

   if ((tapi == 0)             ||
       (user_info == 0)        ||
       (send_buffer == 0)      ||
       (recv_buffer == 0)      ||
       (send_buffer_size == 0) ||
       (recv_buffer_size == 0) ||
       ((*recv_buffer_size) == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   pcc = &user_info->type.close_context;

   /*
     find the context ID, if it exists
   */

   /*
     Get the connection instance from the context handle
   */
   chndle = pcc->context_handle;
   if ((chndle >= tapi->max_connections) || (tapi->clist[chndle].used == 0))
   {
      SYSV_ERROR2(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: invalid context handle [%d/%d]", chndle, tapi->max_connections);
      err = SK_ERROR_CONTEXT_HANDLE_NOT_FOUND;
      goto DONE;
   }

   pconn = &tapi->clist[chndle];

   /*
      Setup the message
   */
   msg.hdr.context_id  = pconn->context_id;
   msg.hdr.sm_id       = tapi->sm_id;
   msg.hdr.cmd         = SK_TAPI_CLOSE_CONTEXT;
   SYSV_MEMCPY(msg.type.close_context.req.context_handle, pconn->context, SK_TAPI_CONTEXT_HANDLE_SIZE);

   /*
     Byte pack the information back into a send message buffer
   */
   if ((err = sk_tapi_close_context_pack(&msg, send_buffer, &send_buffer_size, SK_TAPI_MSG_REQ_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }


   if ((err = sk_nwld_platform_comm_proc(pconn->comm,
                                    send_buffer,
                                    send_buffer_size,
                                    recv_buffer,
                                    recv_buffer_size,
                                    0)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR3(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: process Client Comm: Context_id=0x%.4x sm_id=0x%.4x err=%d",
                     pconn->context_id, tapi->sm_id, err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }

   /*
      Byte unpack the information back into the receiving structure
   */
   if ((err = sk_tapi_close_context_unpack(recv_buffer, (*recv_buffer_size), &msg, SK_TAPI_MSG_RESP_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }


   /*
     Check Message Header
   */
   if (msg.hdr.cmd == SK_TAPI_SYSTEM)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: internal TAPI error [%d]", msg.type.sys_tapi.resp.status);
      err = SK_ERROR_FAILED;
      goto DONE;
   }
   else if (msg.hdr.cmd != SK_TAPI_CLOSE_CONTEXT)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: wrong response command [%d]", msg.hdr.cmd);
      err = SK_ERROR_INVALID_CMD;
      goto DONE;
   }

   /*
      check and make sure the results pass
   */
   if (msg.type.close_context.resp.status != SKTAPIPACKERR_SUCCESS)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: context close failed [%d]", msg.type.close_context.resp.status);
      err = SK_ERROR_FAILED;
      goto DONE;
   }

    /*
      decrement the context_id count
   */
   SYSV_ASSERT(tapi->info[pconn->context_id_index].context_id_cnt > 0);
   tapi->info[pconn->context_id_index].context_id_cnt--;

   if (tapi->info[pconn->context_id_index].context_id_cnt == 0)
   {
      /*
        no more context ID's related this so shut everything down
      */

      if ((err = sk_nwld_platform_comm_unregister(pconn->comm)) != SK_ERROR_SUCCESS)
      {
         SYSV_ERROR3(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: closing Client Comm: Context_id=0x%.4x sm_id=0x%.4x kerr=%d\n",
                     pconn->context_id, tapi->sm_id, err);
         err = SK_ERROR_COMM_FAILED;
         goto DONE;
      }

      tapi->info[pconn->context_id_index].context_id = 0;
      tapi->info[pconn->context_id_index].used       = 0;

      SYSV_ASSERT(tapi->num_context_id > 0);

      tapi->num_context_id--;
   }

   /*
     decrement the table count
   */
   SYSV_ASSERT(tapi->num_connections != 0);
   tapi->num_connections--;

   /*
     zero out
   */
   SYSV_MEMSET(pconn, 0, sizeof(tSkSysConnection));

DONE:

   /* wipe */
   SYSV_MEMSET(&msg, 0, sizeof(sk_msg));
   return err;
}


SKTEEC_STATUS sk_nwld_tapi_open_session(sk_nwld_tapi* tapi,
                                            sk_teec_user_info* user_info,
                                            uint8_t* send_buffer,
                                            uint32_t  send_buffer_size,
                                            uint8_t* recv_buffer,
                                            uint32_t* recv_buffer_size)
{
   SKTEEC_STATUS                err;
   uint32_t                     chndle;
   int32_t                     shndle;
   sk_msg                       msg;
   sk_teec_user_open_session*   pos;
   tSkSysConnection*            pconn;

   if ((tapi == 0)             ||
       (user_info == 0)        ||
       (send_buffer == 0)      ||
       (recv_buffer == 0)      ||
       (send_buffer_size == 0) ||
       (recv_buffer_size == 0) ||
       ((*recv_buffer_size) == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   pos = &user_info->type.open_session;

   /*
     Get the connection instance from the context handle
   */
   chndle = pos->context_handle;
   if ((chndle >= tapi->max_connections) || (tapi->clist[chndle].used == 0))
   {
      SYSV_ERROR2(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: invalid context handle [%d/%d]", chndle, tapi->max_connections);
      err = SK_ERROR_CONTEXT_HANDLE_NOT_FOUND;
      goto DONE;
   }

   pconn = &tapi->clist[chndle];

   /*
      Setup the message
   */
   msg.hdr.context_id  = pconn->context_id;
   msg.hdr.sm_id       = tapi->sm_id;
   msg.hdr.cmd         = SK_TAPI_OPEN_SESSION;
   SYSV_MEMCPY(msg.type.open_session.req.context_handle, pconn->context, SK_TAPI_CONTEXT_HANDLE_SIZE);
   SYSV_MEMCPY(msg.type.open_session.req.taid,           pos->taid,      SK_TAPI_SESSION_TAID_SIZE);


   /*
     Byte pack the information back into a send message buffer
   */
   if ((err = sk_tapi_open_session_pack(&msg, send_buffer, &send_buffer_size, SK_TAPI_MSG_REQ_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }


   if ((err = sk_nwld_platform_comm_proc(pconn->comm,
                                    send_buffer,
                                    send_buffer_size,
                                    recv_buffer,
                                    recv_buffer_size,
                                    0)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR3(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: process Client Comm: Context_id=0x%.4x sm_id=0x%.4x err=%d",
                     pconn->context_id, tapi->sm_id, err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }

   /*
      Byte unpack the information back into the receiving structure
   */
   if ((err = sk_tapi_open_session_unpack(recv_buffer, (*recv_buffer_size), &msg, SK_TAPI_MSG_RESP_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }

   /*
     Check Message Header
   */
   if (msg.hdr.cmd == SK_TAPI_SYSTEM)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: internal TAPI error [%d]", msg.type.sys_tapi.resp.status);
      err = SK_ERROR_FAILED;
      goto DONE;
   }
   else if (msg.hdr.cmd != SK_TAPI_OPEN_SESSION)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: wrong response command [%d]", msg.hdr.cmd);
      err = SK_ERROR_INVALID_CMD;
      goto DONE;
   }

   /*
      check and make sure the results pass
   */
   if (msg.type.open_session.resp.status != SKTAPIPACKERR_SUCCESS)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: open session failed [%d]", msg.type.open_session.resp.status);
      err = SK_ERROR_FAILED;
      goto DONE;
   }

   /*
     find the next available session instance
   */
   shndle = sk_nwld_tapi_get_session_handle(pconn->s, pconn->max_sessions);

   if (shndle == SK_DRV_NO_HANDLE_SESSIONS)
   {
      SYSV_ERROR0(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: finding new session handle");
      err = SK_ERROR_SESSION_HANDLE_NOT_FOUND;
      goto DONE;
   }

   /*
     set the session information
   */
   pconn->s[shndle].used = 1;

   SYSV_MEMCPY(pconn->s[shndle].session, msg.type.open_session.resp.session_handle, SK_TAPI_SESSION_HANDLE_SIZE);

   pconn->num_sessions++;

   /*
     set the return user values
   */
   pos->session_handle = shndle;

DONE:

   /* wipe */
   SYSV_MEMSET(&msg, 0, sizeof(sk_msg));
   return err;
}

SKTEEC_STATUS sk_nwld_tapi_close_session(sk_nwld_tapi* tapi,
                                             sk_teec_user_info* user_info,
                                             uint8_t* send_buffer,
                                             uint32_t  send_buffer_size,
                                             uint8_t* recv_buffer,
                                             uint32_t* recv_buffer_size)
{
   SKTEEC_STATUS                err;
   uint32_t                     chndle;
   uint32_t                     shndle;
   sk_msg                       msg;
   sk_teec_user_close_session*  pcs;
   tSkSysConnection*            pconn;

   if ((tapi == 0)             ||
       (user_info == 0)        ||
       (send_buffer == 0)      ||
       (recv_buffer == 0)      ||
       (send_buffer_size == 0) ||
       (recv_buffer_size == 0) ||
       ((*recv_buffer_size) == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   pcs = &user_info->type.close_session;

   /*
     Get the connection instance from the context handle
   */
   chndle = pcs->context_handle;
   if ((chndle >= tapi->max_connections) || (tapi->clist[chndle].used == 0))
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: invalid context handle [%d]", pcs->context_handle);
      err = SK_ERROR_CONTEXT_HANDLE_NOT_FOUND;
      goto DONE;
   }

   pconn = &tapi->clist[chndle];

   /*
     Get the session handle instance
   */
   shndle = pcs->session_handle;

   if ((shndle >= pconn->max_sessions) || (pconn->s[shndle].used == 0))
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: invalid session handle [%d]", pcs->session_handle);
      err = SK_ERROR_SESSION_HANDLE_NOT_FOUND;
      goto DONE;
   }

   /*
      Setup the message
   */
   msg.hdr.context_id  = pconn->context_id;
   msg.hdr.sm_id       = tapi->sm_id;
   msg.hdr.cmd         = SK_TAPI_CLOSE_SESSION;
   SYSV_MEMCPY(msg.type.close_session.req.context_handle, pconn->context,           SK_TAPI_CONTEXT_HANDLE_SIZE);
   SYSV_MEMCPY(msg.type.close_session.req.session_handle, pconn->s[shndle].session, SK_TAPI_SESSION_HANDLE_SIZE);

   //SYSV_LDUMP(tapi->log, "drv client: context handle", msg.type.close_session.req.context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
   //SYSV_LDUMP(tapi->log, "drv client: session handle", msg.type.close_session.req.session_handle, SK_TAPI_SESSION_HANDLE_SIZE);

   /*
     Byte pack the information back into a send message buffer
   */
   if ((err = sk_tapi_close_session_pack(&msg, send_buffer, &send_buffer_size, SK_TAPI_MSG_REQ_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }


   if ((err = sk_nwld_platform_comm_proc(pconn->comm,
                                    send_buffer,
                                    send_buffer_size,
                                    recv_buffer,
                                    recv_buffer_size,
                                    0)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR3(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: process Client Comm: Context_id=0x%.4x sm_id=0x%.4x err=%d",
                  pconn->context_id, tapi->sm_id, err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }

   /*
      Byte unpack the information back into the receiving structure
   */
   if ((err = sk_tapi_close_session_unpack(recv_buffer, (*recv_buffer_size), &msg, SK_TAPI_MSG_RESP_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }


   /*
     Check Message Header
   */
   if (msg.hdr.cmd == SK_TAPI_SYSTEM)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: internal TAPI error [%d]", msg.type.sys_tapi.resp.status);
      err = SK_ERROR_FAILED;
      goto DONE;
   }
   else if (msg.hdr.cmd != SK_TAPI_CLOSE_SESSION)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: wrong response command [%d]", msg.hdr.cmd);
      err = SK_ERROR_INVALID_CMD;
      goto DONE;
   }

   /*
      check and make sure the results pass
   */
   if (msg.type.close_session.resp.status != SKTAPIPACKERR_SUCCESS)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: close session failed [%d]", msg.type.close_session.resp.status);
      err = SK_ERROR_FAILED;
      goto DONE;
   }

   /*
     capture the session handle
   */
   SYSV_ASSERT(pconn->num_sessions != 0);

   pconn->num_sessions--;
   SYSV_MEMSET(&pconn->s[shndle], 0, sizeof(tSkSysSession));

DONE:

   /* wipe */
   SYSV_MEMSET(&msg, 0, sizeof(sk_msg));
   return err;
}

SKTEEC_STATUS sk_nwld_tapi_invoke_command(sk_nwld_tapi* tapi,
                                              sk_teec_user_info* user_info,
                                              uint8_t* send_buffer,
                                              uint32_t  send_buffer_size,
                                              uint8_t* recv_buffer,
                                              uint32_t* recv_buffer_size)
{
   SKTEEC_STATUS                err;
   uint32_t                     chndle;
   uint32_t                     shndle;
   sk_msg                       msg;
   sk_teec_user_invoke_cmd      *pic;
   tSkSysConnection*            pconn;

   if ((tapi == 0)             ||
       (user_info == 0)        ||
       (send_buffer == 0)      ||
       (recv_buffer == 0)      ||
       (send_buffer_size == 0) ||
       (recv_buffer_size == 0) ||
       ((*recv_buffer_size) == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   pic = &user_info->type.invoke_cmd;

   /*
     Get the connection instance from the context handle
   */
   chndle = pic->context_handle;
   if ((chndle >= tapi->max_connections) || (tapi->clist[chndle].used == 0))
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: invalid context handle [%d]", pic->context_handle);
      err = SK_ERROR_CONTEXT_HANDLE_NOT_FOUND;
      goto DONE;
   }

   pconn = &tapi->clist[chndle];


   /*
     Get the session handle instance
   */
   shndle = pic->session_handle;

   if ((shndle >= pconn->max_sessions) || (pconn->s[shndle].used == 0))
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: invalid session handle [%d]", pic->session_handle);
      err = SK_ERROR_SESSION_HANDLE_NOT_FOUND;
      goto DONE;
   }

   /*
      Setup the message
   */
   msg.hdr.context_id          = pconn->context_id;
   msg.hdr.sm_id               = tapi->sm_id;
   msg.hdr.cmd                 = SK_TAPI_INVOKE_COMMAND;
   msg.type.invoke_cmd.req.cmd = pic->cmd;
   SYSV_MEMCPY(msg.type.invoke_cmd.req.context_handle, pconn->context,           SK_TAPI_CONTEXT_HANDLE_SIZE);
   SYSV_MEMCPY(msg.type.invoke_cmd.req.session_handle, pconn->s[shndle].session, SK_TAPI_SESSION_HANDLE_SIZE);

   //SYSV_LDUMP(tapi->log, "drv client: context handle", msg.type.invoke_cmd.req.context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
   //SYSV_LDUMP(tapi->log, "drv client: session handle", msg.type.invoke_cmd.req.session_handle, SK_TAPI_SESSION_HANDLE_SIZE);

   /*
     Byte pack the information back into a send message buffer
   */
   if ((err = sk_tapi_invoke_command_pack(&msg, send_buffer, &send_buffer_size, SK_TAPI_MSG_REQ_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }

   if ((err = sk_nwld_platform_comm_proc(pconn->comm,
                                    send_buffer,
                                    send_buffer_size,
                                    recv_buffer,
                                    recv_buffer_size,
                                    &pic->op)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR3(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: process Client Comm: Context_id=0x%.4x sm_id=0x%.4x err=%d",
                  pconn->context_id, tapi->sm_id, err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }

   /*
      Byte unpack the information back into the receiving structure
   */
   if ((err = sk_tapi_invoke_command_unpack(recv_buffer, (*recv_buffer_size), &msg, SK_TAPI_MSG_RESP_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }


   /*
     Check Message Header
   */
   if (msg.hdr.cmd == SK_TAPI_SYSTEM)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: internal TAPI error [%d]", msg.type.sys_tapi.resp.status);
      err = SK_ERROR_FAILED;
      goto DONE;
   }
   else if (msg.hdr.cmd != SK_TAPI_INVOKE_COMMAND)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: wrong response command [%d]", msg.hdr.cmd);
      err = SK_ERROR_INVALID_CMD;
      goto DONE;
   }

   /*
      check and make sure the results pass
   */
   if (msg.type.invoke_cmd.resp.status != SKTAPIPACKERR_SUCCESS)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: invoke command failed [%d]", msg.type.invoke_cmd.resp.status);
      err = SK_ERROR_FAILED;
      goto DONE;
   }

DONE:
   return err;
}

SKTEEC_STATUS sk_nwld_tapi_register_memory(sk_nwld_tapi* tapi,
                                               sk_teec_user_info* user_info,
                                               uint8_t* send_buffer,
                                               uint32_t  send_buffer_size,
                                               uint8_t* recv_buffer,
                                               uint32_t* recv_buffer_size)
{
   SKTEEC_STATUS                err;
   uint32_t                     chndle;
   sk_msg                       msg;
   sk_teec_user_register_memory *rm;
   tSkSysConnection*            pconn;

   if ((tapi == 0)             ||
       (user_info == 0)        ||
       (send_buffer == 0)      ||
       (recv_buffer == 0)      ||
       (send_buffer_size == 0) ||
       (recv_buffer_size == 0) ||
       ((*recv_buffer_size) == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   rm = &user_info->type.reg_mem;

   /*
     Get the connection instance from the context handle
   */
   chndle = rm->context_handle;
   if ((chndle >= tapi->max_connections) || (tapi->clist[chndle].used == 0))
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: invalid context handle [%d]", rm->context_handle);
      err = SK_ERROR_CONTEXT_HANDLE_NOT_FOUND;
      goto DONE;
   }

   pconn = &tapi->clist[chndle];

   /*
      Setup the message
   */
   msg.hdr.context_id  = pconn->context_id;
   msg.hdr.sm_id       = tapi->sm_id;
   msg.hdr.cmd         = SK_TAPI_REGISTER_MEMORY;
   SYSV_MEMCPY(msg.type.register_memory.req.context_handle, pconn->context, SK_TAPI_CONTEXT_HANDLE_SIZE);
   msg.type.register_memory.req.memory_size = rm->memory_size;
   msg.type.register_memory.req.flag        = rm->flag;
   msg.type.register_memory.req.addr        = rm->addr;

   /*
     Byte pack the information back into a send message buffer
   */
   if ((err = sk_tapi_register_memory_pack(&msg, send_buffer, &send_buffer_size, SK_TAPI_MSG_REQ_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }


   if ((err = sk_nwld_platform_comm_proc(pconn->comm,
                                    send_buffer,
                                    send_buffer_size,
                                    recv_buffer,
                                    recv_buffer_size,
                                    0)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR3(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: process Client Comm: Context_id=0x%.4x sm_id=0x%.4x err=%d",
                  pconn->context_id, tapi->sm_id, err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }

   /*
      Byte unpack the information back into the receiving structure
   */
   if ((err = sk_tapi_register_memory_unpack(recv_buffer, (*recv_buffer_size), &msg, SK_TAPI_MSG_RESP_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }


   /*
     Check Message Header
   */
   if (msg.hdr.cmd == SK_TAPI_SYSTEM)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: internal TAPI error [%d]", msg.type.sys_tapi.resp.status);
      err = SK_ERROR_FAILED;
      goto DONE;
   }
   else if (msg.hdr.cmd != SK_TAPI_REGISTER_MEMORY)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: wrong response command [%d]", msg.hdr.cmd);
      err = SK_ERROR_INVALID_CMD;
      goto DONE;
   }

   /*
      check and make sure the results pass
   */
   if (msg.type.register_memory.resp.status != SKTAPIPACKERR_SUCCESS)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: register memory failed [%d]", msg.type.register_memory.resp.status);
      err = SK_ERROR_FAILED;
      goto DONE;
   }

   /*
      open the memory
   */
   if ((err = sk_nwld_platform_open_memory(msg.type.register_memory.resp.memory_id, tapi->log)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: opening memory [%d]", err);
      err = SK_ERROR_REGMEM_CMD;
      goto DONE;
   }

   rm->addr = msg.type.register_memory.resp.addr;
   SYSV_MEMCPY(rm->memory_id, msg.type.register_memory.resp.memory_id, SK_TAPI_MEMORY_ID_LENGTH);

DONE:
   return err;
}

SKTEEC_STATUS sk_nwld_tapi_unregister_memory(sk_nwld_tapi* tapi,
                                                 sk_teec_user_info* user_info,
                                                 uint8_t* send_buffer,
                                                 uint32_t  send_buffer_size,
                                                 uint8_t* recv_buffer,
                                                 uint32_t* recv_buffer_size)
{
   SKTEEC_STATUS                  err;
   uint32_t                       chndle;
   sk_msg                         msg;
   sk_teec_user_unregister_memory *urm;
   tSkSysConnection*              pconn;

   if ((tapi == 0)             ||
       (user_info == 0)        ||
       (send_buffer == 0)      ||
       (recv_buffer == 0)      ||
       (send_buffer_size == 0) ||
       (recv_buffer_size == 0) ||
       ((*recv_buffer_size) == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   urm = &user_info->type.unreg_mem;

   /*
     Get the connection instance from the context handle
   */
   chndle = urm->context_handle;
   if ((chndle >= tapi->max_connections) || (tapi->clist[chndle].used == 0))
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: invalid context handle [%d]", urm->context_handle);
      err = SK_ERROR_CONTEXT_HANDLE_NOT_FOUND;
      goto DONE;
   }

   pconn = &tapi->clist[chndle];

   /*
      Setup the message
   */
   msg.hdr.context_id  = pconn->context_id;
   msg.hdr.sm_id       = tapi->sm_id;
   msg.hdr.cmd         = SK_TAPI_UNREGISTER_MEMORY;
   SYSV_MEMCPY(msg.type.unregister_memory.req.context_handle, pconn->context, SK_TAPI_CONTEXT_HANDLE_SIZE);
   SYSV_MEMCPY(msg.type.unregister_memory.req.memory_id, urm->memory_id, SK_TAPI_MEMORY_ID_LENGTH);

   /*
     Byte pack the information back into a send message buffer
   */
   if ((err = sk_tapi_unregister_memory_pack(&msg, send_buffer, &send_buffer_size, SK_TAPI_MSG_REQ_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }


   if ((err = sk_nwld_platform_comm_proc(pconn->comm,
                                    send_buffer,
                                    send_buffer_size,
                                    recv_buffer,
                                    recv_buffer_size,
                                    0)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR3(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: process Client Comm: Context_id=0x%.4x sm_id=0x%.4x err=%d",
                  pconn->context_id, tapi->sm_id, err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }

   /*
      Byte unpack the information back into the receiving structure
   */
   if ((err = sk_tapi_unregister_memory_unpack(recv_buffer, (*recv_buffer_size), &msg, SK_TAPI_MSG_RESP_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_COMM_FAILED;
      goto DONE;
   }

   /*
     Check Message Header
   */
   if (msg.hdr.cmd == SK_TAPI_SYSTEM)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: internal TAPI error [%d]", msg.type.sys_tapi.resp.status);
      err = SK_ERROR_FAILED;
      goto DONE;
   }
   else if (msg.hdr.cmd != SK_TAPI_UNREGISTER_MEMORY)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: wrong response command [%d]", msg.hdr.cmd);
      err = SK_ERROR_INVALID_CMD;
      goto DONE;
   }

   /*
      check and make sure the results pass
   */
   if (msg.type.unregister_memory.resp.status != SKTAPIPACKERR_SUCCESS)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: de-register memory failed [%d]", msg.type.register_memory.resp.status);
      err = SK_ERROR_FAILED;
      goto DONE;
   }

   /*
      close the memory
   */
   if ((err = sk_nwld_platform_close_memory(msg.type.unregister_memory.req.memory_id, tapi->log)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tapi->log, SYSV_LOG_CRITICAL_ERR, "Error: closing memory [%d]", err);
      err = SK_ERROR_REGMEM_CMD;
      goto DONE;
   }

DONE:
   return err;
}

SKTEEC_STATUS sk_nwld_tapi_print_info(sk_nwld_tapi* tapi)
{
   uint32_t i,k;

   SYSV_INFO0(tapi->log, "Client TAPI Information");
   SYSV_INFO1(tapi->log, "SM ID = 0x%.4x", tapi->sm_id);
   SYSV_INFO1(tapi->log, "MAX CONNECTIONS = %d", tapi->max_connections);
   SYSV_INFO1(tapi->log, "NUM CONNECTIONS = %d", tapi->num_connections);
   SYSV_INFO1(tapi->log, "NUM CONTEXTS ID = %d", tapi->num_context_id);

   for(i = 0; i < SK_TAPI_MAX_CONTEXT_IDS; i++)
   {
      SYSV_INFO1(tapi->log, "---------------------- %d -------------", i);
      SYSV_INFO2(tapi->log, "info[%d].used = %d", i, tapi->info[i].used);
      SYSV_INFO2(tapi->log, "info[%d].context_id = 0x%.4x", i, tapi->info[i].context_id);
      SYSV_INFO2(tapi->log, "info[%d].context_id_cnt = %d", i, tapi->info[i].context_id_cnt);
      SYSV_INFO0(tapi->log, "---------------------------------------");
   }

   /*
      Connections
   */
   for(i = 0; i < tapi->max_connections; i++)
   {
      SYSV_INFO1(tapi->log, "***> Connection Info %d <***", i);
      SYSV_INFO2(tapi->log, "clist[%d]:used = %d", i, tapi->clist[i].used);
      SYSV_INFO2(tapi->log, "clist[%d]:CONTEXT ID = 0x%.4x", i, tapi->clist[i].context_id);
      SYSV_INFO2(tapi->log, "clist[%d]:CONTEXT ID IDX = %d", i, tapi->clist[i].context_id_index);
      SYSV_INFO2(tapi->log, "clist[%d]:MAX SESSIONS = %d", i, tapi->clist[i].max_sessions);
      SYSV_INFO2(tapi->log, "clist[%d]:NUM SESSIONS = %d", i, tapi->clist[i].num_sessions);

      /*
         Sessions
      */
      for(k = 0; k < tapi->clist->max_sessions; k++)
      {
         SYSV_INFO1(tapi->log, "***> Session Info %d <***", k);
         SYSV_INFO2(tapi->log, "s[%d]:used = %d", k, tapi->clist->s[k].used);
      }
   }

   return SK_ERROR_SUCCESS;
}







