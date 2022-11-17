#include "bytearray.h"
#include "log/log.h"
#include "macro.h"

using namespace fepoh;

// void test(){
//     ByteArray b(1024);
//     for(int i=0;i<100;++i){
//         srand(time(0));
//         uint32_t tmpI = 30;
//         b.writeFuint32(tmpI);
//     }
//     FEPOH_LOG_INFO(s_log_system) << b.tostring();
// }



// int main(){
//     test();
// }




static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

void test() {
#define XX(type, len, write_fun, read_fun, base_len) {\
    std::vector<type> vec; \
    for(int i = 0; i < len; ++i) { \
        vec.push_back(rand()); \
    } \
    fepoh::ByteArray::ptr ba(new fepoh::ByteArray(base_len)); \
    for(auto& i : vec) { \
        ba->write_fun(i); \
    } \
    ba->setPosition(0); \
    for(size_t i = 0; i < vec.size(); ++i) { \
        type v = ba->read_fun(); \
        FEPOH_ASSERT(v == vec[i]); \
    } \
    FEPOH_ASSERT(ba->getReadSize() == 0); \
    FEPOH_LOG_INFO(s_log_system) << #write_fun "/" #read_fun \
                    " (" #type " ) len=" << len \
                    << " base_len=" << base_len \
                    << " size=" << ba->getSize(); \
}

    XX(int8_t,  1000, writeFint8, readFint8, 1);
    XX(uint8_t, 1000, writeFuint8, readFuint8, 1);
    XX(int16_t,  1000, writeFint16,  readFint16, 1);
    XX(uint16_t, 1000, writeFuint16, readFuint16, 1);
    XX(int32_t,  1000, writeFint32,  readFint32, 1);
    XX(uint32_t, 1000, writeFuint32, readFuint32, 1);
    XX(int64_t,  1000, writeFint64,  readFint64, 1);
    XX(uint64_t, 1000, writeFuint64, readFuint64, 1);

    XX(int32_t,  1000, writeInt32,  readInt32, 1);
    XX(uint32_t, 1000, writeUint32, readUint32, 1);
    XX(int64_t,  1000, writeInt64,  readInt64, 1);
    XX(uint64_t, 1000, writeUint64, readUint64, 1);
#undef XX

#define XX(type, len, write_fun, read_fun, base_len) {\
    std::vector<type> vec; \
    for(int i = 0; i < len; ++i) { \
        vec.push_back(rand()); \
    } \
    fepoh::ByteArray::ptr ba(new fepoh::ByteArray(base_len)); \
    for(auto& i : vec) { \
        ba->write_fun(i); \
    } \
    ba->setPosition(0); \
    for(size_t i = 0; i < vec.size(); ++i) { \
        type v = ba->read_fun(); \
        FEPOH_ASSERT(v == vec[i]); \
    } \
    FEPOH_ASSERT(ba->getReadSize() == 0); \
    FEPOH_LOG_INFO(s_log_system) << #write_fun "/" #read_fun \
                    " (" #type " ) len=" << len \
                    << " base_len=" << base_len \
                    << " size=" << ba->getSize(); \
    ba->setPosition(0); \
    FEPOH_ASSERT(ba->writeToFile("/tmp/" #type "_" #len "-" #read_fun ".dat")); \
    fepoh::ByteArray::ptr ba2(new fepoh::ByteArray(base_len * 2)); \
    FEPOH_ASSERT(ba2->readFromFile("/tmp/" #type "_" #len "-" #read_fun ".dat")); \
    ba2->setPosition(0); \
    FEPOH_ASSERT(ba->toString() == ba2->toString()); \
    FEPOH_ASSERT(ba->getPosition() == 0); \
    FEPOH_ASSERT(ba2->getPosition() == 0); \
}
    XX(int8_t,  1000, writeFint8, readFint8, 1);
    XX(uint8_t, 1000, writeFuint8, readFuint8, 1);
    XX(int16_t,  1000, writeFint16,  readFint16, 1);
    XX(uint16_t, 1000, writeFuint16, readFuint16, 1);
    XX(int32_t,  1000, writeFint32,  readFint32, 1);
    XX(uint32_t, 1000, writeFuint32, readFuint32, 1);
    XX(int64_t,  1000, writeFint64,  readFint64, 1);
    XX(uint64_t, 1000, writeFuint64, readFuint64, 1);

    XX(int32_t,  1000, writeInt32,  readInt32, 1);
    XX(uint32_t, 1000, writeUint32, readUint32, 1);
    XX(int64_t,  1000, writeInt64,  readInt64, 1);
    XX(uint64_t, 1000, writeUint64, readUint64, 1);

#undef XX
}

int main(int argc, char** argv) {
    test();
    return 0;
}
