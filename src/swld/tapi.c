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


#include <sk_both.h>
#include <sk_swld.h>
#include <sk_swld.h>
#include <sk_swld.h>
#include <sk_tapp_swld_app_interface.h>

/*
  Internal Functions
*/
SK_STATUS         sk_swld_tapi_open_context_nw      (sk_swld_tapi* tObj);
SK_STATUS         sk_swld_tapi_close_context_nw     (sk_swld_tapi* tObj);
SK_STATUS         sk_swld_tapi_open_session_nw      (sk_swld_tapi* tObj);
SK_STATUS         sk_swld_tapi_close_session_nw     (sk_swld_tapi* tObj);
SK_STATUS         sk_swld_tapi_invoke_command_nw    (sk_swld_tapi* tObj);
SK_STATUS         sk_swld_tapi_register_memory_nw   (sk_swld_tapi* tObj);
SK_STATUS         sk_swld_tapi_unregister_memory_nw (sk_swld_tapi* tObj);
SK_STATUS         sk_swld_tapi_check_nw             (sk_swld_tapi* tObj);
SK_STATUS         sk_swld_tapi_find_objects         (sk_swld_tapi* tObj, uint8_t* context_handle, uint32_t* c_idx, uint8_t* session_handle, uint32_t* s_idx);
tApiSessionTable* sk_swld_tapi_remove_session       (sk_swld_tapi* tObj, tApiContextTable* context, tApiSessionTable* session);
SK_STATUS         sk_swld_tapi_remove_context       (sk_swld_tapi* tObj, tApiContextTable* context, uint8_t force_shutdown);

SK_STATUS sk_swld_tapi_find_objects(sk_swld_tapi* tObj,
                            uint8_t* context_handle, uint32_t* c_idx,
                            uint8_t* session_handle, uint32_t* s_idx)
{
   SK_STATUS        err;
   tApiSessionTable *st;

   SYSV_ASSERT(c_idx                          != 0);
   SYSV_ASSERT(s_idx                          != 0);
   SYSV_ASSERT(context_handle                 != 0);
   SYSV_ASSERT(session_handle                 != 0);
   SYSV_ASSERT(tObj->conn.num_ctable_elements <= SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT);
   SYSV_ASSERT(tObj->conn.num_stable_elements <= SK_TAPI_MAX_SESSIONS_PER_CONTEXT);

   /*
      find the context object index based on the handle
   */
   if ((err = sk_swld_tapi_handle_generator_plugin_find_context_handle(&tObj->hp,
                                                               SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT,
                                                               tObj->conn.cthandles,
                                                               context_handle,
                                                               SK_TAPI_CONTEXT_HANDLE_SIZE,
                                                               c_idx)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Failed to find context handle [%d]", err);
      err = SK_ERROR_CONTEXT_HANDLE_NOT_FOUND;
      goto DONE;
   }

   (*c_idx) /= SK_TAPI_CONTEXT_HANDLE_SIZE;

   SYSV_ASSERT((*c_idx) < SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT);
   SYSV_ASSERT(tObj->conn.ctable[(*c_idx)].active == 1);

   /*
      find the session object index based on the handle
   */
   if ((err = sk_swld_tapi_handle_generator_plugin_find_session_handle(&tObj->hp,
                                                               SK_TAPI_MAX_SESSIONS_PER_CONTEXT,
                                                               tObj->conn.sthandles,
                                                               session_handle,
                                                               SK_TAPI_SESSION_HANDLE_SIZE,
                                                               s_idx)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Failed to find session handle [%d]", err);
      err = SK_ERROR_SESSION_HANDLE_NOT_FOUND;
      goto DONE;
   }

   (*s_idx) /= SK_TAPI_SESSION_HANDLE_SIZE;

   SYSV_ASSERT((*s_idx) < SK_TAPI_MAX_SESSIONS_PER_CONTEXT);
   SYSV_ASSERT(tObj->conn.stable[(*s_idx)].active == 1);

   /*
     verify the session is attached to the context
   */
   err = SK_ERROR_VERIFY_FAILED;

   for (st = tObj->conn.ctable[(*c_idx)].start; st != 0; st = st->next)
   {
      if (st == &tObj->conn.stable[(*s_idx)])
      {
         /*
           we found it so pass the check
         */
         err = SK_ERROR_SUCCESS;
         break;
      }
   };

DONE:
   return err;
}

tApiSessionTable *sk_swld_tapi_remove_session(sk_swld_tapi* tObj, tApiContextTable* context, tApiSessionTable* session)
{
   tApiSessionTable* s;
   tApiSessionTable* s_next;

   SYSV_ASSERT(tObj != 0);
   SYSV_ASSERT(context != 0);
   SYSV_ASSERT(session != 0);

   s      = session;
   s_next = s->next;

   /*
     Remove session
   */
   if ( context->start == s )
   {
      /*
         this is the first element
      */
      if (s->next != 0)
      {
        context->start       = s->next;
        context->start->prev = 0;
      }
      else
      {
         /*
           this must be the last element, no more items
         */
         context->start = 0;
      }
   }
   else
   {
      /*
        this is element is not the first
      */
      /*
        set the "next" pointer from the previous element
      */
      ((tApiSessionTable *)s->prev)->next = s->next;

      if (s->next != 0)
      {
         /*
           set the "prev" pointer from the next element
         */
         ((tApiSessionTable *)s->next)->prev = s->prev;
      }
   }

   s->active = 0;
   s->next   = 0;
   s->prev   = 0;
   SYSV_MEMSET(s->taid, 0, SK_TAPI_SESSION_TAID_SIZE);

   SYSV_ASSERT(context->num_connected_sessions > 0);
   context->num_connected_sessions--;

   SYSV_ASSERT(tObj->conn.num_stable_elements > 0);
   tObj->conn.num_stable_elements--;

   return s_next;
}

SK_STATUS sk_swld_tapi_remove_context(sk_swld_tapi* tObj, tApiContextTable* context, uint8_t force_shutdown)
{
   SK_STATUS err;

   SYSV_ASSERT(tObj != 0);
   SYSV_ASSERT(context != 0);

   err = SK_ERROR_SUCCESS;

   if (context->num_connected_sessions > 0)
   {
      if (force_shutdown == 1)
      {
         /*
            remove all the sessions
         */
         while(sk_swld_tapi_remove_session(tObj, context, context->start) != 0);
      }
      else
      {
         SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Active Sessions still open [%d]", context->num_connected_sessions);
         err = SK_ERROR_FAILED;
         goto DONE;
      }
   }

   /*
      Shut everything down
   */
   SYSV_ASSERT(context->start == 0);

   context->active                 = 0;
   context->pid                    = 0;
   context->start                  = 0;
   context->num_connected_sessions = 0;

DONE:
   return err;
}

SK_STATUS sk_swld_tapi_init(sk_swld_tapi*                  tObj,
                    uint32_t                  context_id,
                    uint32_t                  sm_id,
                    uint32_t                  max_recv_mem_size,
                    uint32_t                  max_send_mem_size,
                    sk_nwld_to_swld_msg_api*      msgInt,
                    sk_swld_tapi_kernel_access* kaInt,
                    void*                     hp_plugin)
{
   SK_STATUS err;
   uint32_t  i;

   if ((tObj == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   /*
      set the context ID for thus TAPI service
   */
   tObj->context_id = context_id;

   /*
     set the security machine ID
   */
   tObj->sm_id = sm_id;

   SYSV_INFO2(tObj->log, "CONTEXT ID = 0x%.4x SM_ID = 0x%.4x", tObj->context_id, tObj->sm_id);

   /*
     set the user-defined PLATFORM object
   */
   tObj->ka = (*kaInt);

   /*
     setup the core memory region
   */
   tObj->max_msend_size = max_send_mem_size;
   tObj->max_mrecv_size = max_recv_mem_size;
   tObj->msend_size     = tObj->max_msend_size;
   tObj->mrecv_size     = tObj->max_mrecv_size;

   if ((tObj->msend = SYSV_MALLOC(max_send_mem_size)) == 0)
   {
      return SK_ERROR_NO_MEMORY;
   }

   if ((tObj->mrecv = SYSV_MALLOC(max_recv_mem_size)) == 0)
   {
      return SK_ERROR_NO_MEMORY;
   }

   /*
     clear the memory table
   */
   tObj->rmem.mem_region_count = 0;
   tObj->rmem.m                = 0;

   /*
     set the user-defined Message APU object
   */
   tObj->msg = 0;
   if (msgInt != 0)
   {
      tObj->msg = msgInt;
   }

   /*
      Setup the Handle generator
   */
   if ((err = sk_swld_tapi_handle_generator_plugin_init(&tObj->hp, hp_plugin)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error: opening HGP err=%d", err);
      err = SK_ERROR_OPEN_COMM;
      goto DONE;
   }

   /*
      Zero out the remaining elements
   */
   SYSV_MEMSET(&tObj->conn, 0, sizeof(tApiConnectionTable));

   /*
      Setup the pointers
   */
   for (i = 0; i<SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT; i++)
   {
      tObj->conn.ctable[i].context_handle = &tObj->conn.cthandles[(i * SK_TAPI_CONTEXT_HANDLE_SIZE)];
   }

   for (i = 0; i<SK_TAPI_MAX_SESSIONS_PER_CONTEXT; i++)
   {
      tObj->conn.stable[i].session_handle = &tObj->conn.sthandles[(i * SK_TAPI_CONTEXT_HANDLE_SIZE)];
      tObj->conn.stable[i].idx            = i;
   }

DONE:
   return err;
}

SK_STATUS sk_swld_tapi_close(sk_swld_tapi* tObj)
{
   SK_STATUS err;

   if (tObj == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   err = SK_ERROR_SUCCESS;

   if (tObj->msend != 0)
   {
      SYSV_FREE(tObj->msend);
   }

   if (tObj->mrecv != 0)
   {
      SYSV_FREE(tObj->mrecv);
   }

   return err;
}

SK_STATUS sk_swld_tapi_process(sk_swld_tapi* tObj)
{
   SK_STATUS err;
   sk_header hdr;

   if (tObj == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   SYSV_ASSERT(tObj->mrecv_size <= tObj->max_mrecv_size);

   /*
     get the command from the buffer
   */
   if ((err = sk_tapi_header_unpack(tObj->mrecv, tObj->mrecv_size, &hdr)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }


   if (hdr.context_id != tObj->context_id)
   {
      SYSV_ERROR0(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error: Incorrect Context ID");
      err = SK_ERROR_BAD_CONTEXT_ID;
      goto DONE;
   }

   if (hdr.sm_id != tObj->sm_id)
   {
      SYSV_ERROR0(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error: Incorrect SM ID");
      err = SK_ERROR_BAD_SM_ID;
      goto DONE;
   }

   switch(hdr.cmd)
   {
      case SK_TAPI_OPEN_CONTEXT:
         if ((err = sk_swld_tapi_open_context_nw(tObj)) != SK_ERROR_SUCCESS)
         {
            SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error: TAPI opening context err=%d", err);
            err = SK_ERROR_OPEN_CONTEXT;
         }
         break;
      case SK_TAPI_CLOSE_CONTEXT:
         if ((err = sk_swld_tapi_close_context_nw(tObj)) != SK_ERROR_SUCCESS)
         {
            SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error: TAPI closing context err=%d", err);
            err = SK_ERROR_CLOSE_CONTEXT;
         }
         break;
      case SK_TAPI_OPEN_SESSION:
         if ((err = sk_swld_tapi_open_session_nw(tObj)) != SK_ERROR_SUCCESS)
         {
            SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error: TAPI opening session err=%d", err);
            err = SK_ERROR_OPEN_SESSION;
         }
         break;
      case SK_TAPI_CLOSE_SESSION:
         if ((err = sk_swld_tapi_close_session_nw(tObj)) != SK_ERROR_SUCCESS)
         {
            SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error: TAPI closing session err=%d", err);
            err = SK_ERROR_CLOSE_SESSION;
         }
         break;
      case SK_TAPI_INVOKE_COMMAND:
         if ((err = sk_swld_tapi_invoke_command_nw(tObj)) != SK_ERROR_SUCCESS)
         {
            SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error: TAPI invoking command err=%d", err);
            err = SK_ERROR_INV_CMD;
         }
         break;
      case SK_TAPI_REGISTER_MEMORY:
         if ((err = sk_swld_tapi_register_memory_nw(tObj)) != SK_ERROR_SUCCESS)
         {
            SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error: TAPI register memory err=%d", err);
            err = SK_ERROR_REGMEM_CMD;
         }
         break;
      case SK_TAPI_UNREGISTER_MEMORY:
         if ((err = sk_swld_tapi_unregister_memory_nw(tObj)) != SK_ERROR_SUCCESS)
         {
            SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error: TAPI unregister memory err=%d", err);
            err = SK_ERROR_UNREGMEM_CMD;
         }
         break;
      case SK_TAPI_CHECK:
         if ((err = sk_swld_tapi_check_nw(tObj)) != SK_ERROR_SUCCESS)
         {
            SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error: TAPI check err=%d", err);
            err = SK_ERROR_CHECK_CMD;
         }
         break;
      default:
         SYSV_ERROR2(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error: TAPI invalid command cmd=%d err=%d", hdr.cmd, err);
         err = SK_ERROR_INVALID_CMD;
         break;
   };

DONE:
   return err;
}

SK_STATUS sk_swld_tapi_dispatch(sk_swld_tapi* tObj)
{
   SK_STATUS    err;
   sk_msg       msg;

   if (tObj == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   err = SK_ERROR_SUCCESS;

   SYSV_ASSERT(tObj->msg != 0);

   /*
      unless there is an error we will loop here forever
   */
   do
   {
      /*
         read the incoming request from the Srv Comm port
      */
      if (tObj->msg != 0)
      {
         tObj->op.num_param = 0;
         if ((err = tObj->msg->tapi_recv_msg(tObj->msg->mObj, tObj->mrecv, &tObj->mrecv_size, &tObj->op)) != 0)
         {
            SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error: receiving Srv Comm kerr=%d", err);
            err = SK_ERROR_RECV_COMM;
         }
      }

      if (err == SK_ERROR_SUCCESS)
      {
         if ((err = sk_swld_tapi_process(tObj)) == SK_ERROR_SUCCESS)
         {
            SYSV_ASSERT(tObj->msend_size <= tObj->max_msend_size);

            if ((err == SK_ERROR_BAD_CONTEXT_ID) ||
                (err == SK_ERROR_BAD_SM_ID))
            {
               if ((err = sk_tapi_header_unpack(tObj->mrecv, tObj->mrecv_size, &msg.hdr)) != SKTAPIPACKERR_SUCCESS) {
                  SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
                  err = SK_ERROR_INVALID_PARAM;
                  goto DONE;
               }

               msg.hdr.cmd = SK_TAPI_SYSTEM;

               switch(err)
               {
                  case SK_ERROR_BAD_CONTEXT_ID:
                     msg.type.sys_tapi.resp.status = SK_ERROR_BAD_CONTEXT_ID;
                     break;
                  case SK_ERROR_BAD_SM_ID:
                     msg.type.sys_tapi.resp.status  = SK_ERROR_BAD_SM_ID;
                     break;
                  default:
                     break;
               }

               tObj->msend_size = tObj->max_msend_size;
               if ((err = sk_tapi_sys_tapi_pack(&msg, tObj->msend, &tObj->msend_size, SK_TAPI_MSG_RESP_TYPE)) != SKTAPIPACKERR_SUCCESS) {
                  SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
                  err = SK_ERROR_INVALID_PARAM;
                  goto DONE;
               }
            }

            if (tObj->msg != 0)
            {
               /*
                  send the out-going message from the Srv Comm port
               */
               if ((err = tObj->msg->tapi_send_msg(tObj->msg->mObj, tObj->msend, tObj->msend_size, &tObj->op)) != 0)
               {
                  SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error: sending Srv Comm kerr=%d", err);
                  err = SK_ERROR_SEND_COMM;
               }
            }
         }
      }
#ifndef USE_SIM_X86
   }while(err == SK_ERROR_SUCCESS);
#else
   }while(0);
#endif
DONE:
   return err;
}

SK_STATUS sk_swld_tapi_open_context_nw(sk_swld_tapi* tObj)
{
   SK_STATUS             err;
   sk_msg                req_msg;
   sk_msg                resp_msg;
   sk_open_context_req*  oc_req;
   sk_open_context_resp* oc_resp;
   uint32_t              i;

   SYSV_ASSERT(tObj != 0);

   if (tObj == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   /*
     Open Context Command
   */
   SYSV_INFO0(tObj->log, "Tapi: Open Context");

   SYSV_ASSERT(tObj->conn.num_ctable_elements <= SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT);
   SYSV_ASSERT(tObj->conn.num_stable_elements <= SK_TAPI_MAX_SESSIONS_PER_CONTEXT);

   if ((err = sk_tapi_open_context_unpack(tObj->mrecv, tObj->mrecv_size, &req_msg, SK_TAPI_MSG_REQ_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }


   resp_msg.hdr = req_msg.hdr;
   oc_req       = &req_msg.type.open_context.req;
   oc_resp      = &resp_msg.type.open_context.resp;

   /*
      check the context ID
   */
   if (oc_req->context_id != tObj->context_id)
   {
      SYSV_ERROR2(tObj->log, SYSV_LOG_CRITICAL_ERR, "Context ID mismatch [0x%.4x , 0x%.4x]",
                  oc_req->context_id, tObj->context_id);
      oc_resp->status = SK_ERROR_BAD_CONTEXT_ID;
      goto RESPOND;
   }

   /*
      check for space
   */
   if (tObj->conn.num_ctable_elements >= SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT)
   {
      SYSV_ERROR2(tObj->log, SYSV_LOG_CRITICAL_ERR, "Too many connections  [%d / %d]",
                  tObj->conn.num_ctable_elements, SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT);
      oc_resp->status = SK_ERROR_OPEN_CONTEXT;
      goto RESPOND;
   }

   /*
      space is available now find a free spot
   */
   for (i=0; i<SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT;i++)
   {
     if (tObj->conn.ctable[i].active == 0)
     {
        /*
           we found it a free spot
        */
        break;
     }
   }

   SYSV_ASSERT(i < SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT); /* should never occur here */

   /*
      set the object
   */
   if ((err = sk_swld_tapi_handle_generator_plugin_generate_context_handle(&tObj->hp,
                                                                   tObj->conn.ctable[i].context_handle,
                                                                   SK_TAPI_CONTEXT_HANDLE_SIZE)) != SK_TAPI_CONTEXT_HANDLE_SIZE)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Failed to generate context handle [%d]", err);

      /* fail process since this should never happen */
      err = SK_ERROR_FAILED_GEN_HANDLE;
      goto DONE;
   }

   SYSV_LDUMP(tObj->log, "Context Handle", tObj->conn.ctable[i].context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);

   tObj->conn.ctable[i].active = 1;
   tObj->conn.ctable[i].pid    = oc_req->pid;

   SYSV_ASSERT(tObj->conn.num_ctable_elements < SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT);
   tObj->conn.num_ctable_elements++;

   /*
      Send Response Status
   */
   SYSV_MEMCPY(oc_resp->context_handle, tObj->conn.ctable[i].context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
   oc_resp->status = SK_ERROR_SUCCESS;


RESPOND:
   tObj->msend_size = tObj->max_msend_size;
   if ((err = sk_tapi_open_context_pack(&resp_msg, tObj->msend, &tObj->msend_size, SK_TAPI_MSG_RESP_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }


DONE:

   /* wipe */
   SYSV_MEMSET(&req_msg,  0, sizeof(sk_msg));
   SYSV_MEMSET(&resp_msg, 0, sizeof(sk_msg));

   return err;
}

SK_STATUS sk_swld_tapi_close_context_nw(sk_swld_tapi* tObj)
{
   SK_STATUS             err;
   sk_msg                req_msg;
   sk_msg                resp_msg;
   sk_close_context_req  *cc_req;
   sk_close_context_resp *cc_resp;
   uint32_t               idx;

   SYSV_ASSERT(tObj != 0);

   if (tObj == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   /*
     Close Context Command
   */
   SYSV_INFO0(tObj->log, "Tapi: Close Context");

   SYSV_ASSERT(tObj->conn.num_ctable_elements <= SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT);
   SYSV_ASSERT(tObj->conn.num_stable_elements <= SK_TAPI_MAX_SESSIONS_PER_CONTEXT);

   if ((err = sk_tapi_close_context_unpack(tObj->mrecv, tObj->mrecv_size, &req_msg, SK_TAPI_MSG_REQ_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }


   resp_msg.hdr = req_msg.hdr;
   cc_req       = &req_msg.type.close_context.req;
   cc_resp      = &resp_msg.type.close_context.resp;

   /*
      find the context object index based on the handle
   */
   if ((err = sk_swld_tapi_handle_generator_plugin_find_context_handle(&tObj->hp,
                                                               SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT,
                                                               tObj->conn.cthandles,
                                                               cc_req->context_handle,
                                                               SK_TAPI_CONTEXT_HANDLE_SIZE,
                                                               &idx)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Failed to find context handle [%d]", err);

      cc_resp->status = SK_ERROR_CONTEXT_HANDLE_NOT_FOUND;
      goto RESPOND;
   }

   idx /= SK_TAPI_CONTEXT_HANDLE_SIZE;


   SYSV_ASSERT(idx                           < SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT);
   SYSV_ASSERT(tObj->conn.ctable[idx].active == 1);

#if 0
   /*
     Make sure all the sessions are closed
   */
   if (tObj->conn.ctable[idx].num_connected_sessions > 0)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Active Sessions still open [%d]", tObj->conn.ctable[idx].num_connected_sessions);

      cc_resp->status = SK_ERROR_OPEN_SESSION;
      goto RESPOND;
   }

   /*
      Shut everything down
   */
   SYSV_ASSERT(tObj->conn.ctable[idx].start == 0);

   tObj->conn.ctable[idx].time_access            = sk_drv_swld_secure_timer_get();
   tObj->conn.ctable[idx].active                 = 0;
   tObj->conn.ctable[idx].pid                    = 0;
   tObj->conn.ctable[idx].start                  = 0;
   tObj->conn.ctable[idx].num_connected_sessions = 0;

#else
   sk_swld_tapi_remove_context(tObj, &tObj->conn.ctable[idx], 0);
#endif


   /*
      Decrement the connection count
   */
   SYSV_ASSERT(tObj->conn.num_ctable_elements > 0);
   tObj->conn.num_ctable_elements--;

   /*
      Send Response Status
   */
   cc_resp->status = SK_ERROR_SUCCESS;


RESPOND:
   tObj->msend_size = tObj->max_msend_size;
   if ((err = sk_tapi_close_context_pack(&resp_msg, tObj->msend, &tObj->msend_size, SK_TAPI_MSG_RESP_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }

   /* wipe */
   SYSV_MEMSET(&req_msg,  0, sizeof(sk_msg));
   SYSV_MEMSET(&resp_msg, 0, sizeof(sk_msg));
DONE:
   return err;
}

SK_STATUS sk_swld_tapi_open_session_nw(sk_swld_tapi* tObj)
{
   SK_STATUS             err;
   sk_msg                req_msg;
   sk_msg                resp_msg;
   sk_open_session_req   *os_req;
   sk_open_session_resp  *os_resp;
   uint32_t               c_idx;
   tApiSessionTable      *st;
   uint32_t              i;

   SYSV_ASSERT(tObj != 0);

   if (tObj == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   /*
     Open Session Command
   */
   SYSV_INFO0(tObj->log, "Tapi: Open Session");

   SYSV_ASSERT(tObj->conn.num_ctable_elements <= SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT);
   SYSV_ASSERT(tObj->conn.num_stable_elements <= SK_TAPI_MAX_SESSIONS_PER_CONTEXT);

   if ((err = sk_tapi_open_session_unpack(tObj->mrecv, tObj->mrecv_size, &req_msg, SK_TAPI_MSG_REQ_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }


   resp_msg.hdr = req_msg.hdr;
   os_req       = &req_msg.type.open_session.req;
   os_resp      = &resp_msg.type.open_session.resp;

   /*
      find the context object index based on the handle
   */
   if ((err = sk_swld_tapi_handle_generator_plugin_find_context_handle(&tObj->hp,
                                                               SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT,
                                                               tObj->conn.cthandles,
                                                               os_req->context_handle,
                                                               SK_TAPI_CONTEXT_HANDLE_SIZE,
                                                               &c_idx)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Failed to find context handle [%d]", err);

      os_resp->status = SK_ERROR_CONTEXT_HANDLE_NOT_FOUND;
      goto RESPOND;
   }

   c_idx /= SK_TAPI_CONTEXT_HANDLE_SIZE;


   SYSV_ASSERT(c_idx                           < SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT);
   SYSV_ASSERT(tObj->conn.ctable[c_idx].active == 1);

   /*
     Check if we can add another session
   */
   if (tObj->conn.ctable[c_idx].num_connected_sessions >= SK_TAPI_MAX_SESSIONS_PER_CONTEXT)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Failed to session count exceeded [%d]",
                  tObj->conn.ctable[c_idx].num_connected_sessions);
      os_resp->status = SK_ERROR_OPEN_SESSION;
      goto RESPOND;
   }

   /*
      space is available now find a free spot
   */
   for (i=0; i<SK_TAPI_MAX_SESSIONS_PER_CONTEXT;i++)
   {
     if (tObj->conn.stable[i].active == 0)
     {
        /*
           we found it a free spot
        */
        break;
     }
   }

   /*
      set the object
   */
   if ((err = sk_swld_tapi_handle_generator_plugin_generate_session_handle(&tObj->hp,
                                                                   tObj->conn.stable[i].session_handle,
                                                                   SK_TAPI_SESSION_HANDLE_SIZE)) != SK_TAPI_SESSION_HANDLE_SIZE)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Failed to generate session handle [%d]", err);

      /* fail process since this should never happen */
      err = SK_ERROR_FAILED_GEN_HANDLE;
      goto DONE;
   }

   SYSV_LDUMP(tObj->log, "tapi: session handle", tObj->conn.stable[i].session_handle, SK_TAPI_SESSION_HANDLE_SIZE);

   /*
      provide the TIME access
   */
   tObj->conn.ctable[c_idx].time_access = sk_drv_swld_secure_timer_get();
   tObj->conn.stable[i].time_access     = sk_drv_swld_secure_timer_get();

   /*
     configure the session object
   */
   tObj->conn.stable[i].active = 1;
   tObj->conn.stable[i].next   = 0;
   SYSV_MEMCPY(tObj->conn.stable[i].taid, os_req->taid, SK_TAPI_SESSION_TAID_SIZE);

   /*
     add it to the context object
   */
   tObj->conn.stable[i].next = 0;
   tObj->conn.stable[i].prev = 0;

   if (tObj->conn.ctable[c_idx].start != 0)
   {
      for (st = tObj->conn.ctable[c_idx].start; st->next != 0; st = st->next)
      {};

      st->next = &tObj->conn.stable[i];
      tObj->conn.stable[i].prev = st;
   }
   else
   {
      /*
         establish the first link
      */
      tObj->conn.ctable[c_idx].start = &tObj->conn.stable[i];
   }

   SYSV_ASSERT(tObj->conn.ctable[c_idx].num_connected_sessions < SK_TAPI_MAX_SESSIONS_PER_CONTEXT);
   tObj->conn.ctable[c_idx].num_connected_sessions++;

   SYSV_ASSERT(tObj->conn.num_stable_elements < SK_TAPI_MAX_SESSIONS_PER_CONTEXT);
   tObj->conn.num_stable_elements++;

   /*
     Now load (start) the TApp
   */
   if ((err = tapp_load(os_req->taid, &tObj->conn.stable[i].tapp, tObj->log)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Failed loading TApp [%d]", err);
      os_resp->status = SK_ERROR_TAPP_LOAD;
      goto RESPOND;
   }

   /*
     Initialize the TApp
   */
   if ((err = sk_swld_platform_tapp_init(&tObj->conn.stable[i].tapp,
                                                    tObj->conn.stable[i].tapp_session,
                                                    tObj->log,
                                                    TAPI_TAPP_INIT_IFACE_TIMEOUT_MS)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error Initializing TApp [%d]", err);

      if (err != SK_ERROR_TIMEOUT)
      {
         os_resp->status = SK_ERROR_TAPP_CALL;
      }
      else
      {
         os_resp->status = SK_ERROR_TAPP_TIMEOUT;
      }
   }
   else
   {
      /*
         Send Response Status
      */
      SYSV_MEMCPY(os_resp->session_handle, tObj->conn.stable[i].session_handle, SK_TAPI_SESSION_HANDLE_SIZE);

      os_resp->status = SK_ERROR_SUCCESS;
   }


RESPOND:
   tObj->msend_size = tObj->max_msend_size;
   if ((err = sk_tapi_open_session_pack(&resp_msg, tObj->msend, &tObj->msend_size, SK_TAPI_MSG_RESP_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }

DONE:

   /* wipe */
   SYSV_MEMSET(&req_msg,  0, sizeof(sk_msg));
   SYSV_MEMSET(&resp_msg, 0, sizeof(sk_msg));

   return err;
}

SK_STATUS sk_swld_tapi_close_session_nw(sk_swld_tapi* tObj)
{
   SK_STATUS             err;
   sk_msg                req_msg;
   sk_msg                resp_msg;
   sk_close_session_req  *cs_req;
   sk_close_session_resp *cs_resp;
   uint32_t               c_idx;
   uint32_t               s_idx;

   SYSV_ASSERT(tObj != 0);

   if (tObj == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   /*
     Close Session
   */
   SYSV_INFO0(tObj->log, "Tapi: Close Session");

   SYSV_ASSERT(tObj->conn.num_ctable_elements <= SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT);
   SYSV_ASSERT(tObj->conn.num_stable_elements <= SK_TAPI_MAX_SESSIONS_PER_CONTEXT);

   if ((err = sk_tapi_close_session_unpack(tObj->mrecv, tObj->mrecv_size, &req_msg, SK_TAPI_MSG_REQ_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }


   resp_msg.hdr = req_msg.hdr;
   cs_req       = &req_msg.type.close_session.req;
   cs_resp      = &resp_msg.type.close_session.resp;

   SYSV_LDUMP(tObj->log, "tapi: context handle", cs_req->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
   SYSV_LDUMP(tObj->log, "tapi: session handle", cs_req->session_handle, SK_TAPI_SESSION_HANDLE_SIZE);

   if ((err = sk_swld_tapi_find_objects(tObj,
                                cs_req->context_handle, &c_idx,
                                cs_req->session_handle, &s_idx)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Failed to find/verify handle [%d]", err);
      switch(err)
      {
         case SK_ERROR_CONTEXT_HANDLE_NOT_FOUND:
            cs_resp->status = SK_ERROR_CONTEXT_HANDLE_NOT_FOUND;
            err             = SK_ERROR_SUCCESS;
            break;
         case SK_ERROR_SESSION_HANDLE_NOT_FOUND:
            cs_resp->status = SK_ERROR_SESSION_HANDLE_NOT_FOUND;
            err             = SK_ERROR_SUCCESS;
            break;
         default: /* SK_ERROR_VERIFY_FAILED */
            break;
      }

      if (err != SK_ERROR_SUCCESS)
      {
         goto DONE;
      }
      else
      {
        goto RESPOND;
      }
   }

   /*
      update time access
   */
   tObj->conn.ctable[c_idx].time_access = sk_drv_swld_secure_timer_get();

   /*
     End the TApp
   */
   if ((err = sk_swld_platform_tapp_close(&tObj->conn.stable[s_idx].tapp,
                                                     tObj->conn.stable[s_idx].tapp_session,
                                                     tObj->log,
                                                     TAPI_TAPP_CLOSE_IFACE_TIMEOUT_MS)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Warning: Error closing the TApp [%d]", err);
   }

   /*
      Close the TApp. This is all or nothing so it must be shutdown here.
   */
   if ((err = tapp_unload(&tObj->conn.stable[s_idx].tapp, tObj->log)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Warning: Error unloading the TApp [%d]", err);
   }


#if 0
   /*
     Remove session
   */
   if ( tObj->conn.ctable[c_idx].start == (&tObj->conn.stable[s_idx]) )
   {
      /*
         this is the first element
      */
      if (tObj->conn.stable[s_idx].next != 0)
      {
        tObj->conn.ctable[c_idx].start       = tObj->conn.stable[s_idx].next;
        tObj->conn.ctable[c_idx].start->prev = 0;
      }
      else
      {
         /*
           this must be the last element, no more items
         */
         tObj->conn.ctable[c_idx].start = 0;
      }
   }
   else
   {
      /*
        this is element is not the first
      */
      /*
        set the "next" pointer from the previous element
      */
      ((tApiSessionTable *)tObj->conn.stable[s_idx].prev)->next = tObj->conn.stable[s_idx].next;

      if (tObj->conn.stable[s_idx].next != 0)
      {
         /*
           set the "prev" pointer from the next element
         */
         ((tApiSessionTable *)tObj->conn.stable[s_idx].next)->prev = tObj->conn.stable[s_idx].prev;
      }
   }

   tObj->conn.stable[s_idx].active = 0;
   tObj->conn.stable[s_idx].next   = 0;
   tObj->conn.stable[s_idx].prev   = 0;
   SYSV_MEMSET(tObj->conn.stable[s_idx].taid, 0, SK_TAPI_SESSION_TAID_SIZE);

   SYSV_ASSERT(tObj->conn.stable[s_idx].idx == s_idx);
   SYSV_ASSERT(tObj->conn.ctable[c_idx].num_connected_sessions > 0);
   tObj->conn.ctable[c_idx].num_connected_sessions--;

   SYSV_ASSERT(tObj->conn.num_stable_elements > 0);
   tObj->conn.num_stable_elements--;
#else

   SYSV_ASSERT(tObj->conn.stable[s_idx].idx == s_idx);
   sk_swld_tapi_remove_session(tObj, &tObj->conn.ctable[c_idx], &tObj->conn.stable[s_idx]);
#endif

   cs_resp->status = SK_ERROR_SUCCESS;

#ifdef SWLD_USER_DEBUG
   {
      tApiSessionTable *st;
      uint32_t ik;

      ik = 1;
      for (st = tObj->conn.ctable[c_idx].start; st != 0; st = st->next, ik++)
      {
         SYSV_INFO2(tObj->log, "[%d] st->idx = %d\n", ik, st->idx);
      }
      SYSV_INFO1(tObj->log, "removed s_idx = %d\n", s_idx);
      SYSV_INFO1(tObj->log, "tObj->conn.num_stable_elements = %d\n", tObj->conn.num_stable_elements);
   }
#endif


RESPOND:
   tObj->msend_size = tObj->max_msend_size;
   if ((err = sk_tapi_close_session_pack(&resp_msg, tObj->msend, &tObj->msend_size, SK_TAPI_MSG_RESP_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }


DONE:
   /* wipe */
   SYSV_MEMSET(&req_msg,  0, sizeof(sk_msg));
   SYSV_MEMSET(&resp_msg, 0, sizeof(sk_msg));

   return err;
}

SK_STATUS sk_swld_tapi_invoke_command_nw(sk_swld_tapi* tObj)
{
   SK_STATUS              err;
   sk_msg                 req_msg;
   sk_msg                 resp_msg;
   sk_invoke_cmd_req      *ic_req;
   sk_invoke_cmd_resp     *ic_resp;
   uint32_t                c_idx;
   uint32_t                s_idx;
   sk_operation       op;

   SYSV_ASSERT(tObj != 0);

   if (tObj == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   /*
     Invoke Command
   */
   SYSV_INFO0(tObj->log, "Tapi: Invoke Command");

   SYSV_ASSERT(tObj->conn.num_ctable_elements <= SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT);
   SYSV_ASSERT(tObj->conn.num_stable_elements <= SK_TAPI_MAX_SESSIONS_PER_CONTEXT);

   if ((err = sk_tapi_invoke_command_unpack(tObj->mrecv, tObj->mrecv_size, &req_msg, SK_TAPI_MSG_REQ_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }


   resp_msg.hdr = req_msg.hdr;
   ic_req       = &req_msg.type.invoke_cmd.req;
   ic_resp      = &resp_msg.type.invoke_cmd.resp;

   SYSV_LDUMP(tObj->log, "tapi: context handle", ic_req->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
   SYSV_LDUMP(tObj->log, "tapi: session handle", ic_req->session_handle, SK_TAPI_SESSION_HANDLE_SIZE);

   if ((err = sk_swld_tapi_find_objects(tObj,
                                ic_req->context_handle, &c_idx,
                                ic_req->session_handle, &s_idx)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Failed to find/verify handle [%d]", err);
      switch(err)
      {
         case SK_ERROR_CONTEXT_HANDLE_NOT_FOUND:
            ic_resp->status = SK_ERROR_CONTEXT_HANDLE_NOT_FOUND;
            err             = SK_ERROR_SUCCESS;
            break;
         case SK_ERROR_SESSION_HANDLE_NOT_FOUND:
            ic_resp->status = SK_ERROR_SESSION_HANDLE_NOT_FOUND;
            err             = SK_ERROR_SUCCESS;
            break;
         default: /* SK_ERROR_VERIFY_FAILED */
            break;
      }

      if (err != SK_ERROR_SUCCESS)
      {
         goto DONE;
      }
      else
      {
        goto RESPOND;
      }
   }

   /*
      update the time access
   */
   tObj->conn.ctable[c_idx].time_access = sk_drv_swld_secure_timer_get();
   tObj->conn.stable[s_idx].time_access = sk_drv_swld_secure_timer_get();

   /*
     copy memory to stack memory (safety)
   */
   SYSV_MEMCPY(&op, &tObj->op, sizeof(sk_operation));

   /*
      Call the TApp
   */
   if ((err = sk_swld_platform_tapp_pull(&tObj->conn.stable[s_idx].tapp,
                                                                 tObj->conn.stable[s_idx].tapp_session,
                                                                 ic_req->cmd,
                                                                 &op,
                                                                 tObj->log,
                                                                 TAPI_TAPP_RECVOP_IFACE_TIMEOUT_MS)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error calling TApp [%d]", err);

      if (err != SK_ERROR_TIMEOUT)
      {
         ic_resp->status = SK_ERROR_TAPP_CALL;
      }
      else
      {
         ic_resp->status = SK_ERROR_TAPP_TIMEOUT;
      }
   }
   else
   {
      ic_resp->status = SK_ERROR_SUCCESS;
   }

RESPOND:
   tObj->msend_size = tObj->max_msend_size;
   if ((err = sk_tapi_invoke_command_pack(&resp_msg, tObj->msend, &tObj->msend_size, SK_TAPI_MSG_RESP_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }


DONE:

   /* wipe */
   SYSV_MEMSET(&req_msg,  0, sizeof(sk_msg));
   SYSV_MEMSET(&resp_msg, 0, sizeof(sk_msg));

   return err;
}

SK_STATUS sk_swld_tapi_register_memory_nw(sk_swld_tapi* tObj)
{
   SK_STATUS                      err;
   sk_msg                         req_msg;
   sk_msg                         resp_msg;
   sk_register_memory_req         *ms_req;
   sk_register_memory_resp        *ms_resp;
   uint32_t                        c_idx;
   uint32_t                       memory_id_length;
   tApiRegMemory                  *mem;
   tApiRegMemory                  *mptr;

   SYSV_ASSERT(tObj != 0);

   if (tObj == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   /*
     Register Memory Command
   */
   SYSV_INFO0(tObj->log, "Tapi: Memory Registration");

   SYSV_ASSERT(tObj->conn.num_ctable_elements <= SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT);
   SYSV_ASSERT(tObj->conn.num_stable_elements <= SK_TAPI_MAX_SESSIONS_PER_CONTEXT);

   if ((err = sk_tapi_register_memory_unpack(tObj->mrecv, tObj->mrecv_size, &req_msg, SK_TAPI_MSG_REQ_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }


   resp_msg.hdr = req_msg.hdr;
   ms_req       = &req_msg.type.register_memory.req;
   ms_resp      = &resp_msg.type.register_memory.resp;

   SYSV_LDUMP(tObj->log, "tapi: context handle", ms_req->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);

   /*
      find the context object index based on the handle
   */
   if ((err = sk_swld_tapi_handle_generator_plugin_find_context_handle(&tObj->hp,
                                                               SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT,
                                                               tObj->conn.cthandles,
                                                               ms_req->context_handle,
                                                               SK_TAPI_CONTEXT_HANDLE_SIZE,
                                                               &c_idx)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Failed to find context handle [%d]", err);
      err = SK_ERROR_CONTEXT_HANDLE_NOT_FOUND;
      goto DONE;
   }

   /*
      update time access
   */
   tObj->conn.ctable[c_idx].time_access = sk_drv_swld_secure_timer_get();

   /*
      allocate memory for the memory region table
   */
   if ((mem = SYSV_MALLOC(sizeof(tApiRegMemory))) == 0)
   {
      SYSV_ERROR0(tObj->log, SYSV_LOG_CRITICAL_ERR, "Unable to allocate memory");
      err = SK_ERROR_NO_MEMORY;
      goto DONE;
   }

   /*
     Register memory
   */
   mem->next        = 0;
   mem->prev        = 0;
   memory_id_length = SK_TAPI_MEMORY_ID_LENGTH;
   mem->memory_size = ms_req->memory_size;
   mem->flag        = ms_req->flag;

   if (mem->flag != SKTEEC_MEM_ALLOCATE)
   {
      mem->mem_ptr  = (void*)ms_req->addr;
   }
   else
   {
      mem->mem_ptr = 0;
   }

   if ((err = sk_drv_swld_memory_config_allocate_memory(mem->mem_ptr,
                                                        mem->memory_size,
                                                        mem->flag,
                                                        mem->id,
                                                        &memory_id_length,
                                                        mem->info,
                                                        tObj->log)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error allocating memory [%d]", err);
      err = SK_ERROR_FAILED_SHARED_MEMORY_REQ;
      SYSV_FREE(mem);
      goto DONE;
   }

   SYSV_ASSERT(memory_id_length == SK_TAPI_MEMORY_ID_LENGTH);

   /*
      add to memory
   */
   mptr = tObj->rmem.m;
   if (mptr != 0)
   {
      while(mptr->next != 0)
      {
         mptr = mptr->next;
      }

      mptr->next = mem;
      mem->prev = mptr;
   }
   else
   {
      tObj->rmem.m = mem;
   }

   tObj->rmem.mem_region_count++;

   SYSV_LDUMP(tObj->log, "memory id:", mem->id, SK_TAPI_MEMORY_ID_LENGTH);
   SYSV_INFO1(tObj->log, "mem_region_count [%d]", tObj->rmem.mem_region_count);

   ms_resp->addr = (SK_ADDRESS_TYPE)mem->mem_ptr;
   SYSV_MEMCPY(ms_resp->memory_id, mem->id, SK_TAPI_MEMORY_ID_LENGTH);

   ms_resp->status = SK_ERROR_SUCCESS;

   tObj->msend_size = tObj->max_msend_size;
   if ((err = sk_tapi_register_memory_pack(&resp_msg, tObj->msend, &tObj->msend_size, SK_TAPI_MSG_RESP_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }


DONE:

   /* wipe */
   SYSV_MEMSET(&req_msg,  0, sizeof(sk_msg));
   SYSV_MEMSET(&resp_msg, 0, sizeof(sk_msg));

   return err;
}

SK_STATUS sk_swld_tapi_unregister_memory_nw(sk_swld_tapi* tObj)
{
   SK_STATUS                      err;
   sk_msg                         req_msg;
   sk_msg                         resp_msg;
   sk_unregister_memory_req       *unms_req;
   sk_unregister_memory_resp      *unms_resp;
   uint32_t                        c_idx;
   tApiRegMemory                  *mptr;

   SYSV_ASSERT(tObj != 0);

   if (tObj == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   /*
     Register Memory Command
   */
   SYSV_INFO0(tObj->log, "Tapi: Memory De-registration");

   SYSV_ASSERT(tObj->conn.num_ctable_elements <= SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT);
   SYSV_ASSERT(tObj->conn.num_stable_elements <= SK_TAPI_MAX_SESSIONS_PER_CONTEXT);

   if ((err = sk_tapi_unregister_memory_unpack(tObj->mrecv, tObj->mrecv_size, &req_msg, SK_TAPI_MSG_REQ_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }


   resp_msg.hdr = req_msg.hdr;
   unms_req     = &req_msg.type.unregister_memory.req;
   unms_resp    = &resp_msg.type.unregister_memory.resp;

   SYSV_LDUMP(tObj->log, "tapi: context handle", unms_req->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);

   /*
      find the context object index based on the handle
   */
   if ((err = sk_swld_tapi_handle_generator_plugin_find_context_handle(&tObj->hp,
                                                               SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT,
                                                               tObj->conn.cthandles,
                                                               unms_req->context_handle,
                                                               SK_TAPI_CONTEXT_HANDLE_SIZE,
                                                               &c_idx)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Failed to find context handle [%d]", err);
      err = SK_ERROR_CONTEXT_HANDLE_NOT_FOUND;
      goto DONE;
   }

   /*
      update time access
   */
   tObj->conn.ctable[c_idx].time_access = sk_drv_swld_secure_timer_get();

   /*
     Find the registered memory list
   */
   for(mptr = tObj->rmem.m; mptr != 0; mptr = mptr->next)
   {
      if (SYSV_MEMCMP(mptr->id, unms_req->memory_id, SK_TAPI_MEMORY_ID_LENGTH) == 0)
      {
         SYSV_LDUMP(tObj->log, "mptr->id", mptr->id, SK_TAPI_MEMORY_ID_LENGTH);
         SYSV_INFO1(tObj->log, "mptr->memory_size [%d]", mptr->memory_size);
         break;
      }
   }

   SYSV_INFO1(tObj->log, "mem_region_count [%d]", tObj->rmem.mem_region_count);

   if (mptr == 0)
   {
      SYSV_ERROR0(tObj->log, SYSV_LOG_CRITICAL_ERR, "Memory not found");
      err = SK_ERROR_NO_MEMORY;
      goto DONE;
   }


   if (tObj->rmem.m == mptr)
   {
      if (mptr->next != 0)
      {
        tObj->rmem.m       = mptr->next;
        tObj->rmem.m->prev = 0;
      }
      else
      {
         /*
           this must be the last element, no more items
         */
         tObj->rmem.m = 0;
      }
   }
   else
   {
      /*
        this is element is not the first
      */
      /*
        set the "next" pointer from the previous element
      */
      ((tApiRegMemory *)mptr->prev)->next = mptr->next;

      if (mptr->next != 0)
      {
         /*
           set the "prev" pointer from the next element
         */
         ((tApiRegMemory *)mptr->next)->prev = mptr->prev;
      }
   }

   /*
   {
      uint32_t      i;
      tApiRegMemory *m;
      m = tObj->rmem.m;
      while (m != 0)
      {
         printf("m->flag = %d\n", m->flag);
         m = m->next;
      }
   }
   */

   SYSV_ASSERT(tObj->rmem.mem_region_count > 0);
   tObj->rmem.mem_region_count--;

   if ((err = sk_drv_swld_memory_config_free_memory(mptr->mem_ptr,
// this parameter doesn't seem to be in the header
//                                                    mptr->id,
                                                    mptr->info,
                                                    tObj->log)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error free memory [%d]", err);
      err = SK_ERROR_FAILED_SHARED_MEMORY_REQ;
      goto DONE;
   }

   SYSV_FREE(mptr);

   unms_resp->status = SK_ERROR_SUCCESS;

   tObj->msend_size = tObj->max_msend_size;
   if ((err = sk_tapi_unregister_memory_pack(&resp_msg, tObj->msend, &tObj->msend_size, SK_TAPI_MSG_RESP_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }


DONE:

   /* wipe */
   SYSV_MEMSET(&req_msg,  0, sizeof(sk_msg));
   SYSV_MEMSET(&resp_msg, 0, sizeof(sk_msg));

   return err;
}

SK_STATUS sk_swld_tapi_check_nw(sk_swld_tapi* tObj)
{
   SK_STATUS         err;
   uint32_t          c;
   tApiSessionTable  *sess;
   uint32_t           diff_time;
   sk_msg            req_msg;
   sk_msg            resp_msg;
//   sk_check_req      *ck_req;
   sk_check_resp     *ck_resp;

   SYSV_ASSERT(tObj != 0);

   if (tObj == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   /*
     Check Command
   */
   SYSV_INFO0(tObj->log, "Tapi: Check System");

   if ((err = sk_tapi_check_unpack(tObj->mrecv, tObj->mrecv_size, &req_msg, SK_TAPI_MSG_REQ_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }


   resp_msg.hdr = req_msg.hdr;
//   ck_req       = &req_msg.type.sys_check.req;
   ck_resp      = &resp_msg.type.sys_check.resp;

   for (c = 0; c < SK_TAPI_MAX_CONNECTIONS_PER_CONTEXT; c++)
   {
      if (tObj->conn.ctable[c].active == 1)
      {
         sess = tObj->conn.ctable[c].start;

         do
         {
            diff_time = sk_drv_swld_secure_timer_diff(sess->time_access, sk_drv_swld_secure_timer_get());

            if (diff_time > TAPI_SESSION_TIMEOUT_MS)
            {
               /*
                  expire session
               */
               sk_swld_tapi_remove_session(tObj, &tObj->conn.ctable[c], sess);
            }
            else
            {
               sess = sess->next;
            }
         }while(sess != 0);

         diff_time = sk_drv_swld_secure_timer_diff(tObj->conn.ctable[c].time_access, sk_drv_swld_secure_timer_get());

         if (diff_time > TAPI_CONTEXT_TIMEOUT_MS)
         {
            /*
               expire context
            */
            sk_swld_tapi_remove_context(tObj, &tObj->conn.ctable[c], 1);
         }

      }
   }

   ck_resp->status = SK_ERROR_SUCCESS;

   tObj->msend_size = tObj->max_msend_size;
   if ((err = sk_tapi_check_pack(&resp_msg, tObj->msend, &tObj->msend_size, SK_TAPI_MSG_RESP_TYPE)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObj->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }


DONE:
   return err;
}







