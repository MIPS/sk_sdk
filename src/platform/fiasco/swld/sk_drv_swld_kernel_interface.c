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


#include <sk_swld.h>
#include "sk_drv_swld_kernel_interface_vm_fiasco.h"

/*
  user defined functions
*/
SK_STATUS my_sk_drv_server_ka_init(sk_swld_tapi_kernel_access* obj)
{
   SK_STATUS err;
   (void)obj;

   err = SK_ERROR_SUCCESS;

   return err;
}

SK_STATUS my_sk_drv_server_ka_end(sk_swld_tapi_kernel_access* obj)
{
   (void)obj;
   return SK_ERROR_SUCCESS;
}

/*
  standard protocol function
*/
SK_STATUS sk_drv_server_ka_cmd(sk_swld_tapi_kernel_access*       obj,
                               uint32_t                        cmd,
                               tSkSysSrvKernelAccessParamReq*  req,
                               tSkSysSrvKernelAccessParamResp* resp)
{
   SK_STATUS err;
   (void)obj;
   (void)req;
   (void)resp;

   err = SK_ERROR_SUCCESS;

   switch(cmd)
   {
      case SK_KERNEL_ACCESS_SHARE_MEMORY_REQUEST:
         break;
      default:
         err = SK_ERROR_INVALID_CMD;
         break;
   };

   return err;
}
