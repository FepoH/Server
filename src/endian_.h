/*
 * @Author: fepo_h
 * @Date: 2022-11-20 01:27:45
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 01:28:49
 * @FilePath: /fepoh/workspace/fepoh_server/src/endian_.h
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include <byteswap.h>
#include <stdint.h>
#include <type_traits>

namespace fepoh{
/**
 * @description: 64位byteswap
 * @return {*}
 */
template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t) ,T>::type 
byteswap(T value){
    return (T)bswap_64(value);
}

/**
 * @description: 32位byteswap
 * @return {*}
 */
template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t) ,T>::type 
byteswap(T value){
    return (T)bswap_32(value);
}

/**
 * @description: 16位byteswap
 * @return {*}
 */
template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t) ,T>::type 
byteswap(T value){
    return (T)bswap_16(value);
}

/**
 * @description: 查看本机的字节序
 * @return {*}
 */
#if BYTE_ORDER == BIG_ENDIAN
#define FEPOH_ENDIAN BIG_ENDIAN
#else
#define FEPOH_ENDIAN LITTLE_ENDIAN
#endif

#if FEPOH_ENDIAN == BIG_ENDIAN
template<class T>
T Bswap(T t){
    return t;
}

#else

template<class T>
T Bswap(T t){
    return byteswap(t);
}

#endif

}//namepsace