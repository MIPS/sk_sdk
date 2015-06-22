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


#ifndef _TAPP_TOC_H_
#define _TAPP_TOC_H_

#include "tapp.h"

typedef struct _tapp_toc_header_
{
   uint32_t cookie;
   uint32_t entries;
}tapp_toc_header;

/*
  Table of Content (TOC) HEADER structure:

  [COOKIE (4 bytes)][Number of Entries (4 bytes)][reserved (8 bytes)]

  Table of Content (TOC) Entry structure:

  [TAID (16 bytes)][Size (4 bytes)][Address (4 bytes)][Reserved (16 bytes)]
*/

#define TAPP_TOC_INDEX_HDR_COOKIE           0x36c186a6  /* version 1 */
#define TAPP_TOC_INDEX_HDR_COOKIE_POS       0x00
#define TAPP_TOC_INDEX_HDR_COOKIE_SIZE      4
#define TAPP_TOC_INDEX_HDR_NUM_ENTRIES_POS  (TAPP_TOC_INDEX_HDR_COOKIE_POS + TAPP_TOC_INDEX_HDR_COOKIE_SIZE)
#define TAPP_TOC_INDEX_HDR_NUM_ENTRIES_SIZE 4
#define TAPP_TOC_INDEX_HDR_RESERVED_POS     (TAPP_TOC_INDEX_HDR_NUM_ENTRIES_POS + TAPP_TOC_INDEX_HDR_NUM_ENTRIES_SIZE)
#define TAPP_TOC_INDEX_HDR_RESERVED_SIZE    8
#define TAPP_TOC_INDEX_HDR_TAPP_START_POS   (TAPP_TOC_INDEX_HDR_NUM_ENTRIES_POS + TAPP_TOC_INDEX_HDR_NUM_ENTRIES_SIZE + TAPP_TOC_INDEX_HDR_RESERVED_SIZE)
#define TAPP_TOC_INDEX_HDR_TOTAL_SIZE       (TAPP_TOC_INDEX_HDR_COOKIE_SIZE + TAPP_TOC_INDEX_HDR_NUM_ENTRIES_SIZE + TAPP_TOC_INDEX_HDR_RESERVED_SIZE)

#define TAPP_TOC_INDEX_ENTRY_TAID_POS       0x00
#define TAPP_TOC_INDEX_ENTRY_TAID_SIZE      SK_TAPI_SESSION_TAID_SIZE
#define TAPP_TOC_INDEX_ENTRY_SIZE_POS       TAPP_TOC_INDEX_ENTRY_TAID_SIZE
#define TAPP_TOC_INDEX_ENTRY_SIZE_SIZE      4
#define TAPP_TOC_INDEX_ENTRY_ADDR_POS       (TAPP_TOC_INDEX_ENTRY_SIZE_POS + TAPP_TOC_INDEX_ENTRY_SIZE_SIZE)
#define TAPP_TOC_INDEX_ENTRY_ADDR_SIZE      32
#define TAPP_TOC_INDEX_ENTRY_RESERVED_POS  (TAPP_TOC_INDEX_ENTRY_ADDR_POS + TAPP_TOC_INDEX_ENTRY_ADDR_SIZE)
#define TAPP_TOC_INDEX_ENTRY_RESERVED_SIZE  32
#define TAPP_TOC_INDEX_ENTRY_TOTAL_SIZE     (TAPP_TOC_INDEX_ENTRY_RESERVED_POS + TAPP_TOC_INDEX_ENTRY_RESERVED_SIZE)

typedef struct _tapp_toc_entry_
{
   uint8_t  taid[TAPP_TOC_INDEX_ENTRY_TAID_SIZE];
   uint32_t tapp_size;
   uint8_t  address[TAPP_TOC_INDEX_ENTRY_ADDR_SIZE];
   uint8_t  reserved[TAPP_TOC_INDEX_ENTRY_TOTAL_SIZE];
}tapp_toc_entry;



SK_STATUS tapp_toc_remove_entry   (uint8_t* toc, uint32_t toc_size, uint8_t* taid, tapp_toc_header* header, void* log_handle);
SK_STATUS tapp_toc_search_entries (uint8_t* toc, uint32_t toc_size, uint8_t* taid, uint32_t* entry_idx, tapp_toc_header* header, void* log_handle);

#endif /* _TAPP_TOC_H_ */
