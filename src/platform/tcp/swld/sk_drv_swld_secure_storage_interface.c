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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _my_secure_storage_
{
   FILE* fp;
}my_secure_storage;

char *ramdisk_path;

/*
  System Defined Functions
*/
tSkDrvSSFileHandle* sk_drv_swld_secure_storage_open(char* file, uint32_t flag, void* log_handle)
{
   tSkDrvSSFileHandle* ssfd;
   my_secure_storage*  my_fd;
   char             file_loc[256];
   (void)flag;
   (void)log_handle;


   file_loc[0] = 0;
   strcat(file_loc, ramdisk_path);
   strcat(file_loc, file);

   ssfd = (tSkDrvSSFileHandle *)malloc(sizeof(tSkDrvSSFileHandle));

   if (ssfd == 0)
   {
      printf("no memory for SS file handle\n");
      return 0;
   }

   my_fd = (my_secure_storage *)malloc(sizeof(my_secure_storage));

   if (my_fd == 0)
   {
      printf("no memory for my file handle\n");
      free(ssfd);
      return 0;
   }

   if ((my_fd->fp = fopen(file_loc, "rb")) == 0)
   {
      printf("Error opening file [%s]\n", file_loc);
      free(ssfd);
      free(my_fd);
      return NULL;
   }

   ssfd->tObj = my_fd;

   return ssfd;
}

SK_STATUS sk_drv_swld_secure_storage_close(tSkDrvSSFileHandle* handle, void* log_handle)
{
   SK_STATUS          err;
   my_secure_storage* my_fd;
   (void)log_handle;

   err = SK_ERROR_SUCCESS;
   my_fd = (my_secure_storage *)handle->tObj;

   fclose(my_fd->fp);

   free(my_fd);
   free(handle);

   return err;
}

uint32_t sk_drv_swld_secure_storage_write(tSkDrvSSFileHandle* handle, uint8_t* buffer, uint32_t buffer_size, void* log_handle)
{
   my_secure_storage* my_fd;
   (void)log_handle;

   my_fd = (my_secure_storage *)handle->tObj;

   return fwrite(buffer, 1, buffer_size, my_fd->fp);
}

uint32_t sk_drv_swld_secure_storage_read(tSkDrvSSFileHandle* handle, uint8_t* buffer, uint32_t buffer_size, void* log_handle)
{
   my_secure_storage* my_fd;
   (void)log_handle;

   my_fd = (my_secure_storage *)handle->tObj;

   return fread(buffer, 1, buffer_size, my_fd->fp);
}
