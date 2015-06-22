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

#define READ(x) \
   if (byte_cnt + sizeof(x) > pack_len) { return SKTAPIPACKERR_FAILED; } \
   x = sysv_byte_get(&pack[byte_cnt], sizeof(x)); \
   byte_cnt += sizeof(x);


#define READBUF(x, s) \
   if (byte_cnt + s > pack_len) { return SKTAPIPACKERR_FAILED; } \
   SYSV_MEMCPY(x, &pack[byte_cnt], s); \
   byte_cnt           += s;


STATUS sk_tapi_header_unpack(const uint8_t*  pack,
                             uint32_t pack_len,
                             sk_header* hdr)
{
   uint32_t byte_cnt;

   if ((pack == 0) || (hdr == 0))
   {
      return SKTAPIPACKERR_INVALID_PARAMS;
   }

   byte_cnt = 0;
   READ(hdr->context_id);
   READ(hdr->sm_id);
   READ(hdr->cmd);

   return SKTAPIPACKERR_SUCCESS;
}

STATUS sk_tapi_sys_tapi_unpack(const uint8_t* pack,
                               uint32_t pack_len,
                               sk_msg* msg,
                               uint8_t type)
{
   STATUS       err;
   uint32_t     byte_cnt;
   sk_tapi_resp *resp;

   if ((msg == 0) || (pack == 0))
   {
      return SKTAPIPACKERR_INVALID_PARAMS;
   }

   err      = SKTAPIPACKERR_SUCCESS;
   byte_cnt = 0;
   resp     = &msg->type.sys_tapi.resp;

   switch(type)
   {
      case SK_TAPI_SYSTEM:
         READ(msg->hdr.context_id);
         READ(msg->hdr.sm_id);
         READ(msg->hdr.cmd);
         READ(resp->status);
         break;
      default:
         err = SKTAPIPACKERR_INVALID_PARAMS;
         break;
   }

   return err;
}

STATUS sk_tapi_open_context_unpack(const uint8_t* pack,
                                   uint32_t pack_len,
                                   sk_msg* msg,
                                   uint8_t type)
{
   STATUS               err;
   uint32_t             byte_cnt;
   sk_open_context_req  *req;
   sk_open_context_resp *resp;

   if ((msg == 0) || (pack == 0))
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
         READ(msg->hdr.context_id);
         READ(msg->hdr.sm_id);
         READ(msg->hdr.cmd);
         READ(req->context_id);
         break;
      default: /* SK_TAPI_MSG_RESP_TYPE */
         READ(msg->hdr.context_id);
         READ(msg->hdr.sm_id);
         READ(msg->hdr.cmd);
         READBUF(resp->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
         READ(resp->status);
         break;
   }

   return err;
}

STATUS sk_tapi_close_context_unpack(const uint8_t* pack,
                                    uint32_t pack_len,
                                    sk_msg* msg,
                                    uint8_t type)
{
   STATUS                err;
   uint32_t              byte_cnt;
   sk_close_context_req  *req;
   sk_close_context_resp *resp;

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
         READ(msg->hdr.context_id);
         READ(msg->hdr.sm_id);
         READ(msg->hdr.cmd);
         READBUF(req->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
         break;
      default: /* SK_TAPI_MSG_RESP_TYPE */
         READ(msg->hdr.context_id);
         READ(msg->hdr.sm_id);
         READ(msg->hdr.cmd);
         READ(resp->status);
         break;
   }

   return err;
}

STATUS sk_tapi_open_session_unpack(const uint8_t* pack,
                                   uint32_t pack_len,
                                   sk_msg* msg,
                                   uint8_t type)
{
   STATUS               err;
   uint32_t             byte_cnt;
   sk_open_session_req  *req;
   sk_open_session_resp *resp;

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
         READ(msg->hdr.context_id);
         READ(msg->hdr.sm_id);
         READ(msg->hdr.cmd);
         READBUF(req->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
         READBUF(req->taid,           SK_TAPI_SESSION_TAID_SIZE);
         break;
      default: /* SK_TAPI_MSG_RESP_TYPE */
         READ(msg->hdr.context_id);
         READ(msg->hdr.sm_id);
         READ(msg->hdr.cmd);
         READBUF(resp->session_handle, SK_TAPI_SESSION_HANDLE_SIZE);
         READ(resp->status);
         break;
   }

   return err;
}

STATUS sk_tapi_close_session_unpack(const uint8_t* pack,
                                    uint32_t pack_len,
                                    sk_msg* msg,
                                    uint8_t type)
{
   STATUS                err;
   uint32_t              byte_cnt;
   sk_close_session_req  *req;
   sk_close_session_resp *resp;

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
         READ(msg->hdr.context_id);
         READ(msg->hdr.sm_id);
         READ(msg->hdr.cmd);
         READBUF(req->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
         READBUF(req->session_handle, SK_TAPI_SESSION_HANDLE_SIZE);
         break;
      default: /* SK_TAPI_MSG_RESP_TYPE */
         READ(msg->hdr.context_id);
         READ(msg->hdr.sm_id);
         READ(msg->hdr.cmd);
         READ(resp->status);
         break;
   }

   return err;
}

STATUS sk_tapi_invoke_command_unpack(const uint8_t* pack,
                                     uint32_t pack_len,
                                     sk_msg* msg,
                                     uint8_t type)
{
   STATUS             err;
   uint32_t           byte_cnt;
   sk_invoke_cmd_req  *req;
   sk_invoke_cmd_resp *resp;

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
         READ(msg->hdr.context_id);
         READ(msg->hdr.sm_id);
         READ(msg->hdr.cmd);
         READBUF(req->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
         READBUF(req->session_handle, SK_TAPI_SESSION_HANDLE_SIZE);
         READ(req->cmd);
         break;
      default: /* SK_TAPI_MSG_RESP_TYPE */
         READ(msg->hdr.context_id);
         READ(msg->hdr.sm_id);
         READ(msg->hdr.cmd);
         READ(resp->status);
         break;
   }

   return err;
}

STATUS sk_tapi_register_memory_unpack(const uint8_t* pack,
                                      uint32_t pack_len,
                                      sk_msg* msg,
                                      uint8_t type)
{
   STATUS                  err;
   uint32_t                byte_cnt;
   sk_register_memory_req  *req;
   sk_register_memory_resp *resp;

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
         READ(msg->hdr.context_id);
         READ(msg->hdr.sm_id);
         READ(msg->hdr.cmd);
         READBUF(req->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
         READ(req->memory_size);
         READ(req->flag);
         READ(req->addr);
         break;
      default: /* SK_TAPI_MSG_RESP_TYPE */
         READ(msg->hdr.context_id);
         READ(msg->hdr.sm_id);
         READ(msg->hdr.cmd);
         READ(resp->addr);
         READBUF(resp->memory_id, SK_TAPI_MEMORY_ID_LENGTH);
         READ(resp->status);
         break;
   }

   return err;
}

STATUS sk_tapi_unregister_memory_unpack(const uint8_t* pack,
                                        uint32_t pack_len,
                                        sk_msg* msg,
                                        uint8_t type)
{
   STATUS                    err;
   uint32_t                  byte_cnt;
   sk_unregister_memory_req  *req;
   sk_unregister_memory_resp *resp;

   if ((msg == 0) || (pack == 0) || (pack_len == 0))
   {
      return SKTAPIPACKERR_INVALID_PARAMS;
   }

   req      = &msg->type.unregister_memory.req;
   resp     = &msg->type.unregister_memory.resp;
   err      = SKTAPIPACKERR_SUCCESS;
   byte_cnt = 0;

   switch(type)
   {
      case SK_TAPI_MSG_REQ_TYPE:
         READ(msg->hdr.context_id);
         READ(msg->hdr.sm_id);
         READ(msg->hdr.cmd);
         READBUF(req->context_handle, SK_TAPI_CONTEXT_HANDLE_SIZE);
         READBUF(req->memory_id,      SK_TAPI_MEMORY_ID_LENGTH);
         break;
      default: /* SK_TAPI_MSG_RESP_TYPE */
         READ(msg->hdr.context_id);
         READ(msg->hdr.sm_id);
         READ(msg->hdr.cmd);
         READ(resp->status);
         break;
   }

   return err;
}

STATUS sk_tapi_check_unpack(const uint8_t* pack,
                            uint32_t pack_len,
                            sk_msg* msg,
                            uint8_t type)
{
   STATUS        err;
   uint32_t      byte_cnt;
   sk_check_resp *resp;

   if ((msg == 0) || (pack == 0) || (pack_len == 0))
   {
      return SKTAPIPACKERR_INVALID_PARAMS;
   }

   resp     = &msg->type.sys_check.resp;
   err      = SKTAPIPACKERR_SUCCESS;

   byte_cnt = 0;

   switch(type)
   {
      case SK_TAPI_MSG_REQ_TYPE:
         READ(msg->hdr.context_id);
         READ(msg->hdr.sm_id);
         READ(msg->hdr.cmd);
         break;
      default: /* SK_TAPI_MSG_RESP_TYPE */
         READ(msg->hdr.context_id);
         READ(msg->hdr.sm_id);
         READ(msg->hdr.cmd);
         READ(resp->status);
         break;
   }

   return err;
}
