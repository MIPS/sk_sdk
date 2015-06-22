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

/*
  Internal Functions
*/


/*
  API
*/
SK_STATUS sk_swld_tapi_message_interface_init(tApiMsgIntObj* tObjParam,
                                      uint32_t num_context_ids,
                                      uint32_t* context_ids,
                                      tApiMsgSendInterface* msg_context_interface[SK_TAPI_MAX_CONTEXT_IDS],
                                      tApiMsgDriverRecvInterface* msg_rx_interface,
                                      uint32_t max_msg_size)
{
   SK_STATUS err;
   uint32_t  i;

   if ((tObjParam == 0)        ||
       (context_ids == 0)      ||
       (num_context_ids == 0)  ||
       (msg_rx_interface == 0) ||
       (max_msg_size == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   if (num_context_ids > SK_TAPI_MAX_CONTEXT_IDS)
   {
      return SK_ERROR_OUT_OF_RANGE;
   }

   err = SK_ERROR_SUCCESS;

   SYSV_MEMSET(tObjParam, 0, sizeof(tObjParam));

   /*
      set the number of Context ID's passed to the system
   */
   tObjParam->num_context_ids = num_context_ids;

   /*
      set the context ID's
   */
   for(i = 0; i < num_context_ids; i++)
   {
      tObjParam->context_ids[i]     = context_ids[i];
      tObjParam->mi_send_context[i] = msg_context_interface[i];
   }

   /*
     receiver driver
   */
   tObjParam->mi_rx_intf = msg_rx_interface;

   /*
     message buffer
   */
   tObjParam->max_msg_size = max_msg_size;

   if ((tObjParam->msg = SYSV_MALLOC(max_msg_size)) == 0)
   {
      err = SK_ERROR_NO_MEMORY;
      goto DONE;
   }

DONE:
   return err;
}

SK_STATUS sk_swld_tapi_message_interface_end(tApiMsgIntObj* tObjParam)
{
   SK_STATUS err;

   if (tObjParam == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   err = SK_ERROR_SUCCESS;

   if (tObjParam->msg != 0)
   {
      SYSV_FREE(tObjParam->msg);
   }

   return err;
}


SK_STATUS sk_swld_tapi_message_interface_process(tApiMsgIntObj* tObjParam)
{
   SK_STATUS             err;
   uint32_t              msg_size;
   sk_header             hdr;
   uint32_t              i;
   int32_t               found_context;
   tApiMsgSendInterface* context_send_interface;

   if (tObjParam == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   msg_size      = tObjParam->max_msg_size;
   found_context = -1;

   SYSV_ASSERT(tObjParam->mi_rx_intf != 0);


   if ((err = tObjParam->mi_rx_intf->tapi_recv_msg(tObjParam->mi_rx_intf->mObj, tObjParam->msg, &msg_size)) != 0)
   {
      SYSV_ERROR1(tObjParam->log, SYSV_LOG_CRITICAL_ERR, "Error: Recv Message failed [%d]", err);
      err = SK_ERROR_RECV_COMM;
      goto DONE;
   }

   if ((err = sk_tapi_header_unpack(tObjParam->msg, msg_size, &hdr)) != SKTAPIPACKERR_SUCCESS) {
      SYSV_ERROR1(tObjParam->log, SYSV_LOG_CRITICAL_ERR, "Error packing context: %d", err);
      err = SK_ERROR_INVALID_PARAM;
      goto DONE;
   }


   /*
      check if the Context ID is registered with the system
   */
   for(i = 0; i < tObjParam->num_context_ids; i++)
   {
      if (hdr.context_id == tObjParam->context_ids[i])
      {
         found_context = i;
         break;
      }
   }

   if (found_context == -1)
   {
      SYSV_ERROR0(tObjParam->log, SYSV_LOG_CRITICAL_ERR, "Error: Context Not registered with SM");
      err = SK_ERROR_BAD_CONTEXT_ID;
      goto DONE;
   }

   SYSV_ASSERT((found_context >= 0) && (found_context < SK_TAPI_MAX_CONTEXT_IDS));
   SYSV_ASSERT(tObjParam->mi_send_context[found_context] != 0);

   context_send_interface = tObjParam->mi_send_context[found_context];

   SYSV_ASSERT(context_send_interface->tapi_send_msg_to_context != 0);

   if ((err = context_send_interface->tapi_send_msg_to_context(context_send_interface->mObj, tObjParam->msg, msg_size)) != 0)
   {
      SYSV_ERROR1(tObjParam->log, SYSV_LOG_CRITICAL_ERR, "Error: Error sending message to Context [%d]", err);
      err = SK_ERROR_SEND_COMM;
      goto DONE;
   }

   /*
      the receive from context is optional
   */
   if (context_send_interface->tapi_recv_msg_from_context != 0)
   {
      msg_size = tObjParam->max_msg_size;

      if ((err = context_send_interface->tapi_recv_msg_from_context(context_send_interface->mObj, tObjParam->msg, &msg_size)) != 0)
      {
         SYSV_ERROR1(tObjParam->log, SYSV_LOG_CRITICAL_ERR, "Error: Error receiving message from Context [%d]", err);
         err = SK_ERROR_RECV_FROM_CONTEXT;
         goto DONE;
      }
   }

   /*
      the sending of the message is optional
   */
   if (tObjParam->mi_rx_intf->tapi_send_msg != 0)
   {
      if ((err = tObjParam->mi_rx_intf->tapi_send_msg(tObjParam->mi_rx_intf->mObj, tObjParam->msg, msg_size)) != 0)
      {
         SYSV_ERROR1(tObjParam->log, SYSV_LOG_CRITICAL_ERR, "Error: Sending Message failed [%d]", err);
         err = SK_ERROR_SEND_TO_DRIVER;
         goto DONE;
      }
   }

DONE:
   return err;
}








