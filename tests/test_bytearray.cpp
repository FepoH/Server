#include "bytearray.h"
#include "log/log.h"
#include "macro.h"

using namespace fepoh;


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
    srand(time(0)); 
    XX(int8_t,  1000, writeFint8, readFint8, 1000);
    XX(uint8_t, 1000, writeFuint8, readFuint8, 1000);
    XX(int16_t,  1000, writeFint16,  readFint16, 1000);
    XX(uint16_t, 1000, writeFuint16, readFuint16, 1000);
    XX(int32_t,  1000, writeFint32,  readFint32, 1000);
    XX(uint32_t, 1000, writeFuint32, readFuint32, 1000);
    XX(int64_t,  1000, writeFint64,  readFint64, 1000);
    XX(uint64_t, 1000, writeFuint64, readFuint64, 1000);

    XX(int32_t,  1000, writeInt32,  readInt32, 1000);
    XX(uint32_t, 1000, writeUint32, readUint32, 1000);
    XX(int64_t,  1000, writeInt64,  readInt64, 1000);
    XX(uint64_t, 1000, writeUint64, readUint64, 1000);
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
    for(size_t i = 0; i < vec.size(); ++i) { \
        type v = ba->read_fun(); \
        FEPOH_ASSERT(v == vec[i]); \
    } \
    FEPOH_ASSERT(ba->getReadSize() == 0); \
    FEPOH_LOG_INFO(s_log_system) << #write_fun "/" #read_fun \
                    " (" #type " ) len=" << len \
                    << " base_len=" << base_len \
                    << " size=" << ba->getSize(); \
    FEPOH_ASSERT(ba->writeToFile("/tmp/" #type "_" #len "-" #read_fun ".dat")); \
    fepoh::ByteArray::ptr ba2(new fepoh::ByteArray(base_len * 2)); \
    FEPOH_ASSERT(ba2->readFromFile("/tmp/" #type "_" #len "-" #read_fun ".dat")); \
    FEPOH_ASSERT(ba->toString() == ba2->toString()); \
}
    XX(int8_t,  1000, writeFint8, readFint8, 1000);
    XX(uint8_t, 1000, writeFuint8, readFuint8, 1000);
    XX(int16_t,  1000, writeFint16,  readFint16, 1000);
    XX(uint16_t, 1000, writeFuint16, readFuint16, 1000);
    XX(int32_t,  100000, writeFint32,  readFint32, 1000);
    XX(uint32_t, 100000, writeFuint32, readFuint32, 1000);
    XX(int64_t,  100000, writeFint64,  readFint64, 1000);
    XX(uint64_t, 100000, writeFuint64, readFuint64, 1000);
//压缩算法对32位反而增大,对64,压缩后,数据大小几乎打了对折,非常奇怪
    XX(int32_t,  100000, writeInt32,  readInt32, 1000);
    XX(uint32_t, 100000, writeUint32, readUint32, 1000);
    XX(int64_t,  100000, writeInt64,  readInt64, 1000);
    XX(uint64_t, 100000, writeUint64, readUint64, 1000);

#undef XX
}

void test3(){
    FEPOH_ASSERT(false);
}

void test2(){
    test3();
}

void test1(){
    //测试文件输出
    ByteArray::ptr ba(new ByteArray());
    std::string str = "2022-11-17 16:32:5Fuint32/readFuint32 (uint32_t ) len=1000 base_len=1000 size=4000"
"2022-11-17 16:32:55     16324   UNKOWNint64_t ) len=1000 base_len=1000 size=8000"
"2022-11-17 16:32:55     16324   UNKOWN (uint64_t ) len=1000 base_len=1000 size=8000"
"2022-11-17 16:32:55     16324   UNKOWNt32_t ) len=1000 base_len=1000 size=4931"
"2022-11-17 16:32:55     16324   UNKOWNuint32_t ) len=1000 base_len=1000 size=4854"
"2022-11-17 16:32:55     16324   UNKOWNt64_t ) len=1000 base_len=1000 size=4944"
"2022-11-17 16:32:55     16324   UNKOWNuint64_t ) len=1000 base_len=1000 size=4893";
    ba->writeStringF32(str);
    ba->writeToFile("/tmp/aaa");
    ba->setReadPos(10);
    ba->writeToFile("/tmp/bbb");
    //测试backtrace
    //test2();
}

int main(int argc, char** argv) {
    test();
    std::cout << "f------------------------------" << std::endl;
    test1();
    return 0;
}
