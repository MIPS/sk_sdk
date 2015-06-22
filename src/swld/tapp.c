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
#include <sk_swld.h>
#include <sk_tapp_swld_app_interface.h>

#define TOC_FILE SK_SECURE_STORAGE_MOUNT""SK_SECURE_STORAGE_TAPP_TOC_FILE
#define DB_FILE  SK_SECURE_STORAGE_MOUNT""SK_SECURE_STORAGE_TAPP_DB_FILE

SK_STATUS tapp_load(uint8_t* taid, sk_swld_tapp_instance* tapp, void* log_handle)
{
   SK_STATUS           err;
   tSkDrvSSFileHandle* sh;
   tapp_toc_header     header;
   tapp_toc_entry      entry;
   uint8_t             buffer[(TAPP_TOC_INDEX_HDR_TOTAL_SIZE + TAPP_TOC_INDEX_ENTRY_TOTAL_SIZE)];
   uint32_t            i;
   uint8_t             found_entry;

   if ((taid == 0) || (tapp == 0))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   found_entry = 0;

   if ((sh = sk_drv_swld_secure_storage_open(TOC_FILE, SK_DRV_SS_READ_BINARY_FLAG, log_handle)) == 0)
   {
      SYSV_ERROR0(log_handle, SYSV_LOG_CRITICAL_ERR, "Error: opening up TOC file ");
      return SK_ERROR_OPEN_FAILED;
   }

   if (sk_drv_swld_secure_storage_read(sh,buffer, TAPP_TOC_INDEX_HDR_TOTAL_SIZE, log_handle) != TAPP_TOC_INDEX_HDR_TOTAL_SIZE)
   {
      SYSV_ERROR0(log_handle, SYSV_LOG_CRITICAL_ERR, "Error: reading HEADER in TOC file");
      err = SK_ERROR_READ_FAILED;
      goto DONE;
   }

   if ((err = tapp_toc_read_header(buffer, TAPP_TOC_INDEX_HDR_TOTAL_SIZE, &header, log_handle)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(log_handle, SYSV_LOG_CRITICAL_ERR, "Error: Failed reading header err=%d", err);
      err = SK_ERROR_HEADER_FAILED;
      goto DONE;
   }

   if (header.cookie != TAPP_TOC_INDEX_HDR_COOKIE)
   {
      SYSV_ERROR2(log_handle, SYSV_LOG_CRITICAL_ERR, "Error: Bad Cookie [%.8x] err=%d", header.cookie, err);
      err = SK_ERROR_INVALID_COOKIE;
      goto DONE;
   }

   /*
     now search each entry
   */
   for(i = 0;  i < header.entries; i++)
   {
      if (sk_drv_swld_secure_storage_read(sh, buffer, TAPP_TOC_INDEX_ENTRY_TOTAL_SIZE, log_handle) != TAPP_TOC_INDEX_ENTRY_TOTAL_SIZE)
      {
         SYSV_ERROR0(log_handle, SYSV_LOG_CRITICAL_ERR, "Error: reading ENTRY in TOC file");
         err = SK_ERROR_READ_FAILED;
         goto DONE;
      }

      if ((err = tapp_toc_get_entry(buffer, TAPP_TOC_INDEX_ENTRY_TOTAL_SIZE, &entry, log_handle)) != SK_ERROR_SUCCESS)
      {
         SYSV_ERROR1(log_handle, SYSV_LOG_CRITICAL_ERR, "Error: Failed reading entry err=%d", err);
         err = SK_ERROR_ENTRY_FAILED;
         goto DONE;
      }

#if 0
      printf("\nDatabase entry TAID: ");
      display_taid(entry.taid);
      printf("\nLooking for TAID: ");
      display_taid(taid);
      printf("\n\n");
#endif

      if (SYSV_MEMCMP(entry.taid, taid, TAPP_TOC_INDEX_ENTRY_TAID_SIZE) == 0)
      {
         /*
           found it now leave
         */
         found_entry = 1;
         break;
      }
   }

   if (found_entry == 0)
   {
      SYSV_INFO0(log_handle, "Entry not found");
      err = SK_ERROR_ENTRY_NOT_FOUND;
      goto DONE;
   }

   SYSV_LDUMP(log_handle, "Entry: TAID:", entry.taid, SK_TAPI_SESSION_TAID_SIZE);
   SYSV_INFO1(log_handle, "Entry: APP SIZE: %d",entry.tapp_size);
   SYSV_LDUMP(log_handle, "Entry: Address:",entry.address, TAPP_TOC_INDEX_ENTRY_ADDR_SIZE);

   /*
     After getting the TOC information, pass the information to the platform specific
     driver loader
   */
   if ((err = sk_swld_platform_tapp_load(tapp, DB_FILE, &entry, log_handle, TAPI_TAPP_LOAD_TIMEOUT_MS)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(log_handle, SYSV_LOG_CRITICAL_ERR, "Error: Failed loading TApp through interface err=%d", err);
      err = SK_ERROR_LOAD_FAILED;
      goto DONE;
   }

DONE:
   sk_drv_swld_secure_storage_close(sh, log_handle);

   return err;
}

SK_STATUS tapp_unload(sk_swld_tapp_instance* tapp, void* log_handle)
{
   SK_STATUS err;

   if (tapp == 0)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   if ((err = sk_swld_platform_tapp_unload(tapp, log_handle, TAPI_TAPP_UNLOAD_TIMEOUT_MS)) != SK_ERROR_SUCCESS)
   {
      SYSV_ERROR1(log_handle, SYSV_LOG_CRITICAL_ERR, "Error: Failed unloading TApp through interface err=%d", err);
      err = SK_ERROR_UNLOAD_FAILED;
      goto DONE;
   }

DONE:
   return err;
}
