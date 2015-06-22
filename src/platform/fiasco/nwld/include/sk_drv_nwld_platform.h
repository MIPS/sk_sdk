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


#ifndef _SK_DRV_CLIENT_PLATFORM_VM_FIASCO_H_
#define _SK_DRV_CLIENT_PLATFORM_VM_FIASCO_H_



#define CONTEXT_ID      0x1010
#define SM_ID           0x2020
#define MAX_CONNECTIONS 1
#define MAX_SESSIONS    64

typedef struct
{
   char *device_name;
} my_platform_param;

typedef struct
{
   int32_t device_rx;
   int32_t device_tx;
   uint32_t comm_err;
   char device_name[50];
} tSkSysCltCommVmFiascoObj;

#define MYCLTCOMMERR_SUCCESS          0
#define MYCLTCOMMERR_RECV_MSG_FAILED -1
#define MYCLTCOMMERR_SEND_MSG_FAILED -2


int32_t sk_nwld_platform_tapi_ioctl_init(uint32_t max_connections,
                                    uint32_t max_sessions,
                                    uint32_t sm_id,
                                    char *device_name);

int32_t sk_nwld_platform_tapi_ioctl_end(void);
int32_t sk_nwld_platform_tapi_ioctl_debug(void);


#endif /* _SK_DRV_CLIENT_PLATFORM_VM_FIASCO_H_ */
