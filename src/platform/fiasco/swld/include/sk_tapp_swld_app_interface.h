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


#ifndef _SK_TAPP_SWLD_APP_INTERFACE_H_
#define _SK_TAPP_SWLD_APP_INTERFACE_H_

#include <sk_swld.h>

SK_STATUS    sk_swld_platform_tapp_load                         (sk_swld_tapp_instance* tapp, char* db, tapp_toc_entry* entry, void* log_handle, uint32_t timeout_ms);
SK_STATUS    sk_swld_platform_tapp_unload                  (sk_swld_tapp_instance* tapp, void* log_handle, uint32_t timeout_ms);
SK_TA_STATUS sk_swld_platform_tapp_init              (sk_swld_tapp_instance* tapp, void* session, void* log_handle, uint32_t timeout_ms);
SK_TA_STATUS sk_swld_platform_tapp_close             (sk_swld_tapp_instance* tapp, void* session, void* log_handle, uint32_t timeout_ms);
SK_TA_STATUS sk_swld_platform_tapp_pull (sk_swld_tapp_instance* tapp, void* session, uint32_t cmd_id, sk_operation* op, void* log_handle, uint32_t timeout_ms);


#endif /* _SK_TAPP_SWLD_APP_INTERFACE_H_ */
