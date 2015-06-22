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
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <errno.h>

#include <sk_both.h>
#include "sk_drv_server_comm.h"

#define MESSAGE_HEADER_SIZE 4

/*
  user defined function
*/
SK_STATUS sk_swld_platform_comm_init(tSkSysSrvCommOffTargetObj* myObj, uint32_t port)
{
   int32_t            err;
   struct sockaddr_in info;
   struct sockaddr_in sa;
   uint32_t           sa_size;
   int32_t            flag;

   err                  = 0;
   myObj->master_socket = socket(AF_INET, SOCK_STREAM, 0);

   if (myObj->master_socket <= 0)
   {
      printf("Socket Error [%d]\n", errno);
      err = -1;
      goto DONE;
   }

   info.sin_addr.s_addr = INADDR_ANY;
   info.sin_family      = AF_INET;
   info.sin_port        = port;

   printf("info.sin_port = %d\n", info.sin_port);

   if( bind(myObj->master_socket, (struct sockaddr *)&info, (socklen_t)sizeof(info) ) == -1 )
   {
      printf("Error binding to socket [%d]\n", errno);
      err = -2;
      goto DONE;
   }

   flag = 1;
   setsockopt(myObj->master_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int32_t));

   if (listen(myObj->master_socket, 1) == -1)
   {
      printf("Error listening to socket [%d]\n", errno);
      err = -3;
      goto DONE;
   }

   sa_size = sizeof(sa);
   if ((myObj->socket = accept(myObj->master_socket, (struct sockaddr *)&sa, (socklen_t *)&sa_size)) <= 0)
   {
      printf("Error accepting connection [%d]\n", errno);
      err = -4;
      goto DONE;
   }


DONE:
   return err;
}

SK_STATUS sk_swld_platform_comm_end(tSkSysSrvCommOffTargetObj* myObj)
{
   close(myObj->socket);
   close(myObj->master_socket);

   return 0;
}

int32_t sk_swld_platform_comm_recv(void* pobj, uint8_t* buf, uint32_t* buf_len, sk_operation* op)
{
   SK_STATUS err;
   uint32_t  op_size, i;
   tSkSysSrvCommOffTargetObj *obj = pobj;

   err           = SK_ERROR_SUCCESS;
   obj->comm_err = MYSRVCOMMERR_SUCCESS;

   if ((obj->comm_err = recv(obj->socket, (char *)buf_len, MESSAGE_HEADER_SIZE, 0)) != MESSAGE_HEADER_SIZE)
   {
      printf("Failed receiving message header [%d]\n", obj->comm_err);
      obj->comm_err = MYSRVCOMMERR_RECV_MSG_FAILED;
      err = SK_ERROR_COMM_READ_FAILED;
   }

   if ((obj->comm_err = recv(obj->socket, (char *)buf, *buf_len, 0)) != (*buf_len))
   {
      printf("Failed receiving message [%d]\n", obj->comm_err);
      obj->comm_err = MYSRVCOMMERR_RECV_MSG_FAILED;
      err = SK_ERROR_COMM_READ_FAILED;
   }

   if ((obj->comm_err = recv(obj->socket, (char *)&op_size, 4, 0)) != 4)
   {
      printf("Failed receiving message [%d]\n", obj->comm_err);
      obj->comm_err = MYSRVCOMMERR_RECV_MSG_FAILED;
      err = SK_ERROR_COMM_READ_FAILED;
   }

   if (op_size > 0)
   {
      if ((obj->comm_err = recv(obj->socket, (char *)op, op_size, 0)) != op_size)
      {
         printf("Failed receiving message [%d]\n", obj->comm_err);
         obj->comm_err = MYSRVCOMMERR_RECV_MSG_FAILED;
         err = SK_ERROR_COMM_READ_FAILED;
      }

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

         if ((op->param[i].mem.flag & SKTEEC_MEMREF_OUT) && op->param[i].mem.memory_size)
         {
            //printf("Receiving transport data buffer #%d (%u bytes)...\n", i, op->param[i].mem.memory_size);

            if ((obj->comm_err = recv(obj->socket, (void *)op->param[i].mem.addr, op->param[i].mem.memory_size, 0)) != op->param[i].mem.memory_size)
            {
               printf("Failed receiving transport data buffer #%d [%d]\n", i, obj->comm_err);
               obj->comm_err = MYSRVCOMMERR_RECV_MSG_FAILED;
               err = SK_ERROR_COMM_READ_FAILED;
               goto DONE;
            }

            //dump_data("Received transport data:", (uint8_t *)op->param[i].mem.addr, op->param[i].mem.memory_size);
         }
      }
   }
DONE:
   return err;
}



int32_t sk_swld_platform_comm_send(void* pobj, uint8_t* buf, uint32_t buf_len, sk_operation* op)
{
   SK_STATUS err;
   uint32_t  op_size, i;
   tSkSysSrvCommOffTargetObj *obj = pobj;

   err    = SK_ERROR_SUCCESS;

   if ( (obj->comm_err = send(obj->socket, (char *)&buf_len, MESSAGE_HEADER_SIZE, 0)) != MESSAGE_HEADER_SIZE )
   {
      printf("Failed sending message header[%d]\n", obj->comm_err);
      obj->comm_err = MYSRVCOMMERR_SEND_MSG_FAILED;
      err           = SK_ERROR_COMM_SEND_FAILED;
   }

   if ( (obj->comm_err = send(obj->socket, (char *)buf, buf_len, 0)) != buf_len )
   {
      printf("Failed sending message [%d]\n", obj->comm_err);
      obj->comm_err = MYSRVCOMMERR_SEND_MSG_FAILED;
      err           = SK_ERROR_COMM_SEND_FAILED;
   }

   if (op->num_param > 0)
   {
      op_size = sizeof(sk_operation);
   }
   else
   {
      op_size = 0;
   }

   if ( (obj->comm_err = send(obj->socket, (char *)&op_size, 4, 0)) != 4 )
   {
      printf("Failed sending message [%d]\n", obj->comm_err);
      obj->comm_err = MYSRVCOMMERR_SEND_MSG_FAILED;
      err           = SK_ERROR_COMM_SEND_FAILED;
   }

   if (op_size > 0)
   {
      if ( (obj->comm_err = send(obj->socket, (char *)op, op_size, 0)) != op_size )
      {
         printf("Failed sending message [%d]\n", obj->comm_err);
         obj->comm_err = MYSRVCOMMERR_SEND_MSG_FAILED;
         err           = SK_ERROR_COMM_SEND_FAILED;
      }

      for (i = 0; i < op->num_param; i++)
      {
         if (!(op->param[i].mem.flag & SKTEEC_MEMREF_OUT))
         {
            //printf("Sending header of transport data buffer #%d...\n", i, sizeof(uint32_t));

            if ((obj->comm_err = send(obj->socket, &op->param[i].mem.memory_size,
                sizeof(uint32_t), 0)) != sizeof(uint32_t))
            {
               printf("Failed sending size of transport data buffer #%d [%d]\n", i, obj->comm_err);
               obj->comm_err = MYSRVCOMMERR_SEND_MSG_FAILED;
               err = SK_ERROR_COMM_SEND_FAILED;
               goto DONE;
            }

            //printf("Sending transport data buffer #%d (%u bytes)...\n", i, op->param[i].mem.memory_size);
            if ((obj->comm_err = send(obj->socket, (void *)op->param[i].mem.addr,
                op->param[i].mem.memory_size, 0)) != op->param[i].mem.memory_size)
            {
               printf("Failed sending transport data buffer #%d [%d]\n", i, obj->comm_err);
               obj->comm_err = MYSRVCOMMERR_SEND_MSG_FAILED;
               err = SK_ERROR_COMM_SEND_FAILED;
               goto DONE;
            }

            // dump_data("Sent transport data:", (uint8_t *)op->param[i].mem.addr, op->param[i].mem.memory_size);
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






