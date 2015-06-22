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


#ifndef _TAPP_TOC_SHARED_H_
#define _TAPP_TOC_SHARED_H_

#include "tapp_toc.h"

/*
  MACRO defintions
*/
#define TAPP_TOC_HDR_GET_COOKIE(__header__)              ((__header__)->cookie)
#define TAPP_TOC_HDR_GET_ENTRIES(__header__)             ((__header__)->entries)
#define TAPP_TOC_HDR_SET_COOKIE(__header__, __cookie__)  (TAPP_TOC_HDR_GET_COOKIE(__header__) = (__cookie__))
#define TAPP_TOC_HDR_SET_ENTRIES(__header__, __entries__)(TAPP_TOC_HDR_GET_ENTRIES(__header__) = (__entries__))
#define TAPP_TOC_GET_ENTRY_INDEX(__entry__)              (TAPP_TOC_INDEX_HDR_TOTAL_SIZE + ((__entry__)*TAPP_TOC_INDEX_ENTRY_TOTAL_SIZE))

SK_STATUS tapp_toc_read_header  (uint8_t* toc,
                                 uint32_t toc_size,
                                 tapp_toc_header* header,
                                 void* log_handle);
SK_STATUS tapp_toc_write_header (uint8_t* toc,
                                 uint32_t toc_size,
                                 tapp_toc_header* header,
                                 void* long_handle);
SK_STATUS tapp_toc_add_entry    (uint8_t* toc,
                                 uint32_t toc_size,
                                 uint8_t* tapp_manifest_bin,
                                 uint32_t tapp_manifest_bin_size,
                                 uint32_t tapp_size,
                                 uint8_t  address[TAPP_TOC_INDEX_ENTRY_ADDR_SIZE],
                                 void* log_handle);
SK_STATUS tapp_toc_get_entry    (uint8_t* entry,
                                 uint32_t entry_size,
                                 tapp_toc_entry* entry_info,
                                 void* long_handle);
SK_STATUS tapp_toc_list         (uint8_t* toc,
                                 uint32_t toc_size,
                                 void* log_handle);


#endif /* _TAPP_TOC_H_ */
