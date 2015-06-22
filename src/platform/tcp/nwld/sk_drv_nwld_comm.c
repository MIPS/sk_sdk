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

#include <sk_nwld.h>
#include "sk_drv_nwld_platform.h"

#define MESSAGE_HEADER_SIZE 4

static SKTEEC_STATUS my_sk_drv_nwld_comm_init(tSkSysCltCommOffTargetObj* myObj, uint32_t address, uint32_t port);
static SKTEEC_STATUS my_sk_drv_nwld_comm_end(tSkSysCltCommOffTargetObj* myObj);

/*
  user defined function
*/
static SKTEEC_STATUS my_sk_drv_nwld_comm_init(tSkSysCltCommOffTargetObj* myObj, uint32_t address, uint32_t port)
{
   SKTEEC_STATUS      err;
   struct sockaddr_in trusted_ip;
   struct sockaddr_in local;
   int32_t            flag;

   err = SK_ERROR_SUCCESS;

   trusted_ip.sin_addr.s_addr = address;
   trusted_ip.sin_family      = AF_INET;
   trusted_ip.sin_port        = port;

   if( (myObj->socket = socket( AF_INET, SOCK_STREAM, 0 )) <= 0 )
   {
      printf("Error opening socket [%d]\n",myObj->socket);
      err = -1;
      goto DONE;
   }

   flag = 1;
   setsockopt(myObj->socket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

   local.sin_addr.s_addr = INADDR_ANY;
   local.sin_family = AF_INET;
   local.sin_port = INADDR_ANY;

   if (bind(myObj->socket, (struct sockaddr *)&local, (socklen_t) sizeof(local)) < 0)
   {
      printf("Error: Bind Failed [%d].\n",errno);
      (void)close(myObj->socket);
      err = -2;
      goto DONE;
   }

   printf("trusted_ip.sin_port = %d\n", trusted_ip.sin_port);

   if ((connect(myObj->socket,(struct sockaddr *)&trusted_ip,(socklen_t)sizeof(trusted_ip))) <0)
   {
      printf("Error: Connection Failed [%d].\n",errno);
      (void)close(myObj->socket);
      err = -3;
      goto DONE;
   }

   flag = 1;
   setsockopt(myObj->socket, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int32_t));

   printf("NWLD is connected.\n");

DONE:
   return err;
}

static SKTEEC_STATUS my_sk_drv_nwld_comm_end(tSkSysCltCommOffTargetObj* myObj)
{
   SKTEEC_STATUS err;

   err = SK_ERROR_SUCCESS;

   (void)close(myObj->socket);

   return err;
}

/*
  standard protocol function
*/

SKTEEC_STATUS sk_nwld_platform_comm_register(sk_nwld_teec_comm* obj, uint32_t context_id, uint32_t sm_id)
{
   SKTEEC_STATUS              err;
   tSkSysCltCommOffTargetObj *myObj;

   err              = SK_ERROR_SUCCESS;
   obj->context_id  = context_id;
   obj->sm_id       = sm_id;
   obj->reg_init    = 1;

   myObj = (tSkSysCltCommOffTargetObj *)obj->my_obj;

   if ( (err = my_sk_drv_nwld_comm_init(myObj, myObj->address, myObj->port)) != SK_ERROR_SUCCESS)
   {
      printf("Error opening COMM driver\n");
      err = SK_ERROR_OPEN_FAILED;
   }

   return err;
}

SKTEEC_STATUS sk_nwld_platform_comm_unregister(sk_nwld_teec_comm* obj)
{
   tSkSysCltCommOffTargetObj *myObj;

   myObj = (tSkSysCltCommOffTargetObj *)obj->my_obj;

   return my_sk_drv_nwld_comm_end(myObj);
}

SKTEEC_STATUS sk_nwld_platform_comm_proc(sk_nwld_teec_comm* obj, uint8_t* send_buf, uint32_t send_buf_len, uint8_t* recv_buf, uint32_t* recv_buf_len, sk_operation* op)
{
   SKTEEC_STATUS              err;
   tSkSysCltCommOffTargetObj *myObj;
   uint32_t                   op_size;
   uint32_t                   i;
   sk_operation local_op;

   err   = SK_ERROR_SUCCESS;
   myObj = (tSkSysCltCommOffTargetObj *)obj->my_obj;

   /*
     send message header (4 bytes: packet size)
   */
   if ( (myObj->comm_err = send(myObj->socket, (char *)&send_buf_len, MESSAGE_HEADER_SIZE, 0)) != MESSAGE_HEADER_SIZE )
   {
      printf("Failed sending message header [%d]\n", myObj->comm_err);
      myObj->comm_err = MYCLTCOMMERR_SEND_MSG_FAILED;
      err = SK_ERROR_COMM_SEND_FAILED;
      goto DONE;
   }

   /*
     send the message
   */
   if ( (myObj->comm_err = send(myObj->socket, (char *)send_buf, send_buf_len, 0)) != send_buf_len )
   {
      printf("Failed sending message [%d]\n", myObj->comm_err);
      myObj->comm_err = MYCLTCOMMERR_SEND_MSG_FAILED;
      err = SK_ERROR_COMM_SEND_FAILED;
      goto DONE;
   }

   /*
     send OP structure size
   */
   if (op != 0)
   {
      op_size = sizeof(sk_operation);
   }
   else
   {
      op_size = 0;
   }

   if ( (myObj->comm_err = send(myObj->socket, (char *)&op_size, 4, 0)) != 4 )
   {
      printf("Failed sending OP message header [%d]\n", myObj->comm_err);
      myObj->comm_err = MYCLTCOMMERR_SEND_MSG_FAILED;
      err = SK_ERROR_COMM_SEND_FAILED;
      goto DONE;
   }

   if (op_size > 0)
   {
      if ( (myObj->comm_err = send(myObj->socket, (char *)op, op_size, 0)) != op_size )
      {
         printf("Failed sending OP message [%d]\n", myObj->comm_err);
         myObj->comm_err = MYCLTCOMMERR_SEND_MSG_FAILED;
         err = SK_ERROR_COMM_SEND_FAILED;
         goto DONE;
      }
      for (i = 0; i < op->num_param; i++)
      {
         if ((op->param[i].mem.flag & SKTEEC_MEMREF_OUT) && op->param[i].mem.memory_size)
         {
            //printf("Sending transport data buffer #%d (%u bytes)...\n", i, op->param[i].mem.memory_size);
            //dump_data("", (uint8_t *)op->param[i].mem.addr, op->param[i].mem.memory_size);

            if ((myObj->comm_err = send(myObj->socket, (void *)op->param[i].mem.addr, op->param[i].mem.memory_size, 0)) != op->param[i].mem.memory_size)
            {
               printf("Failed sending transport data buffer #%d [%d]\n", i, myObj->comm_err);
               myObj->comm_err = MYCLTCOMMERR_SEND_MSG_FAILED;
               err = SK_ERROR_COMM_SEND_FAILED;
               goto DONE;
            }
         }
      }
   }


   //
   // Send the data buffers that are flagged with SKTEEC_MEMREF_OUT
   //

   /*
      receive the response header
   */
   // TODO: check return length against value that ideally the user supplied
   if ((myObj->comm_err = recv(myObj->socket, (char *)recv_buf_len, MESSAGE_HEADER_SIZE, 0)) != MESSAGE_HEADER_SIZE)
   {
      printf("Failed receiving message [%d]\n", myObj->comm_err);
      myObj->comm_err = MYCLTCOMMERR_RECV_MSG_FAILED;
      err = SK_ERROR_COMM_READ_FAILED;
      goto DONE;
   }

   /*
      receive the response
   */
   if ((myObj->comm_err = recv(myObj->socket, (char *)recv_buf, *recv_buf_len, 0)) != (*recv_buf_len))
   {
      printf("Failed receiving message [%d]\n", myObj->comm_err);
      myObj->comm_err = MYCLTCOMMERR_RECV_MSG_FAILED;
      err = SK_ERROR_COMM_READ_FAILED;
      goto DONE;
   }

   /*
      receive the OP response
   */
   op_size = 0;
   if ((myObj->comm_err = recv(myObj->socket, (char *)&op_size, 4, 0)) != 4)
   {
      printf("Failed receiving OP message header [%d]\n", myObj->comm_err);
      myObj->comm_err = MYCLTCOMMERR_RECV_MSG_FAILED;
      err = SK_ERROR_COMM_READ_FAILED;
      goto DONE;
   }

   if (op_size > 0)
   {
      if (op == 0)
      {
         printf("OP not set but expected value\n");
         err = SK_ERROR_COMM_READ_FAILED;
         goto DONE;
      }

      if ((myObj->comm_err = recv(myObj->socket, (char *)&local_op, op_size, 0)) != op_size)
      {
         printf("Failed receiving OP message [%d]\n", myObj->comm_err);
         myObj->comm_err = MYCLTCOMMERR_RECV_MSG_FAILED;
         err = SK_ERROR_COMM_READ_FAILED;
         goto DONE;
      }
      op->num_param = local_op.num_param; // dirty hack

      for (i = 0; i < op->num_param; i++)
      {
         if (!(op->param[i].mem.flag & SKTEEC_MEMREF_OUT))
         {
            uint32_t nbytes;

            if ((myObj->comm_err = recv(myObj->socket, (char *)&nbytes, sizeof(nbytes), 0)) != sizeof(nbytes))
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

            if ((myObj->comm_err = recv(myObj->socket, (void *)op->param[i].mem.addr, nbytes, 0)) != nbytes)
            {
               printf("Failed receiving transport data buffer #%d [%d]\n", i, myObj->comm_err);
               myObj->comm_err = MYCLTCOMMERR_RECV_MSG_FAILED;
               err = SK_ERROR_COMM_READ_FAILED;
               goto DONE;
            }
         }
      }

   }

DONE:
   return err;
}
