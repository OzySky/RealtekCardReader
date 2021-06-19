//
//  Debug.hpp
//  RealtekPCIeCardReader
//
//  Created by FireWolf on 2/11/20.
//  Revised by FireWolf on 6/9/20.
//  Adopted by FireWolf on 2/17/21 for Darwin Kernel.
//  Copyright © 2020 FireWolf. All rights reserved.
//

#ifndef Debug_h
#define Debug_h

#include <IOKit/IOLib.h>
#include <stdarg.h>

//static void OSLog(const char* format, ...)
//{
//    char buffer[1024];
//
//    buffer[0] = '\0';
//
//    va_list args;
//
//    va_start(args, format);
//
//    vsnprintf(buffer, sizeof(buffer), format, args);
//
//    va_end(args);
//
//    if (ml_get_interrupts_enabled())
//    {
//        IOLog("%s", buffer);
//
//        //IOSleep(10);
//    }
//}

#define OSLog IOLog

#include <os/log.h>

//#define OSLog(fmt, ...) \
//os_log(OS_LOG_DEFAULT, fmt, ##__VA_ARGS__);

#define MODULE "RTSX: "

#ifndef __cplusplus
    #define __PRETTY_FUNCTION__ __func__
#endif

#ifdef DEBUG
#define pinfo(fmt, ...) \
{                       \
    OSLog(MODULE "%s DInfo: " fmt "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__); \
};
#else
#define pinfo(fmt, ...) {}
#endif

#ifdef DEBUG
#define pinfof(fmt, ...) \
{                        \
    OSLog(MODULE "%s DInfo: " fmt, __PRETTY_FUNCTION__, ##__VA_ARGS__); \
};
#else
#define pinfof(fmt, ...) {}
#endif

#define perr(fmt, ...) \
{                      \
    OSLog(MODULE "%s Error: " fmt "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__); \
};

#define pwarning(fmt, ...) \
{                          \
    OSLog(MODULE "%s Warning: " fmt "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__); \
};

#define passert(cond, fmt, ...) \
{ \
    if (!(cond)) \
    { \
        OSLog(MODULE "%s Assertion Failed: " fmt "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__); \
        panic(MODULE "Assertion triggered in file %s at line %d\n", __FILE__, __LINE__); \
    } \
};

#define psoftassert(cond, fmt, ...) \
{ \
    if (!(cond)) \
    { \
        OSLog(MODULE "%s Soft Assertion Failed: " fmt "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__); \
        OSLog(MODULE "Assertion triggered in file %s at line %d\n", __FILE__, __LINE__); \
    } \
};

#define pfatal(fmt, ...) \
{ \
    panic(MODULE "%s Fatal Error: " fmt "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__); \
};

static inline void pbufcol(const void* buffer, size_t length)
{
    for (auto index = 0; index < length; index += 1)
    {
        pinfo("[%02d] 0x%02X", index, reinterpret_cast<const UInt8*>(buffer)[index]);
    }
}

static inline void pbufrow(const void* buffer, size_t length)
{
    for (auto index = 0; index < length; index += 1)
    {
        OSLog("%02X", reinterpret_cast<const UInt8*>(buffer)[index]);
    }
    
    OSLog("\n");
}

static inline void pbuf(const void* buffer, size_t length, size_t column = 8)
{
    UInt8* ptr = const_cast<UInt8*>(reinterpret_cast<const UInt8*>(buffer));
    
    auto nrows = length / column;
    
    for (auto row = 0; row < nrows; row += 1)
    {
        OSLog("RTSX: [%04lu] ", row * column);
        
        pbufrow(ptr, column);
        
        ptr += column;
    }
    
    if (nrows * column < length)
    {
        OSLog("RTSX: [%04lu] ", nrows * column);
        
        pbufrow(ptr, length - nrows * column);
    }
}

#include <IOKit/IODMACommand.h>

static inline void pdma(IODMACommand* dma, IOByteCount length, IOByteCount column)
{
    auto buffer = IOMallocZero(length);
    
    psoftassert(dma->readBytes(0, buffer, length) == length, "Failed to read the DMA contents.");
    
    pbuf(buffer, length, column);
    
    IOFree(buffer, length);
}

#endif /* Debug_h */