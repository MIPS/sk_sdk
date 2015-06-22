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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sk_both.h>
#include "sk_drv_server_comm.h"

#define MESSAGE_HEADER_SIZE 4

int open2(char *name, int mode);
int close2(int fd);
int read2(int fd, void *buf, size_t nbyte);
int write2(int fd, void *buf, size_t nbyte);

/*
  user defined function
*/
SK_STATUS sk_swld_platform_comm_init(tSkSysSrvCommVmFiascoObj* myObj, char *device_name)
{
   char ext_device_name[60];

   myObj->device_rx = -1;
   myObj->device_tx = -1;

   // Note: RX/TX swapped for secure world
   strcpy(ext_device_name, device_name);
   strcat(ext_device_name, "_tx");
   myObj->device_rx = open2(ext_device_name, O_RDWR);

   if (myObj->device_rx < 0)
   {
      printf("sk_swld_platform_comm_init(): Failed to open receive device '%s'.\n", ext_device_name);
      return SK_ERROR_OPEN_FAILED;
   }

   printf("sk_swld_platform_comm_init(): Receive '%s' has been opened (%d).\n",
          ext_device_name, myObj->device_rx);

   strcpy(ext_device_name, device_name);
   strcat(ext_device_name, "_rx");
   myObj->device_tx = open2(ext_device_name, O_RDWR);

   if (myObj->device_tx < 0)
   {
      printf("sk_swld_platform_comm_init(): Failed to open transmit device '%s'.\n", ext_device_name);
      close2(myObj->device_rx);
      myObj->device_rx = -1;
      return SK_ERROR_OPEN_FAILED;
   }

   printf("sk_swld_platform_comm_init(): Transmit device '%s' has been opened (%d).\n",
          ext_device_name, myObj->device_tx);

   return SK_ERROR_SUCCESS;
}

SK_STATUS sk_swld_platform_comm_end(tSkSysSrvCommVmFiascoObj* myObj)
{
   if (myObj->device_rx >= 0)
   {
      close2(myObj->device_rx);
      myObj->device_rx = -1;
   }

   if (myObj->device_tx >= 0)
   {
      close2(myObj->device_tx);
      myObj->device_tx = -1;
   }

   return SK_ERROR_SUCCESS;
}

int32_t sk_swld_platform_comm_recv(void *pobj, uint8_t* buf, uint32_t* buf_len, sk_operation* op)
{
   unsigned              i;
   SK_STATUS      err;
   uint32_t   op_size;
   tSkSysSrvCommVmFiascoObj* obj;

   obj = pobj;
   err           = SK_ERROR_SUCCESS;
   obj->comm_err = MYSRVCOMMERR_SUCCESS;

   //
   // Receive message header
   //

   if ((obj->comm_err = read2(obj->device_rx, (char *)buf_len, MESSAGE_HEADER_SIZE)) != MESSAGE_HEADER_SIZE)
   {
      printf("Failed receiving control message header [%d]\n", obj->comm_err);
      obj->comm_err = MYSRVCOMMERR_RECV_MSG_FAILED;
      err = SK_ERROR_COMM_READ_FAILED;
      goto DONE;
   }

   //
   // Receive message
   //

   if ((obj->comm_err = read2(obj->device_rx, (char *)buf, *buf_len)) != *buf_len)
   {
      printf("Failed receiving control message [%d]\n", obj->comm_err);
      obj->comm_err = MYSRVCOMMERR_RECV_MSG_FAILED;
      err = SK_ERROR_COMM_READ_FAILED;
      goto DONE;
   }

   //
   // Receive SK operation header
   //

   if ((obj->comm_err = read2(obj->device_rx, &op_size, sizeof(op_size))) != sizeof(op_size))
   {
      printf("Failed receiving control operation header [%d]\n", obj->comm_err);
      obj->comm_err = MYSRVCOMMERR_RECV_MSG_FAILED;
      err = SK_ERROR_COMM_READ_FAILED;
      goto DONE;
   }

   if (op_size)
   {
      //
      // Receive SK operation
      //
      if ((obj->comm_err = read2(obj->device_rx, op, op_size)) != op_size)
      {
         printf("Failed receiving control operation [%d]\n", obj->comm_err);
         obj->comm_err = MYSRVCOMMERR_RECV_MSG_FAILED;
         err = SK_ERROR_COMM_READ_FAILED;
         goto DONE;
      }

      //
      // Receive the memory buffers that are flagged with SKTEEC_MEMREF_OUT
      //
      for (i = 0; i < op->num_param; i++)
      {
         // Allocate the local data buffers
         if (op->param[i].mem.memory_size)
         {
            op->param[i].mem.addr = (SK_ADDRESS_TYPE)malloc(op->param[i].mem.memory_size);
         }
         else
         {
            op->param[i].mem.addr = 0;
         }

         if ((op->param[i].mem.flag & SKTEEC_MEMREF_OUT) &&
             op->param[i].mem.memory_size)
         {
            if ((obj->comm_err = read2(obj->device_rx, (void *)op->param[i].mem.addr,
                op->param[i].mem.memory_size)) != op->param[i].mem.memory_size)
            {
               obj->comm_err = MYSRVCOMMERR_RECV_MSG_FAILED;
               err = SK_ERROR_COMM_READ_FAILED;
               goto DONE;
            }
         }
      }
   }

DONE:
   return err;
}

int32_t sk_swld_platform_comm_send(void *pobj, uint8_t* buf, uint32_t buf_len, sk_operation* op)
{
   unsigned         i;
   SK_STATUS err;
   tSkSysSrvCommVmFiascoObj *obj;

   obj = pobj;
   err = SK_ERROR_SUCCESS;
   if ((obj->comm_err = write2(obj->device_tx, (char *)&buf_len, sizeof(buf_len))) != sizeof(buf_len) )
   {
      printf("Failed sending control response header[%d]\n", obj->comm_err);
      obj->comm_err = MYSRVCOMMERR_SEND_MSG_FAILED;
      err           = SK_ERROR_COMM_SEND_FAILED;
      goto DONE;
   }

   if ((obj->comm_err = write2(obj->device_tx, (char *)buf, buf_len)) != buf_len)
   {
      printf("Failed sending control response [%d]\n", obj->comm_err);
      obj->comm_err = MYSRVCOMMERR_SEND_MSG_FAILED;
      err           = SK_ERROR_COMM_SEND_FAILED;
      goto DONE;
   }

   if (op)
   {
      //
      // Sends data buffers back to the client
      //
      for (i = 0; i < op->num_param; i++)
      {
         if (!(op->param[i].mem.flag & SKTEEC_MEMREF_OUT))
         {
            if ((obj->comm_err = write2(obj->device_tx, &op->param[i].mem.memory_size,
                sizeof(uint32_t))) != sizeof(uint32_t))
            {
               obj->comm_err = MYSRVCOMMERR_SEND_MSG_FAILED;
               err = SK_ERROR_COMM_SEND_FAILED;
               goto DONE;
            }

            if ((obj->comm_err = write2(obj->device_tx, (void *)op->param[i].mem.addr,
                op->param[i].mem.memory_size)) != op->param[i].mem.memory_size)
            {
               printf("Failed sending transport data buffer #%d [%d]\n", i, obj->comm_err);
               obj->comm_err = MYSRVCOMMERR_SEND_MSG_FAILED;
               err = SK_ERROR_COMM_SEND_FAILED;
               goto DONE;
            }
         }

         if (op->param[i].mem.addr)
         {
            // Free the local data buffer
            free((void *)op->param[i].mem.addr);
         }
      }
   }

DONE:
   return err;
}
