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


#ifndef _SK_SWLD_H_
#define _SK_SWLD_H_

#include "sk_both.h"

#ifndef TAPI_TAPP_LOAD_TIMEOUT_MS
   #define TAPI_TAPP_LOAD_TIMEOUT_MS 1000
#endif

#ifndef TAPI_TAPP_UNLOAD_TIMEOUT_MS
   #define TAPI_TAPP_UNLOAD_TIMEOUT_MS 1000
#endif

#ifndef TAPI_TAPP_INIT_IFACE_TIMEOUT_MS
   #define TAPI_TAPP_INIT_IFACE_TIMEOUT_MS 1000
#endif

#ifndef TAPI_TAPP_CLOSE_IFACE_TIMEOUT_MS
   #define TAPI_TAPP_CLOSE_IFACE_TIMEOUT_MS 1000
#endif

#ifndef TAPI_TAPP_RECVOP_IFACE_TIMEOUT_MS
   #define TAPI_TAPP_RECVOP_IFACE_TIMEOUT_MS 1000
#endif

#ifndef TAPI_SESSION_TIMEOUT_MS
   #define TAPI_SESSION_TIMEOUT_MS 0xffffffff
#endif

#ifndef TAPI_CONTEXT_TIMEOUT_MS
   #define TAPI_CONTEXT_TIMEOUT_MS 0xffffffff
#endif

#ifndef TAPI_PROJECT_DIR
   #define TAPI_PROJECT_DIR "/home/aelias/Development/virtualization/"
#endif

#define SK_SECURE_STORAGE_MOUNT         ""
#define SK_SECURE_STORAGE_TAPP_LOCATION ""
#define SK_SECURE_STORAGE_TAPP_TOC_FILE "toc.dat"
#define SK_SECURE_STORAGE_TAPP_DB_FILE  "db.dat"


typedef int SK_TA_STATUS;

#define SK_TA_ERROR_SUCCESS  0
#define SK_TA_ERROR_FAILED  -1

/*
   API definitions for the Secure Trustlet
*/
typedef struct _sktapp_func_def_
{
   void*         session;      /* opaque object used for TApp, if required */
   SK_TA_STATUS  (*SKTAPP_init_iface)(void* session);
   SK_TA_STATUS  (*SKTAPP_close_iface)(void* session);
   SK_TA_STATUS  (*SKTAPP_receive_operation_iface)(uint8_t *taid, void* session, uint32_t cmd, sk_operation* op);
}sktapp_func_def;

#define SK_TAPP_MAX_SERVICE_NAME     256
#define SK_TAPP_MAX_VENDOR_NAME      256
#define SK_TAPP_MAX_DESCRIPTION_NAME 512

typedef struct _tapps_info_data_
{
   /*
      TAID
   */
   SK_TAID  taid;
   uint8_t  multi_instance;
   uint8_t  multi_session;
   uint32_t max_heap_size;
   uint32_t max_stack_size;
   uint8_t  service_name[SK_TAPP_MAX_SERVICE_NAME];
   uint8_t  vendor_name[SK_TAPP_MAX_VENDOR_NAME];
   uint8_t  description[SK_TAPP_MAX_DESCRIPTION_NAME];
}tapps_info_data;

#define TAPPS_INFO_DATA_SIZE (SK_TAPI_SESSION_TAID_SIZE + \
                              sizeof(uint8_t)  +          \
                              sizeof(uint8_t)  +          \
                              sizeof(uint32_t) +          \
                              sizeof(uint32_t) +          \
                              SK_TAPP_MAX_SERVICE_NAME +  \
                              SK_TAPP_MAX_VENDOR_NAME  +  \
                              SK_TAPP_MAX_DESCRIPTION_NAME)

typedef struct _sk_swld_tapp_instance_
{
   void* arch_obj;  /* maintain instance variables required by the LOADER interface */
}sk_swld_tapp_instance;

typedef int SK_STATUS;

/*
   PRNG type
*/
#define SKPRNG_MAX_SEED_LENGTH 256

typedef struct _tSkPrng_
{
   void* type;
}tSkPrng;


/*
  Common MESSAGE Parsing functions
*/
#define SK_MSG_GET_CONTEXT_ID(_sk_msg_) ((_sk_msg_)->hdr.context_id)
#define SK_MSG_GET_SM_ID(_sk_msg_)      ((_sk_msg_)->hdr.sm_id)

#include <sk_swld_drv.h>
#include <sk_swld_tapi.h>
#include <sk_swld_tapi_handle_generator_plugin.h>
#include <sk_swld_tapp.h>

#endif
