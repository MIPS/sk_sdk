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
#include "sk_drv_nwld_platform.h"

SKTEEC_STATUS sk_nwld_platform_init(sk_nwld_teec_platform* obj, void* param)
{
   SKTEEC_STATUS     err;
   my_platform_param *ip_param;
   (void)obj;

   err = SK_ERROR_SUCCESS;
   ip_param = (my_platform_param *)param;

   /*
     Initialize the IOCTL framework
   */
   err = sk_nwld_platform_tapi_ioctl_init(MAX_CONNECTIONS, MAX_SESSIONS, SM_ID, ip_param->address, ip_param->port);

   return err;
}

SKTEEC_STATUS sk_nwld_platform_end(sk_nwld_teec_platform* obj)
{
   SKTEEC_STATUS err;
   (void)obj;

   err = SK_ERROR_SUCCESS;

   return err;
}

SKTEEC_STATUS sk_drv_nwld_platform_debug(sk_nwld_teec_platform* obj)
{
   SKTEEC_STATUS err;
   (void)obj;

   err = SK_ERROR_SUCCESS;

   sk_nwld_platform_tapi_ioctl_debug();

   return err;
}








