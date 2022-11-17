#pragma once

#include <byteswap.h>
#include <stdint.h>
#include <type_traits>

namespace fepoh{

template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t) ,T>::type 
byteswap(T value){
    return (T)bswap_64(value);
}

template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t) ,T>::type 
byteswap(T value){
    return (T)bswap_32(value);
}

template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t) ,T>::type 
byteswap(T value){
    return (T)bswap_16(value);
}

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