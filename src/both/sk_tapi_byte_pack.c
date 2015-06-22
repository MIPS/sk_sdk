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

#define STORE(val) \
         cnt = sysv_byte_put(pack, byte_cnt, *pack_len, sizeof(val), val); \
         if (!cnt) { return SKTAPIPACKERR_FAILED; } \
         byte_cnt += cnt;

#define STOREBUF(val, len) \
         if (byte_cnt + len > *pack_len) { return SKTAPIPACKERR_FAILED; } \
         SYSV_MEMCPY(&pack[byte_cnt], val, len); \
         byte_cnt += len;

STATUS sk_tapi_sys_tapi_pack(const sk_msg* msg,
                             uint8_t* pack,
                             uint32_t* pack_len,
                             uint8_t type)
{
   STATUS             err;
   uint32_t           byte_cnt, cnt;
   const sk_tapi_resp *resp;

   if ((msg == 0) || (pack == 0) || (pack_len == 0))
   {
      return SKTAPIPACKERR_INVALID_PARAMS;
   }

   err      = SKTAPIPACKERR_SUCCESS;
   byte_cnt = 0;
   resp     = &msg->type.sys_tapi.resp;

   switch(type)
   {
      case SK_TAPI_SYSTEM:
         STORE(msg->hdr.context_id);
         STORE(msg->hdr.sm_id);
         STORE(msg->hdr.cmd);
         STORE(resp->status);
         break;
      default:
         err = SKTAPIPACKERR_INVALID_PARAMS;
         break;
   };

   *pack_len = byte_cnt;

   return err;
}

STATUS sk_tapi_open_context_pack(const sk_msg* msg,
                                 uint8_t* pack,
                                 uint32_t* pack_len,
                                 uint8_t type)
{
   STATUS                     err;
   uint32_t                   byte_cnt, cnt;
   const sk_open_context_req  *req;
   const sk_open_context_resp *resp;

   if ((msg == 0) || (pack == 0) || (pack_len == 0))
   {
      return SKTAPIPACKERR_INVALID_PARAMS;
   }

   err      = SKTAPIPACKERR_SUCCESS;
   byte_cnt = 0;
   req      = &msg->type.open_context.req;
   resp     = &msg->type.open_context.resp;

   switch(type)
   {
      case SK_TAPI_MSG_REQ_TYPE:
         STORE(msg->hdr.context_id);
         STORE(msg->hdr.sm_id);
         STORE(msg->hdr.cmd);
         STORE(req->context_id);
         break;
      default: /* SK_TAPI_MSG_RESP_TYPE */
         STORE(msg->hdr.context_id);
         STORE(msg->hdr.sm_id);
         STORE(msg->hdr.cmd);
         STOREBUF(resp->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
         STORE(resp->status);
         break;
   };

   *pack_len = byte_cnt;

   return err;
}

STATUS sk_tapi_close_context_pack(const sk_msg* msg,
                                  uint8_t* pack,
                                  uint32_t* pack_len,
                                  uint8_t type)
{
   STATUS                      err;
   uint32_t            byte_cnt, cnt;
   const sk_close_context_req  *req;
   const sk_close_context_resp *resp;

   if ((msg == 0) || (pack == 0))
   {
      return SKTAPIPACKERR_INVALID_PARAMS;
   }

   err      = SKTAPIPACKERR_SUCCESS;
   byte_cnt = 0;
   req      = &msg->type.close_context.req;
   resp     = &msg->type.close_context.resp;

   switch(type)
   {
      case SK_TAPI_MSG_REQ_TYPE:
         STORE(msg->hdr.context_id);
         STORE(msg->hdr.sm_id);
         STORE(msg->hdr.cmd);
         STOREBUF(req->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
         break;
      default: /* SK_TAPI_MSG_RESP_TYPE */
         STORE(msg->hdr.context_id);
         STORE(msg->hdr.sm_id);
         STORE(msg->hdr.cmd);
         STORE(resp->status);
         break;
   };

   *pack_len = byte_cnt;

   return err;
}

STATUS sk_tapi_open_session_pack(const sk_msg* msg,
                                 uint8_t* pack,
                                 uint32_t* pack_len,
                                 uint8_t type)
{
   STATUS                     err;
   uint32_t            byte_cnt, cnt;
   const sk_open_session_req  *req;
   const sk_open_session_resp *resp;

   if ((msg == 0) || (pack == 0) || (pack_len == 0))
   {
      return SKTAPIPACKERR_INVALID_PARAMS;
   }

   err      = SKTAPIPACKERR_SUCCESS;
   byte_cnt = 0;
   req      = &msg->type.open_session.req;
   resp     = &msg->type.open_session.resp;

   switch(type)
   {
      case SK_TAPI_MSG_REQ_TYPE:
         STORE(msg->hdr.context_id);
         STORE(msg->hdr.sm_id);
         STORE(msg->hdr.cmd);
         STOREBUF(req->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
         STOREBUF(req->taid, SK_TAPI_SESSION_TAID_SIZE);
         break;
      default: /* SK_TAPI_MSG_RESP_TYPE */
         STORE(msg->hdr.context_id);
         STORE(msg->hdr.sm_id);
         STORE(msg->hdr.cmd);
         STOREBUF(resp->session_handle, SK_TAPI_SESSION_HANDLE_SIZE);
         STORE(resp->status);
         break;
   };

   *pack_len = byte_cnt;

   return err;
}

STATUS sk_tapi_close_session_pack(const sk_msg* msg,
                                  uint8_t* pack,
                                  uint32_t* pack_len,
                                  uint8_t type)
{
   STATUS                      err;
   uint32_t            byte_cnt, cnt;
   const sk_close_session_req  *req;
   const sk_close_session_resp *resp;

   if ((msg == 0) || (pack == 0) || (pack_len == 0))
   {
      return SKTAPIPACKERR_INVALID_PARAMS;
   }

   err      = SKTAPIPACKERR_SUCCESS;
   byte_cnt = 0;
   req      = &msg->type.close_session.req;
   resp     = &msg->type.close_session.resp;

   switch(type)
   {
      case SK_TAPI_MSG_REQ_TYPE:
         STORE(msg->hdr.context_id);
         STORE(msg->hdr.sm_id);
         STORE(msg->hdr.cmd);
         STOREBUF(req->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
         STOREBUF(req->session_handle, SK_TAPI_SESSION_HANDLE_SIZE);
         break;
      default: /* SK_TAPI_MSG_RESP_TYPE */
         STORE(msg->hdr.context_id);
         STORE(msg->hdr.sm_id);
         STORE(msg->hdr.cmd);
         STORE(resp->status);
         break;
   };

   *pack_len = byte_cnt;

   return err;
}

STATUS sk_tapi_invoke_command_pack(const sk_msg* msg,
                                   uint8_t* pack,
                                   uint32_t* pack_len,
                                   uint8_t type)
{
   STATUS                   err;
   uint32_t            byte_cnt, cnt;
   const sk_invoke_cmd_req  *req;
   const sk_invoke_cmd_resp *resp;

   if ((msg == 0) || (pack == 0) || (pack_len == 0))
   {
      return SKTAPIPACKERR_INVALID_PARAMS;
   }

   err      = SKTAPIPACKERR_SUCCESS;
   byte_cnt = 0;
   req      = &msg->type.invoke_cmd.req;
   resp     = &msg->type.invoke_cmd.resp;

   switch(type)
   {
      case SK_TAPI_MSG_REQ_TYPE:
         STORE(msg->hdr.context_id);
         STORE(msg->hdr.sm_id);
         STORE(msg->hdr.cmd);
         STOREBUF(req->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
         STOREBUF(req->session_handle, SK_TAPI_SESSION_HANDLE_SIZE);
         STORE(req->cmd);
         break;
      default: /* SK_TAPI_MSG_RESP_TYPE */
         STORE(msg->hdr.context_id);
         STORE(msg->hdr.sm_id);
         STORE(msg->hdr.cmd);
         STORE(resp->status);
         break;
   };

   *pack_len = byte_cnt;

   return err;
}

STATUS sk_tapi_register_memory_pack(const sk_msg* msg,
                                    uint8_t* pack,
                                    uint32_t* pack_len,
                                    uint8_t type)
{
   STATUS                        err;
   uint32_t            byte_cnt, cnt;
   const sk_register_memory_req  *req;
   const sk_register_memory_resp *resp;

   if ((msg == 0) || (pack == 0) || (pack_len == 0))
   {
      return SKTAPIPACKERR_INVALID_PARAMS;
   }

   err      = SKTAPIPACKERR_SUCCESS;
   byte_cnt = 0;
   req      = &msg->type.register_memory.req;
   resp     = &msg->type.register_memory.resp;

   switch(type)
   {
      case SK_TAPI_MSG_REQ_TYPE:
         STORE(msg->hdr.context_id);
         STORE(msg->hdr.sm_id);
         STORE(msg->hdr.cmd);
         STOREBUF(req->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
         STORE(req->memory_size);
         STORE(req->flag);
         STORE(req->addr);
         break;
      default: /* SK_TAPI_MSG_RESP_TYPE */
         STORE(msg->hdr.context_id);
         STORE(msg->hdr.sm_id);
         STORE(msg->hdr.cmd);
         STORE(resp->addr);
         STOREBUF(resp->memory_id, SK_TAPI_MEMORY_ID_LENGTH);
         STORE(resp->status);
         break;
   };

   *pack_len = byte_cnt;

   return err;
}

STATUS sk_tapi_unregister_memory_pack(const sk_msg* msg,
                                      uint8_t* pack,
                                      uint32_t* pack_len,
                                      uint8_t type)
{
   STATUS                          err;
   uint32_t            byte_cnt, cnt;
   const sk_unregister_memory_req  *req;
   const sk_unregister_memory_resp *resp;

   if ((msg == 0) || (pack == 0) || (pack_len == 0))
   {
      return SKTAPIPACKERR_INVALID_PARAMS;
   }

   err      = SKTAPIPACKERR_SUCCESS;
   req      = &msg->type.unregister_memory.req;
   resp     = &msg->type.unregister_memory.resp;
   byte_cnt = 0;

   switch(type)
   {
      case SK_TAPI_MSG_REQ_TYPE:
         STORE(msg->hdr.context_id);
         STORE(msg->hdr.sm_id);
         STORE(msg->hdr.cmd);
         STOREBUF(req->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
         STOREBUF(req->memory_id, SK_TAPI_MEMORY_ID_LENGTH);
         break;
      default: /* SK_TAPI_MSG_RESP_TYPE */
         STORE(msg->hdr.context_id);
         STORE(msg->hdr.sm_id);
         STORE(msg->hdr.cmd);
         STORE(resp->status);
         break;
   };

   *pack_len = byte_cnt;

   return err;
}

STATUS sk_tapi_check_pack(const sk_msg* msg,
                          uint8_t* pack,
                          uint32_t* pack_len,
                          uint8_t type)
{
   STATUS              err;
   uint32_t            byte_cnt, cnt;
   const sk_check_resp *resp;

   if ((msg == 0) || (pack == 0) || (pack_len == 0))
   {
      return SKTAPIPACKERR_INVALID_PARAMS;
   }

   resp      = &msg->type.sys_check.resp;
   err       = SKTAPIPACKERR_SUCCESS;
   byte_cnt  = 0;

   switch(type)
   {
      case SK_TAPI_MSG_REQ_TYPE:
         STORE(msg->hdr.context_id);
         STORE(msg->hdr.sm_id);
         STORE(msg->hdr.cmd);
         break;
      default: /* SK_TAPI_MSG_RESP_TYPE */
         STORE(msg->hdr.context_id);
         STORE(msg->hdr.sm_id);
         STORE(msg->hdr.cmd);
         STORE(resp->status);
         break;
   };

   *pack_len = byte_cnt;

   return err;
}
