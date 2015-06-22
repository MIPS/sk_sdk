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

#define STORE(val) \
         cnt = sysv_byte_put(obuffer, byte_cnt, *obuffer_size, sizeof(val), val); \
         if (!cnt) { return SK_ERROR_INVALID_PARAM; } \
         byte_cnt += cnt;

#define STOREBUF(val, len) \
         if (byte_cnt + len > *obuffer_size) { return SK_ERROR_INVALID_PARAM; } \
         SYSV_MEMCPY(&obuffer[byte_cnt], val, len); \
         byte_cnt += len;

#define READ(x) \
   if (byte_cnt + sizeof(x) > ibuffer_size) { return SK_ERROR_INVALID_PARAM; } \
   x = sysv_byte_get(&ibuffer[byte_cnt], sizeof(x)); \
   byte_cnt += sizeof(x);


#define READBUF(x, s) \
   if (byte_cnt + s > ibuffer_size) { return SK_ERROR_INVALID_PARAM; } \
   SYSV_MEMCPY(x, &ibuffer[byte_cnt], s); \
   byte_cnt           += s;

SK_STATUS tapp_manifest_pack_put(tapps_info_data* tapp_data, uint8_t* obuffer, uint32_t* obuffer_size)
{
   SK_STATUS err;
   uint32_t byte_cnt, cnt;

   err = SK_ERROR_SUCCESS;

   if ((tapp_data == 0) ||
       (obuffer == 0)   ||
       (obuffer_size == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   byte_cnt      = 0;

   STOREBUF(tapp_data->taid, sizeof(tapp_data->taid));
   STORE(tapp_data->multi_instance);
   STORE(tapp_data->multi_session);
   STORE(tapp_data->max_heap_size);
   STORE(tapp_data->max_stack_size);
   STOREBUF(tapp_data->service_name, sizeof(tapp_data->service_name));
   STOREBUF(tapp_data->vendor_name, sizeof(tapp_data->vendor_name));
   STOREBUF(tapp_data->description, sizeof(tapp_data->description));

   *obuffer_size = byte_cnt;

   return err;
}

SK_STATUS tapp_manifest_pack_get(uint8_t* ibuffer, uint32_t ibuffer_size, tapps_info_data* tapp_data)
{
   SK_STATUS err;
   uint32_t byte_cnt;

   err = SK_ERROR_SUCCESS;

   if ((tapp_data == 0) ||
       (ibuffer == 0)   ||
       (ibuffer_size == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   byte_cnt = 0;

   READBUF(tapp_data->taid, sizeof(tapp_data->taid));
   READ(tapp_data->multi_instance);
   READ(tapp_data->multi_session);
   READ(tapp_data->max_heap_size);
   READ(tapp_data->max_stack_size);
   READBUF(tapp_data->service_name, sizeof(tapp_data->service_name));
   READBUF(tapp_data->vendor_name,  sizeof(tapp_data->vendor_name));
   READBUF(tapp_data->description,  sizeof(tapp_data->description));

   return err;
}












