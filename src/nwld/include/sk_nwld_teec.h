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


#ifndef _SK_TEEC_API_
#define _SK_TEEC_API_

/**
  \defgroup TEECApi  Normal World Client API
  \addtogroup TEECApi
  @{
*/
/*
  Data Structures
*/
typedef struct _SKTEEC_SharedMemory_
{
   uint32_t        memory_size;
   uint32_t        flag;
   void            *buffer;
   uint8_t         id[SK_TAPI_MEMORY_ID_LENGTH];
}SKTEEC_SharedMemory;

typedef struct _SKTEEC_Context_
{
   int32_t  device;
   SKTEEC_CONTEXT_HANDLE handle;
}SKTEEC_Context;

typedef struct _SKTEEC_Session_
{
   SKTEEC_SESSION_HANDLE handle;
   SKTEEC_Context*       context;
}SKTEEC_Session;

typedef struct _SKTEEC_param_
{
   SKTEEC_SharedMemory* meminfo;
}SKTEEC_param;

typedef struct _SKTEEC_Operation_
{
   uint32_t     num_param;
   SKTEEC_param param[SK_MAXIMUM_OPERATIONS];
}SKTEEC_Operation;

/**
* \details
* Opens a context to the secure world.
*
* \param [in] device_name              -- Unused
* \param [in] contextID                -- Context Identifier of SWLD server you wish to talk to
* \param [in] opaqueCredentials        -- Unused
* \param [out] context                 -- Initializes the context structure for use by #SKTEEC_OpenSession which should be your next call.
*
* \return
*     - #SK_ERROR_SUCCESS on success
*/
SKTEEC_STATUS SKTEEC_OpenContext          (SKTEEC_Context* context, const char* device_name, uint32_t contextID, void* opaqueCredentials);

/**
* \details
* Close a context
*
* \param [in] context
*
*/
SKTEEC_STATUS SKTEEC_CloseContext         (SKTEEC_Context* context);

/**
* \details
* Open a session on a given secure world context (opened by #SKTEEC_OpenContext) to a given trusted application
*
* \param [in] context              -- Open Context to secure world
* \param [in] TAID                 -- Trusted Application Identifier
* \param [in] opaqueCredentials    -- Unused
* \param [out] session             -- Initialized session structure that can be used by functions: #SKTEEC_CloseSession, #SKTEEC_InvokeCommand
*
* \return
*     - #SK_ERROR_SUCCESS on success
*/
SKTEEC_STATUS SKTEEC_OpenSession          (SKTEEC_Context* context, SKTEEC_Session* session, SK_TAID TAID, void* opaqueCredentials);

/**
* \details
* Close a session to a trusted application
*
* \param [in] session  -- The session to close
*
* \return
*     - #SK_ERROR_SUCCESS on success
*/
SKTEEC_STATUS SKTEEC_CloseSession         (SKTEEC_Session* session);

/**
* \details
* Invoke a command (pull request) on an opened session
*
* \param [in] session        -- The session to invoke the command on
* \param [in] commandID      -- An application specific command identifier (known between the normal and secure world app)
* \param [in,out] operation  -- The operation buffer setup previously to include memory references to inputs and outputs.
*
* \return
*     - #SK_ERROR_SUCCESS on success
*/
SKTEEC_STATUS SKTEEC_InvokeCommand        (SKTEEC_Session* session, uint32_t commandID, SKTEEC_Operation* operation);

/**
* \details
* For future use
*
* \param [in] context
* \param [in] memInfo
*
* \return
*     - #SK_ERROR_SUCCESS on success
*/
SKTEEC_STATUS SKTEEC_AllocateSharedMemory (SKTEEC_Context* context, SKTEEC_SharedMemory* memInfo);

/**
* \details
* For future use
*
* \param [in] context
* \param [in] memInfo
*
* \return
*     - #SK_ERROR_SUCCESS on success
*/
SKTEEC_STATUS SKTEEC_RegisterMemory       (SKTEEC_Context* context, SKTEEC_SharedMemory* memInfo);

/**
* \details
* For future use
*
* \param [in] context
* \param [in] memInfo
*
* \return
*     - #SK_ERROR_SUCCESS on success
*/
SKTEEC_STATUS SKTEEC_UnregisterMemory     (SKTEEC_Context* context, SKTEEC_SharedMemory* memInfo);

/**
 @}
*/

#endif /* _SK_TEEC_API_ */







