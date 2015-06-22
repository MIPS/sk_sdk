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


#ifndef _SK_DRV_SWLD_INTERFACE_
#define _SK_DRV_SWLD_INTERFACE_

typedef struct _sk_swld_tapi_kernel_access_
{
   void* my_obj;
}sk_swld_tapi_kernel_access;

/*
  Shared memory Request/Response
*/
typedef struct _shared_memory_req_
{
   uint32_t memory_addr;
   uint32_t memory_size;
}shared_memory_req;

typedef struct _shared_memory_resp_
{
   int32_t error;
}shared_memory_resp;

typedef union _tSkSysSrvKernelAccessParamReq_
{
   shared_memory_req sm;
}tSkSysSrvKernelAccessParamReq;

typedef union _tSkSysSrvKernelAccessParamResp_
{
   shared_memory_resp sm;
}tSkSysSrvKernelAccessParamResp;

#define SK_KERNEL_ACCESS_SHARE_MEMORY_REQUEST 0


/*
  Return:
*/
SK_STATUS sk_drv_server_ka_cmd(sk_swld_tapi_kernel_access*       obj,
                               uint32_t                        cmd,
                               tSkSysSrvKernelAccessParamReq*  req,
                               tSkSysSrvKernelAccessParamResp* resp);


SK_STATUS sk_drv_swld_memory_config_allocate_memory (uint8_t  *mem,
                                                     uint32_t  mem_size,
                                                     uint32_t  flag,
                                                     uint8_t  *memory_id,
                                                     uint32_t *memory_id_length,
                                                     void     *info,
                                                     void     *log);
SK_STATUS sk_drv_swld_memory_config_free_memory     (uint8_t *mem,
                                                     void    *info,
                                                     void    *log);

typedef struct _tSkDrvSSFileHandle_
{
   void* tObj;
}tSkDrvSSFileHandle;

#define SK_DRV_SS_READ_BINARY_FLAG   0x00000001
#define SK_DRV_SS_WRITE_BINARY_FLAG  0x00000002
#define SK_DRV_SS_APPEND_FLAG        0x00000004


tSkDrvSSFileHandle* sk_drv_swld_secure_storage_open  (char* file, uint32_t flag, void* log_handle);
SK_STATUS           sk_drv_swld_secure_storage_close (tSkDrvSSFileHandle* handle, void* log_handle);
uint32_t            sk_drv_swld_secure_storage_write (tSkDrvSSFileHandle* handle, uint8_t* buffer, uint32_t buffer_size, void* log_handle);
uint32_t            sk_drv_swld_secure_storage_read  (tSkDrvSSFileHandle* handle, uint8_t* buffer, uint32_t buffer_size, void* log_handle);


uint32_t sk_drv_swld_secure_timer_get  (void);
int32_t  sk_drv_swld_secure_timer_diff (uint32_t t1, uint32_t t2);

#endif
