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

SK_STATUS tapp_toc_read_header(uint8_t* toc, uint32_t toc_size, tapp_toc_header* header, void* log_handle)
{
   SK_STATUS       err;

   if ((toc == 0)      ||
       (toc_size == 0) ||
       (header == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   err = SK_ERROR_SUCCESS;

   header->cookie = sysv_byte_get(&toc[TAPP_TOC_INDEX_HDR_COOKIE_POS], TAPP_TOC_INDEX_HDR_COOKIE_SIZE);

   if (header->cookie != TAPP_TOC_INDEX_HDR_COOKIE)
   {
      SYSV_ERROR2(log_handle, SYSV_LOG_CRITICAL_ERR, "Error: Bad Cookie [%.8x] err=%d", header->cookie, err);
      return SK_ERROR_INVALID_COOKIE;
   }

   header->entries = sysv_byte_get(&toc[TAPP_TOC_INDEX_HDR_NUM_ENTRIES_POS], TAPP_TOC_INDEX_HDR_NUM_ENTRIES_SIZE);

   return err;
}

//#warning this function doesnt tell the user how many bytes it wrote out to toc
SK_STATUS tapp_toc_write_header(uint8_t* toc, uint32_t toc_size, tapp_toc_header* header, void* log_handle)
{
   SK_STATUS       err;
   uint32_t cnt;
   (void)log_handle;

   if ((toc == 0)      ||
       (toc_size == 0) ||
       (header == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   err = SK_ERROR_SUCCESS;

   cnt = sysv_byte_put(toc, TAPP_TOC_INDEX_HDR_COOKIE_POS,      toc_size, TAPP_TOC_INDEX_HDR_COOKIE_SIZE,      header->cookie);
   if (!cnt) { return SK_ERROR_HEADER_FAILED; }
   cnt = sysv_byte_put(toc, TAPP_TOC_INDEX_HDR_NUM_ENTRIES_POS, toc_size, TAPP_TOC_INDEX_HDR_NUM_ENTRIES_SIZE, header->entries);
   if (!cnt) { return SK_ERROR_HEADER_FAILED; }

   return err;
}

SK_STATUS tapp_toc_get_entry(uint8_t* entry, uint32_t entry_size, tapp_toc_entry* entry_info, void* log_handle)
{
   SK_STATUS err;
   uint8_t*  p;

   (void)log_handle;

   if ((entry == 0)      ||
       (entry_size == 0) ||
       (entry_info == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   err = SK_ERROR_SUCCESS;
   p   = entry;

   // sanity check input size
   if ((SK_TAPI_SESSION_TAID_SIZE + TAPP_TOC_INDEX_ENTRY_SIZE_SIZE + TAPP_TOC_INDEX_ENTRY_ADDR_SIZE) > entry_size) {
      return SK_ERROR_HEADER_FAILED;
   }

   /*
      add element to end
   */
   SYSV_MEMCPY(entry_info->taid, p, SK_TAPI_SESSION_TAID_SIZE);
   p += TAPP_TOC_INDEX_ENTRY_TAID_SIZE;
   entry_info->tapp_size = sysv_byte_get(p, TAPP_TOC_INDEX_ENTRY_SIZE_SIZE);
   p += TAPP_TOC_INDEX_ENTRY_SIZE_SIZE;
   SYSV_MEMCPY(entry_info->address, p, TAPP_TOC_INDEX_ENTRY_ADDR_SIZE);

   return err;
}

#define STOREBUF(val, len) \
         if (byte_cnt + len > toc_size) { return SK_ERROR_HEADER_FAILED; } \
         SYSV_MEMCPY(ptoc, val, len); \
         byte_cnt += len;

SK_STATUS tapp_toc_add_entry(uint8_t* toc,
                             uint32_t toc_size,
                             uint8_t* tapp_manifest_bin,
                             uint32_t tapp_manifest_bin_size,
                             uint32_t tapp_size,
                             uint8_t  address[TAPP_TOC_INDEX_ENTRY_ADDR_SIZE],
                             void* log_handle)
{
   SK_STATUS       err;
   uint8_t*        ptoc;
   tapps_info_data tapp_info;
   tapp_toc_header header;
   uint32_t        byte_cnt;

   if ((toc == 0)                    ||
       (toc_size == 0)               ||
       (tapp_manifest_bin == 0)      ||
       (tapp_manifest_bin_size == 0) ||
       (tapp_size == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   if ((err = tapp_toc_read_header(toc, toc_size, &header, log_handle)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(log_handle, SYSV_LOG_CRITICAL_ERR, "Error: Failed reading header err=%d", err);
      return SK_ERROR_HEADER_FAILED;
   }

   /*
      go to the end position (TOC will never be fragmented)
   */
   ptoc = (toc + TAPP_TOC_INDEX_HDR_TAPP_START_POS + (header.entries*TAPP_TOC_INDEX_ENTRY_TOTAL_SIZE));

   /*
     parse manifest information
   */
   tapp_manifest_pack_get(tapp_manifest_bin, tapp_manifest_bin_size, &tapp_info);

   /*
      add element to end
   */
   byte_cnt = 0;
   STOREBUF(tapp_info.taid, SK_TAPI_SESSION_TAID_SIZE);
   sysv_byte_put(ptoc, 0, toc_size - TAPP_TOC_INDEX_ENTRY_TAID_SIZE, TAPP_TOC_INDEX_ENTRY_SIZE_SIZE, tapp_size);
   ptoc += TAPP_TOC_INDEX_ENTRY_SIZE_SIZE;
   STOREBUF (address, TAPP_TOC_INDEX_ENTRY_ADDR_SIZE);

   /*
      update header
   */
   header.entries++;
   if ((err = tapp_toc_write_header(toc, toc_size, &header, log_handle)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(log_handle, SYSV_LOG_CRITICAL_ERR, "Error: Failed writing header err=%d", err);
      return SK_ERROR_HEADER_FAILED;
   }

   return err;
}

SK_STATUS tapp_toc_list(uint8_t* toc, uint32_t toc_size, void* log_handle)
{
   SK_STATUS       err;
   uint32_t        i;
   tapp_toc_header header;
   tapp_toc_entry  entry;
   uint8_t*        ptoc;

   if ((toc == 0)  ||
       (toc_size == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   if ((err = tapp_toc_read_header(toc, toc_size, &header, log_handle)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(log_handle, SYSV_LOG_CRITICAL_ERR, "Error: Failed reading header err=%d", err);
      return SK_ERROR_HEADER_FAILED;
   }

   SYSV_INFO0(log_handle, "HEADER ------");
   SYSV_INFO1(log_handle, "COOKIE: 0x%.8x", header.cookie);
   SYSV_INFO1(log_handle, "ENTRIES: %d", header.entries);
   SYSV_INFO0(log_handle, "");
   SYSV_INFO0(log_handle, "");

   ptoc = (toc + TAPP_TOC_INDEX_HDR_TAPP_START_POS);

   for(i = 0;  i < header.entries; i++)
   {
      SYSV_INFO1(log_handle, "ENTRY ------ [%d]", i);
      tapp_toc_get_entry(ptoc, TAPP_TOC_INDEX_ENTRY_TOTAL_SIZE, &entry, log_handle);

      SYSV_LDUMP(log_handle, "Entry: TAID:", entry.taid, SK_TAPI_SESSION_TAID_SIZE);
      SYSV_INFO1(log_handle, "Entry: APP SIZE: %d",entry.tapp_size);
      SYSV_LDUMP(log_handle, "Entry: Address:",entry.address, TAPP_TOC_INDEX_ENTRY_ADDR_SIZE);

      ptoc += TAPP_TOC_INDEX_ENTRY_TOTAL_SIZE;
   }

   return err;
}













