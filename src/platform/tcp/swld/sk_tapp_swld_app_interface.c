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
#include "sk_tapp_swld_app_interface.h"
#include "sk_tapp_swld_app_interface_elf.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sk_swld.h>

// why this isn't in a header...
void SKTAPP_getapi(sktapp_func_def *api);

extern char *ramdisk_path;

SK_STATUS sk_swld_platform_tapp_load(sk_swld_tapp_instance* tapp, char* db, tapp_toc_entry* entry, void* log_handle, uint32_t timeout_ms)
{
   SK_STATUS                  err;
   uint32_t                   address;
   FILE                       *fp_db;
   tapp_elf_tapp_interface *obj;
   void                       *manifest;
   uintptr_t                   p;
   char                     file[256];
   uint32_t                    code_size;
   (void)log_handle;
   (void)timeout_ms;

   //printf("sk_tapp_swld_app_interface_load()\n");

   if ((tapp == NULL) || (entry == NULL) || (db == NULL))
   {
      return SK_ERROR_INVALID_PARAM;
   }

   obj            = NULL;
   manifest       = NULL;
   fp_db          = NULL;
   err            = SK_ERROR_SUCCESS;
   tapp->arch_obj = NULL;

   do
   {
      //-------------------------------------------------------------
      // Allocate memory for the object, manifest and the executable
      //-------------------------------------------------------------

      if ((obj = malloc(sizeof(tapp_elf_tapp_interface))) == NULL)
      {
         //printf("Unable to allocate memory for object\n");
         err = SK_ERROR_NO_MEMORY;
         break;
      }

      memset(obj, 0, sizeof(tapp_elf_tapp_interface));

      if ((manifest = malloc(TAPPS_INFO_DATA_SIZE)) == NULL)
      {
         //printf("Unable to allocate memory for manifest\n");
         err = SK_ERROR_NO_MEMORY;
         break;
      }

      if ((obj->mem_base = malloc((entry->tapp_size - TAPPS_INFO_DATA_SIZE) + 8192)) == NULL)
      {
         //printf("Unable to allocate memory for the application code\n");
         err = SK_ERROR_NO_MEMORY;
         break;
      }

      //-------------------
      // Read the manifest
      //-------------------

      strcpy(file, ramdisk_path);
      strcat(file, db);

      // Open the database
      if ((fp_db = fopen(file, "rb")) == 0)
      {
         //printf("Unable to open database file %s\n", file);
         err = SK_ERROR_OPEN_FAILED;
         break;
      }

      // Address of the manifest within the database
      address  = (entry->address[0] << 24);
      address |= (entry->address[1] << 16);
      address |= (entry->address[2] << 8);
      address |= (entry->address[3] << 0);

      // Seek and read the manifest
      if (fseek(fp_db, address, SEEK_SET) < 0)
      {
         //printf("Failed to seek manifest at offset %u of the database\n", address);
         err = SK_ERROR_READ_FAILED;
         break;
      }

      if (fread(manifest, 1, TAPPS_INFO_DATA_SIZE, fp_db) != TAPPS_INFO_DATA_SIZE)
      {
         //printf("Error reading manifest from the database\n");
         err = SK_ERROR_READ_FAILED;
         break;
      }

      // Extracts the object's information from the manifest
      tapp_manifest_pack_get(manifest, TAPPS_INFO_DATA_SIZE, &obj->info);


      code_size = entry->tapp_size - TAPPS_INFO_DATA_SIZE;


      //------------------------------
      // Load the trusted application
      //------------------------------

      // Align the code base on a 4K boundary
      p = (uintptr_t)obj->mem_base;

      if (p & 4095)
      {
         p += 4096 - (p & 4095);
      }

      obj->code = (void*)p;

      // Read application's code from the database
      if (fread(obj->code, 1, code_size, fp_db) != code_size)
      {
         //printf("Error reading relocatable ELF from the database\n");
         err = SK_ERROR_READ_FAILED;
         break;
      }

      obj->tapp_iface.offset = (intptr_t)obj->code;
      //printf("Requesting SKTAPP API from the trusted application (calling %p, api=%p, size=%d)...\n",
      //       obj->func, &obj->tapp_iface.def, sizeof(obj->tapp_iface.def));
      //obj->func(&obj->tapp_iface.def);
      SKTAPP_getapi(&obj->tapp_iface.def);
      //printf("Got the SKTAPP API from the trusted application.\n");
      obj->def = &obj->tapp_iface.def;
      tapp->arch_obj = obj;
   } while (0);

   if (fp_db)
   {
      fclose(fp_db);
   }

   if (manifest)
   {
      free(manifest);
   }

   if (err != SK_ERROR_SUCCESS)
   {
      if (obj)
      {
         if (obj->mem_base)
         {
            free(obj->mem_base);
         }

         free(obj);
         tapp->arch_obj = NULL;
      }
   }

   return err;
}

SK_STATUS sk_swld_platform_tapp_unload(sk_swld_tapp_instance* tapp, void* log_handle, uint32_t timeout_ms)
{
   //printf("sk_swld_platform_tapp_unload()\n");
   (void)log_handle;
   (void)timeout_ms;

   if (!tapp)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   if (tapp->arch_obj)
   {
      tapp_elf_tapp_interface *obj;

      obj = (tapp_elf_tapp_interface *)tapp->arch_obj;
      free(obj->mem_base);
      free(obj);
      tapp->arch_obj = NULL;
   }

   return SK_ERROR_SUCCESS;
}

SK_TA_STATUS sk_swld_platform_tapp_init(sk_swld_tapp_instance* tapp, void* session, void* log_handle, uint32_t timeout_ms)
{
   SK_STATUS                 err;
   tapp_elf_tapp_interface * obj;
   session_type              s;

   (void)log_handle;
   (void)timeout_ms;
   (void)session;

   //printf("sk_swld_platform_tapp_init()\n");

   if (!tapp)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   obj = (tapp_elf_tapp_interface *)tapp->arch_obj;
   err = obj->def->SKTAPP_init_iface(&s);
   //printf("SKTAPP_init_iface() returned %d\n", err);

   return err;
}

SK_TA_STATUS sk_swld_platform_tapp_close(sk_swld_tapp_instance* tapp, void* session, void* log_handle, uint32_t timeout_ms)
{
   SK_STATUS                   err;
   tapp_elf_tapp_interface *obj;
   session_type                s;

   (void)log_handle;
   (void)timeout_ms;
   (void)session;

   if (!tapp)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   obj = (tapp_elf_tapp_interface *)tapp->arch_obj;
   err = obj->def->SKTAPP_close_iface(&s);

   return err;
}

SK_TA_STATUS sk_swld_platform_tapp_pull(sk_swld_tapp_instance* tapp, void* session, uint32_t cmd_id, sk_operation* op, void* log_handle, uint32_t timeout_ms)
{
   SK_STATUS                   err;
   tapp_elf_tapp_interface *obj;
   session_type                s;

   (void)log_handle;
   (void)timeout_ms;
   (void)session;

   if (!tapp)
   {
      return SK_ERROR_INVALID_PARAM;
   }

   obj = (tapp_elf_tapp_interface *)tapp->arch_obj;

// NOTE: obj->info.taid[] == TAID of application

   err = obj->def->SKTAPP_receive_operation_iface(obj->info.taid, &s, cmd_id, op);

   return err;
}
