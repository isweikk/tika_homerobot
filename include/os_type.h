#ifndef __OS_TYPE_H_
#define __OS_TYPE_H_

#define OS_API __attribute__ ((visibility("default")))

typedef char       INT8;
typedef short      INT16;
typedef int        INT32;
typedef long long  INT64;

typedef unsigned char        UINT8;
typedef unsigned short       UINT16;
typedef unsigned int         UINT32;
typedef unsigned long long   UINT64;


#define OS_INT8_MAX         127
#define OS_INT8_MIN         (-127 - (-1))

#define OS_UINT8_MAX        255
#define OS_UINT8_MIN        0

#define OS_INT16_MAX        32767
#define OS_INT16_MIN        ((-32767) - (-1))

#define OS_UINT16_MAX       65535
#define OS_UINT16_MIN       0

#define OS_INT32_MAX        2147483647
#define OS_INT32_MIN        (-2147483647 - (-1))

#define OS_UINT32_MAX       4294967295
#define OS_UINT32_MIN       0

#define OS_INT64_MAX        (9223372036854775807)
#define OS_INT64_MIN        (-9223372036854775807 - (-1))

#define OS_UINT64_MAX       18446744073709551615
#define OS_UINT64_MIN       0

/* define invalid value of int */
#define NULL_UINT8          255
#define NULL_UINT16         65535
#define NULL_UINT32         4294967295
#define NULL_UINT64         18446744073709551615

/* define invalid pointer */
#if defined(__cplusplus)
#define    NULL_PTR     nullptr
#else
#define    NULL_PTR     NULL
#endif

#define    __STATIC__    static
#define    __CONST__     const
#define    __FAST__      register
#define    __EXTERN__    extern

/* OS function return value definiation    */
#define    OS_OK           0L
#define    OS_FAIL         1L
#define    OS_ERROR        ((int)0xffffffff)


#if 1//(OS_ARCH_ENDIAN == OS_ARCH_LITTLE_ENDIAN)

#define OS_HOST_TO_NET32(_host) \
    ((((_host) & 0x000000ff) << 24) | \
    (((_host) & 0x0000ff00) <<  8) | \
    (((_host) & 0x00ff0000) >>  8) | \
    (((_host) & 0xff000000) >> 24))
    
#define OS_HOST_TO_NET16(_host)  ((((_host) & 0x00ff) << 8) | (((_host) & 0xff00) >> 8))

#define OS_NET_TO_HOST32(_network) \
    ((((_network) & 0x000000ff) << 24) | \
    (((_network) & 0x0000ff00) <<  8) | \
    (((_network) & 0x00ff0000) >>  8) | \
    (((_network) & 0xff000000) >> 24))
    
#define OS_NET_TO_HOST16(_network) ((((_network) & 0x00ff) << 8) | (((_network) & 0xff00) >> 8))

#elif (OS_ARCH_ENDIAN == OS_ARCH_BIG_ENDIAN)

#define OS_HOST_TO_NET16(_host) (_host)
#define OS_HOST_TO_NET32(_host) (_host)
#define OS_NET_TO_HOST16(_network) (_network)
#define OS_NET_TO_HOST32(_network) (_network)

#endif

#define OS_GET_STRUCT(_structTypedef, _fieldName, _fieldPtr) \
    ((_structTypedef *)(((_CHAR8*)(_fieldPtr))-offsetof(_structTypedef,_fieldName)))

#define OS_NELEMENTS(array)        (sizeof (array) / sizeof ((array) [0]))

#define OS_MAX(x, y)    (((x) < (y)) ? (y) : (x))
#define OS_MIN(x, y)    (((x) < (y)) ? (x) : (y))

#define OS_ROUND_UP(x, align)      (((int) (x) + (align - 1)) & ~(align - 1))
#define OS_ROUND_DOWN(x, align)    ((int)(x) & ~(align - 1))
#define OS_ALIGNED(x, align)       (((int)(x) & (align - 1)) == 0)

#define OS_ALIGN16(_x) (OS_ROUND_UP(_x,2))
#define OS_ALIGN32(_x) (OS_ROUND_UP(_x,4))
#define OS_ALIGN64(_x) (OS_ROUND_UP(_x,8))


#define OS_ALIGN_DATASIZE        (sizeof(char))
#define OS_ALIGN_PRTSIZE         (sizeof(void *))
#define OS_ALIGN_SIZE            (OS_MAX(OS_ALIGN_DATASIZE, OS_ALIGN_PRTSIZE))


#define OS_DECLARE_HANDLE(_handleName) \
        typedef struct { void* pUnused; } _handleName##__ ; \
        typedef __CONST__ _handleName##__ * _handleName; \
        typedef _handleName*  LP##_handleName

#define OS_UNUSED_ARG(_arg) if (_arg);
#define IN
#define OUT
#define INOUT

#define USE_STD_PRINT 1

#if USE_STD_PRINT	//default log
#include <stdio.h>

#define ESC_START     "\033["
#define ESC_END       "\033[0m"
#define COLOR_FATAL   "31;40;5m"
#define COLOR_ALERT   "31;40;1m"
#define COLOR_CRIT    "31;40;1m"
#define COLOR_ERROR   "31;40;1m"
#define COLOR_WARN    "33;40;1m"
#define COLOR_NOTICE  "34;40;1m"
#define COLOR_INFO    "32;40;1m"
#define COLOR_DEBUG   "36;40;1m"
#define COLOR_TRACE   "37;40;1m"

#define  LOG_LV_FATAL   0
#define  LOG_LV_ERROR   1
#define  LOG_LV_WARN    2
#define  LOG_LV_NOTICE  3
#define  LOG_LV_INFO    4
#define  LOG_LV_DBG     5
#define  LOG_LV_TRACE   6

#ifdef OS_DEBUG
#define LOG_LEVEL   LOG_LV_TRACE
#else
#define LOG_LEVEL   LOG_LV_NOTICE
#endif

#ifdef _WIN32
#define _PRINT_END_LF "\r\n"
#define __FILENAME__ (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1):__FILE__)
#define PRINT_FUNC   printf //UtLogMsgA
#else
#define _PRINT_END_LF "\n"
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#define PRINT_FUNC   printf
#endif

#if LOG_LEVEL>=LOG_LV_ERROR
#define LogError(fmt, ...)  PRINT_FUNC("[ERROR ] [%s :%04d] " fmt _PRINT_END_LF, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define LogError(fmt, ...)
#endif

#if LOG_LEVEL>=LOG_LV_WARN
#define LogWarn(fmt, ...)   PRINT_FUNC("[WARN  ] [%s :%04d] " fmt _PRINT_END_LF, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define LogWarn(fmt, ...)
#endif

#if LOG_LEVEL>=LOG_LV_NOTICE
#define LogNotice(fmt, ...)   PRINT_FUNC("[NOTICE] [%s :%04d] " fmt _PRINT_END_LF, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define LogNotice(fmt, ...)
#endif


#if LOG_LEVEL>=LOG_LV_INFO
#define LogInfo(fmt, ...)   PRINT_FUNC("[INFO  ] [%s :%04d] " fmt _PRINT_END_LF, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define LogInfo(fmt, ...)
#endif

#if LOG_LEVEL>=LOG_LV_DBG
#define LogDbg(fmt, ...)    PRINT_FUNC("[DEBUG ] [%s :%04d] " fmt _PRINT_END_LF, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define LogDbg(fmt, ...)
#endif

#if LOG_LEVEL>=LOG_LV_TRACE
#define LogTrace()  PRINT_FUNC("\033[1;31;40m[TRACE ] [%s : %04d] $@$@$@$\033[0m" _PRINT_END_LF, __FUNCTION__, __LINE__)
#else
#define LogTrace()
#endif

#endif //default log


#endif	//__OS_TYPE_H
