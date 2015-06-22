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

#ifndef SK_BOTH_SYSV_H_
#define SK_BOTH_SYSV_H_

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

uint32_t sysv_byte_put(uint8_t* image, uint32_t offset, uint32_t buflen, uint32_t const value_size, uint64_t value);
uint64_t sysv_byte_get(const uint8_t* image, uint32_t const value_size);

#define SYSV_SYSTEM_OK      0
#define SYSV_SYSTEM_FAILED -1

/**
* \details
*
* Returns a system specific process id for identification.
*
*
* \return
*    - Process ID.
*
*/
uint32_t      sysv_getpid    (void);

/**
* \details
*
* Returns a system specific parent process id for identification.
*
*
* \return
*    - Parent Process ID.
*
*/
uint32_t      sysv_getppid    (void);

/**
* \details
*
* Implements an ASSERT functionality, which could be simply a printf and exit.
*
* \param [in] x Condition
* \param [in] func_name Function  Name string
* \param [in] file_name File Name string
* \param [in] line_number Line Number
*
*
*/
#ifdef __clang__
   #define NORETURN __attribute__((noreturn))
#else
   #define NORETURN
#endif
#define sysv_assert(x, func_name, file_name, line_number) if (!x) { sysv_assert2(x, func_name, file_name, line_number); }
void          sysv_assert2    (uint8_t x, uint8_t const* func_name, uint8_t const* file_name, uint32_t line_number) NORETURN;

/**
* \details
*
*  Performs memory copy operation.
*
* \param [in] s1 From buffer
* \param [in] s2 To buffer
* \param [in] n Size in bytes
*
* \return
*    - should comply with the Linux prototype
*
*/
int32_t       sysv_memcmp    (const void* s1, const void* s2, uint32_t n);

/**
* \details
*
*  Fills memory with a constant byte n.
*
* \param [in] s Buffer
* \param [in] c Constant
* \param [in] n Size in bytes
*
* \return
*    - should comply with the Linux prototype
*
*/
void          *sysv_memset    (void* s, int32_t c, uint32_t n);

/**
* \details
*
*  Performs a memory copy operation.
*
* \param [in] dest From buffer
* \param [in] src To buffer
* \param [in] n Size in bytes
*
* \return
*    - should comply with the Linux prototype
*
*/
void          *sysv_memcpy    (void *dest, const void *src, uint32_t n);

/**
* \details
*
*  Performs a memory move operation.
*
* \param [in] dest From buffer
* \param [in] src To buffer
* \param [in] n Size in bytes
*
* \return
*    - should comply with the Linux prototype
*
*/
void          *sysv_memmove   (void *dest, const void *src, uint32_t n);

/**
* \details
*
*  Performs a memory string copy operation.
*
* \param [in] dest From buffer
* \param [in] src To buffer
*
* \return
*    - should comply with the Linux prototype
*
*/
uint8_t       *sysv_strcpy    (void* dest, const void* src);
/**
* \details
*
*  Performs n bytes string copy operation of n bytes.
*
* \param [in] dest From buffer
* \param [in] src To buffer
* \param [in] n No more that n bytes
*
* \return
*    - should comply with the Linux prototype
*
*/
uint8_t       *sysv_strncpy   (void* dest, const void* src, uint32_t n);

/**
* \details
*
*  Calculates the length of the provided string.
*
* \param [in] str buffer
*
*
* \return
*    - should comply with the Linux prototype
*
*/
uint32_t       sysv_strlen    (const void* str);

/**
* \details
*
*  Performs a string concatenation operation.
*
* \param [in] dest From buffer
* \param [in] src To buffer
* \param [in] n Size in bytes
*
* \return
*    - should comply with the Linux prototype
*
*/
uint8_t       *sysv_strncat   (void *dest, void *src, uint32_t n);

/**
* \details
*
*  Performs a string compare operation.
*
* \param [in] str1 String buffer
* \param [in] str2 String buffer
*
* \return
*    - should comply with the Linux prototype
*
*/
int32_t        sysv_strcmp    (const void* str1, const void* str2);

/**
* \details
*
*  Allocates dynamic memory
*
* \param [in] size Bytes
*
* \return
*    - Pointer to the allocated memory. On error return NULL.
*
*/
void          *sysv_malloc    (uint32_t size);

/**
* \details
*
*  Frees dynamic memory
*
* \param [in] ptr Memory
*
*/
void           sysv_free      (void* ptr);

/**
* \details
*
* elp_system_prng
*
* \param [in] out [TBD]
* \param [out] outlen [TBD]
*
* \return
*    - #ELP_SYSTEM_OK Success.
*    - #ELP_SYSTEM_FAILED Failed.
*
*/
uint32_t       sysv_system_prng(void *out, uint32_t outlen);

/**
* \details
*
*  Allocates memory for an array of 'nmemb' elements of 'size' bytes each.
*
* \param [in] nmemb Number of elements
* \param [in] size Element size in bytes
*
* \return
*    - Pointer to the allocated memory. On error return NULL.
*
*/
void          *sysv_calloc    (uint32_t nmemb, uint32_t size);

/**
* \details
*
*  Changes the size of the memory block pointed to by 'ptr' to 'size' bytes.
*
* \param [in] ptr Memory
* \param [in] size New size in bytes
*
* \return
*    - Pointer to the allocated memory. On error return NULL.
*
*/
void          *sysv_realloc   (void *ptr, uint32_t size);

/**
* \details
*
* Converts the unsigned integer 'hostlong' from host byte order to network byte order
*
* \param [in] hostlong Value to convert
*
* \return
*    - Converted value
*
*/
uint32_t       sysv_htonl     (uint32_t hostlong);

/**
* \details
*
* Converts the unsigned short integer 'hostshort' from host byte order to network byte order.
*
* \param [in] hostshort Value to convert
*
* \return
*    - Converted value
*
*/
uint16_t       sysv_htons     (uint16_t hostshort);

/**
* \details
*
* Converts the unsigned integer 'netlong' from network byte order to host byte order
*
* \param [in] netlong Value to convert
*
* \return
*    - Converted value
*
*/
uint32_t       sysv_ntohl     (uint32_t netlong);

/**
* \details
*
* Converts the unsigned short integer 'netshort' from network byte order to host byte order
*
* \param [in] netshort Value to convert
*
* \return
*    - Converted value
*
*/
uint16_t       sysv_ntohs     (uint16_t netshort);

int32_t        sysv_ioctl_1   (int32_t d, uint32_t request, void* arg);
int32_t        sysv_open      (const char *pathname, int32_t flags);

/**
* \details
* This function will must be called to allocate and initialize the LOGGING resources.
*
* \param [in] log a DOUBLE pointer to a logging instance (in the event you need to MALLOC the memory inside this function).
* \param [in] mask a MASK parameter (please see 'elliptic_log_def.h').
* \param [in] name a string used as an identifier.
* \param [in] log_param an opaque parameter instance that is defined in elliptic_platform_specific_system.h.
*
* \return
*    - none
*
*/
void sysv_log_init(void **log, uint32_t mask, const uint8_t* name, void* log_param);
/**
* \details
* This function will must be called to release the LOGGING resources.
*
* \param [in] log a pointer to the logging instance.
*
* \return
*    - none.
*
*/
void sysv_log_end(void *log);
/**
* \details
* This function passes in information to support 0 parameters
*
* \param [in] tag a tag value.
* \param [in] log a pointer to the logging instance.
* \param [in] file a pointer to the FILE name.
* \param [in] line a pointer to the LINE number.
* \param [in] format a variadic function (defined by the MACRO's)
*
* \return
*    - none
*
*/
void sysv_log_write(uint32_t tag, void *log, uint8_t* file, uint32_t line, const char *format, ...);
/**
* \details
* This function generates a table
*
* \param [in] log a pointer to the logging instance.
* \param [in] string a string name for the table.
* \param [in] table a to the table.
* \param [in] size the size of the table.
*
* \return
*    - none
*
*/
void    sysv_log_dump(void *log, uint8_t* string, uint8_t* table, uint32_t size);
void    sysv_log_reset(void *log, uint8_t lock);
int32_t sysv_log_update_info(void *log);

#include <stdio.h>

#ifndef SYSV_NO_LOG

   #define SYSV_LOG_INIT(__h__, __mask__, __name__, __param__) sysv_log_init(__h__, __mask__, (uint8_t*)__name__, __param__);
   #define SYSV_LOG_END(__h__) sysv_log_end(__h__);
   #define SYSV_LOG_UPDATE_INFO(__h__) sysv_log_update_info(__h__)
   #define SYSV_LOG_RESET(__h__, __lock__) sysv_log_reset(__h__, __lock__)

   #define SYSV_LOG0(__h__,__stag__,__fmt__) sysv_log_write(__stag__, __h__, (uint8_t *)__FILE__, __LINE__, __fmt__);
   #define SYSV_LOG1(__h__,__stag__,__fmt__,__a__) sysv_log_write(__stag__, __h__, (uint8_t *)__FILE__, __LINE__, __fmt__, __a__);
   #define SYSV_LOG2(__h__,__stag__,__fmt__,__a__,__b__) sysv_log_write(__stag__, __h__, (uint8_t *)__FILE__, __LINE__, __fmt__, __a__, __b__);
   #define SYSV_LOG3(__h__,__stag__,__fmt__,__a__,__b__,__c__) sysv_log_write(__stag__, __h__, (uint8_t *)__FILE__, __LINE__, __fmt__, __a__, __b__, __c__);
   #define SYSV_LOG4(__h__,__stag__,__fmt__,__a__,__b__,__c__,__d__) sysv_log_write(__stag__, __h__, (uint8_t *)__FILE__, __LINE__, __fmt__, __a__, __b__, __c__, __d__);
   #define SYSV_LOG5(__h__,__stag__,__fmt__,__a__,__b__,__c__,__d__,__e__) sysv_log_write(__stag__, __h__, (uint8_t *)__FILE__, __LINE__, __fmt__, __a__, __b__, __c__, __d__, __e__);
   #define SYSV_LOG6(__h__,__stag__,__fmt__,__a__,__b__,__c__,__d__,__e__,__f__) sysv_log_write(__stag__, __h__, (uint8_t *)__FILE__, __LINE__, __fmt__, __a__, __b__, __c__, __d__, __e__, __f__);
   #define SYSV_LOG7(__h__,__stag__,__fmt__,__a__,__b__,__c__,__d__,__e__,__f__,__g__) sysv_log_write(__stag__, __h__, (uint8_t *)__FILE__, __LINE__, __fmt__, __a__, __b__, __c__, __d__, __e__, __f__, __g__);
   #define SYSV_LOG8(__h__,__stag__,__fmt__,__a__,__b__,__c__,__d__,__e__,__f__,__g__,__i__) sysv_log_write(__stag__, __h__, (uint8_t *)__FILE__, __LINE__, __fmt__, __a__, __b__, __c__, __d__, __e__, __f__, __g__,__i__);
   #define SYSV_LDUMP(__h__,__n__,__t__,__s__) sysv_log_dump(__h__, (uint8_t*)__n__, __t__, __s__);
#else
   #define SYSV_LOG_INIT(__h__, __mask__, __name__, __param__)
   #define SYSV_LOG_END(__h__)
   #define SYSV_LOG_UPDATE_INFO(__h__) 0
   #define SYSV_LOG_RESET(__h__, __lock__)
   #define SYSV_LOG0(__h__,__stag__,__fmt__)
   #define SYSV_LOG1(__h__,__stag__,__fmt__,__a__)
   #define SYSV_LOG2(__h__,__stag__,__fmt__,__a__,__b__)
   #define SYSV_LOG3(__h__,__stag__,__fmt__,__a__,__b__,__c__)
   #define SYSV_LOG4(__h__,__stag__,__fmt__,__a__,__b__,__c__,__d__)
   #define SYSV_LOG5(__h__,__stag__,__fmt__,__a__,__b__,__c__,__d__,__e__)
   #define SYSV_LOG6(__h__,__stag__,__fmt__,__a__,__b__,__c__,__d__,__e__,__f__)
   #define SYSV_LOG7(__h__,__stag__,__fmt__,__a__,__b__,__c__,__d__,__e__,__f__,__g__)
   #define SYSV_LOG8(__h__,__stag__,__fmt__,__a__,__b__,__c__,__d__,__e__,__f__,__g__,__i__)
   #define SYSV_LDUMP(__h__,__n__,__t__,__s__)
#endif

#define SYSV_ERROR0(__h__, __stag__, __fmt__) SYSV_LOG0(__h__,__stag__,__fmt__)
#define SYSV_ERROR1(__h__, __stag__, __fmt__, __a__) SYSV_LOG1(__h__,__stag__,__fmt__,__a__)
#define SYSV_ERROR2(__h__, __stag__, __fmt__, __a__ ,__b__) SYSV_LOG2(__h__, __stag__, __fmt__, __a__, __b__)
#define SYSV_ERROR3(__h__, __stag__, __fmt__,__a__,__b__,__c__) SYSV_LOG3(__h__,__stag__,__fmt__,__a__,__b__,__c__)
#define SYSV_ERROR4(__h__, __stag__, __fmt__,__a__,__b__,__c__,__d__) SYSV_LOG4(__h__,__stag__,__fmt__,__a__,__b__,__c__,__d__)
#define SYSV_ERROR5(__h__, __stag__, __fmt__,__a__,__b__,__c__,__d__,__e__) SYSV_LOG5(__h__,__stag__,__fmt__,__a__,__b__,__c__,__d__,__e__)
#define SYSV_ERROR6(__h__, __stag__, __fmt__,__a__,__b__,__c__,__d__,__e__,__f__) SYSV_LOG6(__h__,__stag__,__fmt__,__a__,__b__,__c__,__d__,__e__,__f__)
#define SYSV_ERROR7(__h__, __stag__, __fmt__,__a__,__b__,__c__,__d__,__e__,__f__,__g__) SYSV_LOG7(__h__,__stag__,__fmt__,__a__,__b__,__c__,__d__,__e__,__f__,__g__)

#ifndef SYSV_NO_DEBUG_LOG
#define SYSV_INFO0(__h__,__fmt__) SYSV_LOG0(__h__,SYSV_LOG_INFO, __fmt__)
#define SYSV_INFO1(__h__,__fmt__,__a__) SYSV_LOG1(__h__,SYSV_LOG_INFO, __fmt__,__a__)
#define SYSV_INFO2(__h__,__fmt__,__a__,__b__) SYSV_LOG2(__h__,SYSV_LOG_INFO, __fmt__,__a__,__b__)
#define SYSV_INFO3(__h__,__fmt__,__a__,__b__,__c__) SYSV_LOG3(__h__,SYSV_LOG_INFO, __fmt__,__a__,__b__,__c__)
#define SYSV_INFO4(__h__,__fmt__,__a__,__b__,__c__,__d__) SYSV_LOG4(__h__,SYSV_LOG_INFO, __fmt__,__a__,__b__,__c__,__d__)
#define SYSV_INFO5(__h__,__fmt__,__a__,__b__,__c__,__d__,__e__) SYSV_LOG5(__h__,SYSV_LOG_INFO, __fmt__,__a__,__b__,__c__,__d__,__e__)
#define SYSV_INFO6(__h__,__fmt__,__a__,__b__,__c__,__d__,__e__,__f__) SYSV_LOG6(__h__,SYSV_LOG_INFO, __fmt__,__a__,__b__,__c__,__d__,__e__,__f__)
#define SYSV_INFO7(__h__,__fmt__,__a__,__b__,__c__,__d__,__e__,__f__,__g__) SYSV_LOG7(__h__,SYSV_LOG_INFO, __fmt__,__a__,__b__,__c__,__d__,__e__,__f__,__g__)
#define SYSV_INFO8(__h__,__fmt__,__a__,__b__,__c__,__d__,__e__,__f__,__g__,__i__) SYSV_LOG8(__h__,SYSV_LOG_INFO, __fmt__,__a__,__b__,__c__,__d__,__e__,__f__,__g__,__i__)
#define SYSV_DUMP(__h__,__name__,__table__,__size__) SYSV_LDUMP(__h__,__name__,__table__,__size__)

#else
#define SYSV_INFO0(__h__,__fmt__)
#define SYSV_INFO1(__h__,__fmt__,__a__)
#define SYSV_INFO2(__h__,__fmt__,__a__,__b__)
#define SYSV_INFO3(__h__,__fmt__,__a__,__b__,__c__)
#define SYSV_INFO4(__h__,__fmt__,__a__,__b__,__c__,__d__)
#define SYSV_INFO5(__h__,__fmt__,__a__,__b__,__c__,__d__,__e__)
#define SYSV_INFO6(__h__,__fmt__,__a__,__b__,__c__,__d__,__e__,__f__)
#define SYSV_INFO7(__h__,__fmt__,__a__,__b__,__c__,__d__,__e__,__f__,__g__)
#define SYSV_INFO8(__h__,__fmt__,__a__,__b__,__c__,__d__,__e__,__f__,__g__,__i__)

#define SYSV_DUMP(h,name,table,size)
#endif

#define SYSV_LOG_NONE         0x00000000
#define SYSV_LOG_CRITICAL_ERR 0x00000001
#define SYSV_LOG_WARNING_ERR  0x00000002
#define SYSV_LOG_INFO         0x00000004
#define SYSV_LOG_TIME         0x00000008
#define SYSV_LOG_DUMP         0x00000010
#define SYSV_LOG_EXTERNAL     0x00000020
#define SYSV_LOG_INFO2        0x00000040  /* sensitive material here */
#define SYSV_LOG_ALL         (SYSV_LOG_INFO2       |\
                              SYSV_LOG_EXTERNAL    |\
                              SYSV_LOG_DUMP        |\
                              SYSV_LOG_TIME        |\
                              SYSV_LOG_INFO        |\
                              SYSV_LOG_WARNING_ERR |\
                              SYSV_LOG_CRITICAL_ERR)

/* System Definitions */
#define SYSV_MEMSET(__x__,__y__,__z__)             sysv_memset((__x__),(__y__),(__z__))
#define SYSV_MEMCMP(__x__,__y__,__z__)             sysv_memcmp((__x__),(__y__),(__z__))
#define SYSV_MEMCPY(__x__,__y__,__z__)             sysv_memcpy((__x__),(__y__),(__z__))
#define SYSV_MEMMOVE(__x__,__y__,__z__)            sysv_memmove((__x__),(__y__),(__z__))
#define SYSV_STRCPY(__x__,__y__)                   sysv_strcpy((__x__),(__y__))
#define SYSV_STRNCPY(__x__,__y__,__z__)            sysv_strncpy((__x__),(__y__),(__z__))
#define SYSV_STRLEN(__x__)                         sysv_strlen((__x__))
#define SYSV_STRCMP(__x__,__y__)                   sysv_strcmp((__x__),(__y__))
#define SYSV_NTOHL(__x__)                          sysv_ntohl((__x__))
#define SYSV_NTOHS(__x__)                          sysv_ntohs((__x__))
#define SYSV_HTONL(__x__)                          sysv_htonl((__x__))
#define SYSV_HTONS(__x__)                          sysv_htons((__x__))
#define SYSV_MALLOC(__x__)                         sysv_malloc((__x__))
#define SYSV_CALLOC(__x__,__y__)                   sysv_calloc((__x__),(__y__))
#define SYSV_REALLOC(__x__,__y__)                  sysv_realloc((__x__),(__y__))
#define SYSV_FREE(__x__)                           sysv_free((__x__))
#define SYSV_GETPID                                sysv_getpid()
#define SYSV_GETPPID                               sysv_getppid()
#define SYSV_IOCTL_1(__x__,__y__,__z__)            sysv_ioctl_1((__x__),(__y__),(__z__))
#define SYSV_OPEN(__x__,__y__)                     sysv_open((__x__),(__y__))
#define SYSV_CLOSE(__x__)                          sysv_close((__x__))

#ifdef RELEASE
#define SYSV_ASSERT(__x__)
#else
#define SYSV_ASSERT(__x__)                         sysv_assert((__x__),  (void *)__func__ , (void *)__FILE__, __LINE__)
#endif


#endif
