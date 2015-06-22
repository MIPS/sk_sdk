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


#ifndef _SK_DRV_SERVER_COMM_
#define _SK_DRV_SERVER_COMM_

#include "sk_swld.h"

/*
  Internal Functions
*/
typedef struct _tSkSysSrvCommVmFiascoObj_
{
   uint8_t  init;
   int32_t  device_rx;
   int32_t  device_tx;
   uint32_t  comm_err;
}tSkSysSrvCommVmFiascoObj;

#define MYSRVCOMMERR_SUCCESS          0
#define MYSRVCOMMERR_RECV_MSG_FAILED -1
#define MYSRVCOMMERR_SEND_MSG_FAILED -2

SK_STATUS sk_swld_platform_comm_init(tSkSysSrvCommVmFiascoObj* myObj, char *device_name);
SK_STATUS sk_swld_platform_comm_end(tSkSysSrvCommVmFiascoObj* myObj);

/*
  Return: SK_ERROR_SUCCESS
          SK_ERROR_COMM_RECV_FAILED

*/
int32_t sk_swld_platform_comm_recv    (void *pobj, uint8_t* buf, uint32_t* buf_len, sk_operation* op);

/*
  Return: SK_ERROR_SUCCESS
          SK_ERROR_COMM_SEND_FAILED

*/
int32_t sk_swld_platform_comm_send   (void *pobj, uint8_t* buf, uint32_t buf_len, sk_operation* op);

SK_STATUS sk_drv_server_is_init     (tSkSysSrvCommVmFiascoObj* obj);


#endif /* _SK_DRV_SERVER_COMM_ */







