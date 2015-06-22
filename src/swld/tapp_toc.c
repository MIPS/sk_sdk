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

SK_STATUS tapp_toc_remove_entry(uint8_t* toc, uint32_t toc_size, uint8_t* taid, tapp_toc_header* header, void* log_handle)
{
   SK_STATUS       err;
   uint32_t        entry_idx;
   uint32_t        i;

   if ((err = tapp_toc_search_entries(toc, toc_size, taid, &entry_idx, header, log_handle)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(log_handle, SYSV_LOG_CRITICAL_ERR, "Error: Failed to find TAID err=%d", err);
      return SK_ERROR_FAILED;
   }

   /*
      found TAID and remove it
   */
   toc += TAPP_TOC_GET_ENTRY_INDEX(entry_idx);
   SYSV_MEMSET(toc, 0, TAPP_TOC_INDEX_ENTRY_TOTAL_SIZE);

   /*
      move all the entries up (TOC will never be fragmented)
   */
   SYSV_ASSERT(header->entries > 0);

   header->entries--;

   for (i = entry_idx; i < header->entries; i++)
   {
      SYSV_MEMCPY(toc, (toc + TAPP_TOC_INDEX_ENTRY_TOTAL_SIZE), TAPP_TOC_INDEX_ENTRY_TOTAL_SIZE);
   }

   return err;
}

SK_STATUS tapp_toc_search_entries(uint8_t* toc, uint32_t toc_size, uint8_t* taid, uint32_t* entry_idx, tapp_toc_header* header, void* log_handle)
{
   SK_STATUS err;
   uint32_t  i;

   (void)log_handle;

   if ((toc == 0)      ||
       (toc_size == 0) ||
       (taid == 0)     ||
       (entry_idx == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   err = SK_ERROR_SUCCESS;

   if (header->cookie != TAPP_TOC_INDEX_HDR_COOKIE)
   {
      SYSV_ERROR2(log_handle, SYSV_LOG_CRITICAL_ERR, "Error: Bad Cookie [%.8x] err=%d", header->cookie, err);
      return SK_ERROR_INVALID_COOKIE;
   }

   /*
     go to the start of Entry
   */
   toc += TAPP_TOC_INDEX_HDR_TAPP_START_POS;
   for(i = 0; i < header->entries; i++)
   {
      if (SYSV_MEMCMP(toc, taid, TAPP_TOC_INDEX_ENTRY_TAID_SIZE) == 0)
      {
         *entry_idx = i;
         goto DONE;
      }

      toc += TAPP_TOC_INDEX_ENTRY_TOTAL_SIZE;
   }

   /*
      not found
   */
   err = SK_ERROR_FAILED;

DONE:
   return err;
}













