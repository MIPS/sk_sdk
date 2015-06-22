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
#ifndef _SK_BOTH_H_
#define _SK_BOTH_H_

#define SK_ERROR_SUCCESS                    0
#define SK_ERROR_FAILED                    -1
#define SK_ERROR_INVALID_CMD               -2
#define SK_ERROR_INVALID_PARAM             -3
#define SK_ERROR_OPEN_COMM                 -4
#define SK_ERROR_CLOSE_COMM                -5
#define SK_ERROR_OPEN_CONTEXT              -6
#define SK_ERROR_CLOSE_CONTEXT             -7
#define SK_ERROR_OPEN_SESSION              -8
#define SK_ERROR_CLOSE_SESSION             -9
#define SK_ERROR_INV_CMD                  -10
#define SK_ERROR_REGMEM_CMD               -11
#define SK_ERROR_UNREGMEM_CMD             -12
#define SK_ERROR_RECV_COMM                -13
#define SK_ERROR_SEND_COMM                -14
#define SK_ERROR_FAILED_GEN_HANDLE        -15
#define SK_ERROR_CONTEXT_HANDLE_NOT_FOUND -16
#define SK_ERROR_SESSION_HANDLE_NOT_FOUND -17
#define SK_ERROR_VERIFY_FAILED            -18
#define SK_ERROR_PRNG_INIT_FAILED         -19
#define SK_ERROR_PRNG_READ_FAILED         -20
#define SK_ERROR_PRNG_END_FAILED          -21
#define SK_ERROR_NOT_FOUND                -22
#define SK_ERROR_NO_MEMORY                -23
#define SK_ERROR_FAILED_REG               -24
#define SK_ERROR_FAILED_PLATCMD_TAID_REG  -25
#define SK_ERROR_BAD_CONTEXT_ID           -26
#define SK_ERROR_BAD_SM_ID                -27
#define SK_ERROR_OUT_OF_RANGE             -28
#define SK_ERROR_RECV_FROM_CONTEXT        -29
#define SK_ERROR_SEND_TO_DRIVER           -30
#define SK_ERROR_NOT_INITIALIZED          -31
#define SK_ERROR_STARTING_TAPI2TAPP_INT   -32
#define SK_ERROR_FAILED_SHARED_MEMORY_REQ -33
#define SK_ERROR_CHECK_CMD                -34
#define SK_ERROR_TAPP_LOAD                -35
#define SK_ERROR_TAPP_INIT                -36
#define SK_ERROR_TAPP_CLOSE               -37
#define SK_ERROR_TAPP_CALL                -38
#define SK_ERROR_TAPP_TIMEOUT             -39
#define SK_ERROR_TIMEOUT                  -40
#define SK_ERROR_OPEN_FAILED              -41
#define SK_ERROR_READ_FAILED              -42
#define SK_ERROR_HEADER_FAILED            -43
#define SK_ERROR_INVALID_COOKIE           -44
#define SK_ERROR_ENTRY_FAILED             -45
#define SK_ERROR_ENTRY_NOT_FOUND          -46
#define SK_ERROR_LOAD_FAILED              -47
#define SK_ERROR_UNLOAD_FAILED            -48
#define SK_ERROR_COMM_READ_FAILED         -49
#define SK_ERROR_COMM_SEND_FAILED         -50
#define SK_ERROR_COMM_FAILED              -51
#define SK_ERROR_DRIVER_OPEN_FAILED       -52

#include <sysv_platform_specific_system_type.h>
#include <sk_both_sysv.h>
#include <sk_both_tapi.h>

#endif
