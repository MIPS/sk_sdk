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
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <sk_nwld.h>
#include <sk_drv_nwld_platform.h>

static SKTEEC_STATUS my_sk_drv_nwld_comm_init(tSkSysCltCommVmFiascoObj* myObj, char *device_name)
{
   char ext_device_name[60];

   myObj->device_rx = -1;
   myObj->device_tx = -1;

   strcpy(ext_device_name, device_name);
   //strcat(ext_device_name, "_rx");
   myObj->device_rx = open(ext_device_name, O_RDWR);

   if (myObj->device_rx < 0)
   {
      printf("my_sk_drv_nwld_comm_init(): Failed to open receive device '%s'.\n", ext_device_name);
      return SK_ERROR_DRIVER_OPEN_FAILED;
   }

   printf("my_sk_drv_nwld_comm_init(): Receive device '%s' has been opened (%d).\n",
          ext_device_name, myObj->device_rx);

   /*
   strcpy(ext_device_name, device_name);
   strcat(ext_device_name, "_tx");
   myObj->device_tx = open(ext_device_name, O_RDWR);

   if (myObj->device_tx < 0)
   {
      printf("my_sk_drv_nwld_comm_init(): Failed to open transmit device '%s'.\n", ext_device_name);
      close(myObj->device_rx);
      myObj->device_rx = -1;
      return SK_ERROR_DRIVER_OPEN_FAILED;
   }
   */

   myObj->device_tx = myObj->device_rx;

   printf("my_sk_drv_nwld_comm_init(): Transmit device '%s' has been opened (%d).\n",
          ext_device_name, myObj->device_tx);

   return SK_ERROR_SUCCESS;
}

static SKTEEC_STATUS my_sk_drv_nwld_comm_end(tSkSysCltCommVmFiascoObj* myObj)
{
   if ((myObj->device_tx > 0) && (myObj->device_tx != myObj->device_rx))
   {
      close(myObj->device_tx);
      myObj->device_tx = -1;
   }

   if (myObj->device_rx >= 0)
   {
      close(myObj->device_rx);
      myObj->device_rx = -1;
   }

   return SK_ERROR_SUCCESS;
}

SKTEEC_STATUS sk_nwld_platform_comm_register(sk_nwld_teec_comm* obj, uint32_t context_id, uint32_t sm_id)
{
   SKTEEC_STATUS               err;
   tSkSysCltCommVmFiascoObj *myObj;

   obj->context_id  = context_id;
   obj->sm_id       = sm_id;
   obj->reg_init    = 1;

   myObj = (tSkSysCltCommVmFiascoObj *)obj->my_obj;

   if ((err = my_sk_drv_nwld_comm_init(myObj, myObj->device_name)) != SK_ERROR_SUCCESS)
   {
      printf("Error opening communication driver.\n");
      err = SK_ERROR_OPEN_FAILED;
   }

   return err;
}

SKTEEC_STATUS sk_nwld_platform_comm_unregister(sk_nwld_teec_comm* obj)
{
   tSkSysCltCommVmFiascoObj *myObj;

   myObj = (tSkSysCltCommVmFiascoObj *)obj->my_obj;

   return my_sk_drv_nwld_comm_end(myObj);
}

// Test
int read_device(int dev, char *buf, int nbytes)
{
   int result;
   char *p = buf;
   int left_bytes = nbytes;

   while (left_bytes)
   {
      //printf("Reading data...\n");
      result = read(dev, p, left_bytes);

      if (result < 0)
      {
         return 0;
      }

      p += result;
      left_bytes -= result;
      //printf("Read %d bytes\n", result);
   }

   return nbytes;
}


SKTEEC_STATUS sk_nwld_platform_comm_proc(sk_nwld_teec_comm* obj, uint8_t* send_buf, uint32_t send_buf_len, uint8_t* recv_buf, uint32_t* recv_buf_len, sk_operation* op)
{
   unsigned                           i;
   uint32_t                 nbytes;
   SKTEEC_STATUS               err;
   tSkSysCltCommVmFiascoObj *myObj;
   uint32_t                op_size;

   err   = SK_ERROR_SUCCESS;
   myObj = (tSkSysCltCommVmFiascoObj *)obj->my_obj;

   if (flock(myObj->device_tx, LOCK_EX)) {
      perror("Cannot lock file");
      return SK_ERROR_COMM_SEND_FAILED;
   }

   //printf("ENTER sk_nwld_platform_comm_proc()\n");

   //
   // Send the control data
   //
   //printf("Send control buffer size = %u bytes\n", send_buf_len);
   //printf("Receive control buffer size = %u bytes\n", *recv_buf_len);
   //printf("Sending control message header (%u bytes)...\n", sizeof(send_buf_len));

   if ((myObj->comm_err = write(myObj->device_tx, &send_buf_len, sizeof(send_buf_len))) != sizeof(send_buf_len))
   {
      printf("Failed sending control message header [%d]\n", myObj->comm_err);
      myObj->comm_err = MYCLTCOMMERR_SEND_MSG_FAILED;
      err = SK_ERROR_COMM_SEND_FAILED;
      goto DONE;
   }

   //printf("Sending control message (%u bytes)...\n", send_buf_len);
   //dump_data("", send_buf, send_buf_len);

   if ((myObj->comm_err = write(myObj->device_tx, send_buf, send_buf_len)) != send_buf_len)
   {
      printf("Failed sending control message [%d]\n", myObj->comm_err);
      myObj->comm_err = MYCLTCOMMERR_SEND_MSG_FAILED;
      err = SK_ERROR_COMM_SEND_FAILED;
      goto DONE;
   }

   //
   // Send the SK operation
   //
   if (op != NULL)
   {
      op_size = sizeof(sk_operation);

#if 0
      printf("Control operation to be sent:\n");
      printf("    num_param=%u\n", op->num_param);

      for (i = 0; i < op->num_param; i++)
      {
         printf("    param[%d]=\n", i);
         printf("       memory_size=%u\n", op->param[i].mem.memory_size);
         printf("       flag=0x%x\n", op->param[i].mem.flag);
         printf("       addr=0x%x\n", op->param[i].mem.addr);
      }
#endif
   }
   else
   {
      op_size = 0;
      //printf("No control operation provided\n");
   }

   //
   // Send the SK operation header
   //
   //printf("Sending the control operation header (%u bytes)...\n", sizeof(op_size));

   if ((myObj->comm_err = write(myObj->device_tx, &op_size, sizeof(op_size))) != sizeof(op_size))
   {
      printf("Failed sending control operation header [%d]\n", myObj->comm_err);
      myObj->comm_err = MYCLTCOMMERR_SEND_MSG_FAILED;
      err = SK_ERROR_COMM_SEND_FAILED;
      goto DONE;
   }

   if (op_size)
   {
      //printf("Sending the control operation (%u bytes)...\n", op_size);

      if ((myObj->comm_err = write(myObj->device_tx, op, op_size)) != op_size)
      {
         printf("Failed sending the control operation [%d]\n", myObj->comm_err);
         myObj->comm_err = MYCLTCOMMERR_SEND_MSG_FAILED;
         err = SK_ERROR_COMM_SEND_FAILED;
         goto DONE;
      }

      //
      // Send the data buffers that are flagged with SKTEEC_MEMREF_OUT
      //
      //printf("Sending the transport data buffers...\n");

      for (i = 0; i < op->num_param; i++)
      {
         if ((op->param[i].mem.flag & SKTEEC_MEMREF_OUT) &&
             op->param[i].mem.memory_size)
         {
            //printf("Sending transport data buffer #%d (%u bytes)...\n", i, op->param[i].mem.memory_size);
            //dump_data("", (uint8_t *)op->param[i].mem.addr, op->param[i].mem.memory_size);

            if ((myObj->comm_err = write(myObj->device_tx, (void *)op->param[i].mem.addr,
                op->param[i].mem.memory_size)) != op->param[i].mem.memory_size)
            {
               printf("Failed sending transport data buffer #%d [%d]\n", i, myObj->comm_err);
               myObj->comm_err = MYCLTCOMMERR_SEND_MSG_FAILED;
               err = SK_ERROR_COMM_SEND_FAILED;
               goto DONE;
            }
         }
      }
   }

   //printf("**** fsync() ****\n");
   //fsync(myObj->device_tx);

   //
   // Receive the response header
   //
   //printf("Receiving control response header (%u bytes)...\n", sizeof(uint32_t));

   if ((myObj->comm_err = read(myObj->device_rx, (char *)&nbytes, sizeof(nbytes))) != sizeof(nbytes))
   {
      printf("Failed receiving control response header [%d]\n", myObj->comm_err);
      myObj->comm_err = MYCLTCOMMERR_RECV_MSG_FAILED;
      err = SK_ERROR_COMM_READ_FAILED;
      goto DONE;
   }

   if (nbytes > *recv_buf_len)
   {
      printf("Control response too large for response buffer. Wants to store %u bytes in a buffer of size %u bytes.\n",
             nbytes, *recv_buf_len);
      myObj->comm_err = MYCLTCOMMERR_RECV_MSG_FAILED;
      err = SK_ERROR_COMM_READ_FAILED;
      goto DONE;
   }

   *recv_buf_len = nbytes;

   //
   // Receive the response
   //
   //printf("Receiving control response (%u bytes)...\n", *recv_buf_len);

   if ((myObj->comm_err = read(myObj->device_rx, recv_buf, *recv_buf_len)) != *recv_buf_len)
   {
      printf("Failed receiving control response [%d]\n", myObj->comm_err);
      myObj->comm_err = MYCLTCOMMERR_RECV_MSG_FAILED;
      err = SK_ERROR_COMM_READ_FAILED;
      goto DONE;
   }

   //printf("Received control response:\n");
   //dump_data("", recv_buf, *recv_buf_len);

   if (op_size)
   {
      //
      // Fill the receive buffers with the data received from the server
      //
      //printf("Receiving transport data buffers from the server...\n");

      for (i = 0; i < op->num_param; i++)
      {
         if (!(op->param[i].mem.flag & SKTEEC_MEMREF_OUT))
         {
            uint32_t nbytes;

            //printf("Receiving size of transport data buffer #%d (%u bytes)...\n", i, sizeof(nbytes));

            if ((myObj->comm_err = read_device(myObj->device_rx, (char *)&nbytes, sizeof(nbytes))) != sizeof(nbytes))
            {
               printf("Failed receiving size of transport data buffer #%d [%d]\n", i, myObj->comm_err);
               myObj->comm_err = MYCLTCOMMERR_RECV_MSG_FAILED;
               err = SK_ERROR_COMM_READ_FAILED;
               goto DONE;
            }

            if (nbytes > op->param[i].mem.memory_size)
            {
               printf("Data too large for transport data buffer #%d. Wants to store %u bytes in a buffer of size %u bytes.\n",
                      i, nbytes, op->param[i].mem.memory_size);
               myObj->comm_err = MYCLTCOMMERR_RECV_MSG_FAILED;
               err = SK_ERROR_COMM_READ_FAILED;
               goto DONE;
            }

            //printf("Receiving transport data buffer #%d (%u bytes)...\n", i, nbytes);

            if ((myObj->comm_err = read_device(myObj->device_rx, (void *)op->param[i].mem.addr, nbytes)) != nbytes)
            {
               printf("Failed receiving transport data buffer #%d [%d]\n", i, myObj->comm_err);
               myObj->comm_err = MYCLTCOMMERR_RECV_MSG_FAILED;
               err = SK_ERROR_COMM_READ_FAILED;
               goto DONE;
            }

            //dump_data("Received transport data buffer", (uint8_t *)op->param[i].mem.addr, nbytes);
         }
      }
   }

DONE:
   //printf("EXIT sk_nwld_platform_comm_proc() status=%d\n", err);
   flock(myObj->device_tx, LOCK_UN);

   return err;
}

