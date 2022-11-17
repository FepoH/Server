// #include"bytearray.h"
// #include "endian_.h"
// #include "log/log.h"

// #include <string.h>
// #include <iostream>
// #include <sstream>


// namespace fepoh{

// static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

// ByteArray::Node::Node():size(0),data(nullptr),next(nullptr){
// }

// ByteArray::Node::Node(size_t):size(size),data(new char(size)),next(nullptr){
// }

// ByteArray::Node::~Node(){
//     std::cout <<" ~Node" <<std::endl;
//     if(data){
//         delete[] data;
//     }
//     next = nullptr;
// }

// //创建一个节点
// ByteArray::ByteArray(size_t basesize,uint8_t endian)
//         :m_baseSize(basesize),m_endian(endian){
// }

// ByteArray::~ByteArray(){
//     std::cout << "~ByteArray" <<std::endl;
//     Node* tmp = m_head;
//     Node* tmpNext = nullptr;
//     while(tmp){
//         tmpNext = tmp->next;
//         delete tmp;
//         tmp = tmpNext;
//     }
//     m_head = m_write = m_read = nullptr;
// }

// void ByteArray::clear(){
//     m_write = m_read = m_head;
//     m_size = m_readBegin = 0;
// }

// void ByteArray::write(const void* buf, size_t size){
//     if(size == 0) {
//         return;
//     }
//     if(size > getCapcity()){
//         addCapcity(size + 1 - getCapcity());
//     }

//     size_t npos = m_size % m_baseSize;
//     size_t ncap = m_write->size - npos;
//     size_t bpos = 0;
//     insert();
//     if(m_write->data){
//         std::cout << "***********************" <<  m_write->data <<std::endl;
//     }else{
//         std::cout << "////////////////////////" <<std::endl;
//     }
//     //while(size > 0) {
//         // if(ncap >= size) {
//             //break;
//             memcpy(m_head->data + npos, (const char*)buf + bpos, size);
            
//         //     if(m_write->size == (npos + size)) {
//         //         m_write = m_write->next;
//         //     }
//         //     m_size += size;
//         //     bpos += size;
//         //     size = 0;
//         // } else {
//         //     memcpy(m_write->data + npos, (const char*)buf + bpos, ncap);
//         //     m_size += ncap;
//         //     bpos += ncap;
//         //     size -= ncap;
//         //     m_write = m_write->next;
//         //     ncap = m_write->size;
//         //     npos = 0;
//         // }
//     //}
//     m_size += size;
//     // if(m_position > m_size) {
//     //     m_size = m_position;
//     // }


// }








// //     //1.写的buf为空.2.大小或大小为0
// //     if(!buf || (size == 0)){
// //         return ;
// //     }
    
// //     if(size > getCapcity()){
// //         addCapcity(size + 1 - getCapcity());
// //     }
// //     int tmpSize = size;     //剩余可写数据大小
// //     size_t npos = m_size % m_baseSize;  //当前数据块起始写位置
// //     size_t ncap = m_baseSize - npos;    //当前数据块剩余容量
// //     size_t nfinish = 0;     //可写数据已完成部分
// //     while(tmpSize > 0){
// //         //如果可写的大小 <= 当前块所剩容量
// //         std::cout << tmpSize << " " << npos << "  "<< ncap << "  " << nfinish << "  " <<m_write->data << std::endl;
// //         if(tmpSize <= ncap){
// //             memcpy(m_write->data + npos ,(char*)buf + nfinish, tmpSize);
// //             if(npos + tmpSize == m_baseSize){
// //                 m_write = m_write->next;
// //             }
// //             break;
// //         }else{
// //             memcpy(m_write->data + npos ,(char*)buf + nfinish,ncap);
// //             m_write = m_write->next;
// //             tmpSize -= ncap;
// //             npos = 0;
// //             ncap = m_baseSize;
// //             nfinish += ncap;
// //         }
// //     }
// //     m_size += size; //数据部分变大
// //     insert();
// // }

// void ByteArray::addCapcity(size_t size){
//     if(size <= getCapcity()){
//         return ;
//     }
//     if(!m_head){
//         m_head = new Node(m_baseSize);
//         m_read = m_write = m_head;
//         m_capcity += m_baseSize;
//     }
//     size -= getCapcity();
//     Node* tmp = m_head;
//     while(tmp->next){
//         tmp = tmp->next;
//     }
    
//     int n = size / m_baseSize + (size % m_baseSize == 0 ? 0 : 1);
//     for(int i = 0;i < n; ++i){
//         tmp->next = new Node(m_baseSize);
//         tmp = tmp->next;
//     }
//     m_capcity += m_baseSize *n;
// }

// size_t ByteArray::read(void* buf, size_t size){
//     return read(buf,size,m_readBegin);
// }

// size_t ByteArray::read(void* buf, size_t size, size_t position){
//     if(size <= 0){
//         return 0;
//     }
//     m_readBegin = position;
//     if(size > getReadSize()){
//         size = getReadSize();
//     }

//     int n = size;
//     size_t npos = m_readBegin % m_baseSize;
//     size_t ncap = m_baseSize - npos;
//     size_t nfinish = 0;
//     while(n > 0){
//         //如果要读的大小 < 当前块所剩容量
//         if(n <= ncap){
//             memcpy((char*)buf + nfinish, m_read->data + npos,n);
//             npos += n;
//             if(npos == m_baseSize){
//                 m_read = m_read->next;
//             }
//             n = 0;
//         }else{
//             memcpy((char*)buf + nfinish,m_write->data + npos,ncap);
//             m_read = m_read->next;
//             n -= ncap;
//             npos = 0;
//             ncap = m_baseSize;
//             nfinish += ncap;
//         }
//     }
//     m_readBegin += size;
//     return size;
// }




// bool ByteArray::writeToFile(const std::string& path){
//     std::ofstream ofs;
//     ofs.open(path,std::ios::trunc | std::ios::binary);
//     if(!ofs){
//         FEPOH_LOG_ERROR(s_log_system) << "open file error." << "path = " << path
//                 <<",errno = " << errno << ":" <<strerror(errno);
//         return false;
//     }
//     std::string str;
//     str.resize(getReadSize());
//     str = tostring();
//     ofs.write(str.c_str(),getReadSize());
//     return true;
// }

// bool ByteArray::readFromFile(const std::string& path){
//     std::ifstream ifs;
//     ifs.open(path, std::ios::binary);
//     if(!ifs) {
//         FEPOH_LOG_ERROR(s_log_system) << "readFromFile path=" << path
//             << " error, errno=" << errno << " errstr=" << strerror(errno);
//         return false;
//     }
//     std::shared_ptr<char> buff(new char[m_baseSize], [](char* ptr) { delete[] ptr;});
//     while(!ifs.eof()) {
//         ifs.read(buff.get(), m_baseSize);
//         write(buff.get(), ifs.gcount());
//     }
//     return true;
// }

// std::string ByteArray::tostringAll(){
//     std::string str;
//     Node* tmpReadPtr = m_read;
//     size_t tmpReadBegin = m_readBegin;
//     str.resize(m_size);
//     read(&str[0],m_size,0);
//     m_read = tmpReadPtr;
//     m_readBegin = tmpReadBegin;
//     return str;
//     return "11";
// }

// std::string ByteArray::tostring(){
//     std::string str;
//     str.resize(getReadSize());
//     std::cout << str.size() <<std::endl;;
//     read(&str[0],getReadSize());
//     return str;
// }

// std::string ByteArray::toHexString(){
//     std::string str = tostring();
//     std::stringstream ss;
//     for(size_t i = 0; i < str.size(); ++i) {
//         if(i > 0 && i % 32 == 0) {
//             ss << std::endl;
//         }
//         ss << std::setw(2) << std::setfill('0') << std::hex
//            << (int)(uint8_t)str[i] << " ";
//     }
//     return ss.str();
// }

// std::string ByteArray::insert(){
//     std::stringstream ss;
//     ss << "m_readBegin = " << m_readBegin << ",m_capcity = " << m_capcity << ",m_size = " << m_size
//             << ",m_basesize = " <<m_baseSize;
//     FEPOH_LOG_DEBUG(s_log_system) <<ss.str() ;
//     return ss.str();
// }

// /*
//     压缩算法
// */
// static uint32_t EncodeZigzag32(const int32_t& v){
//     if(v < 0){
//         return (uint32_t(-v)) * 2 -1;
//     }else{
//         return v * 2;
//     }
// }

// static uint64_t EncodeZigzag64(const int64_t& v){
//     if(v < 0){
//         return ((uint32_t)(-v)) * 2 - 1;
//     }else{
//         return v * 2;
//     }
// }

// static int32_t DecodeZigzag32(const uint32_t& v){
//     return (v >> 1) ^ -(v & 1);
// }

// static int64_t DecodeZigzag64(const uint64_t& v){
//     return (v >> 1) ^ -(v & 1);
// }







// /*
// write
// */
// #define XX(Value,Length)    \
//     if(FEPOH_ENDIAN != m_endian){   \
//         byteswap(Value);            \
//     }                               \
//     write(&Value,Length);      

// void ByteArray::writeFint8  (int8_t value){
//     write(&value,1);
// }

// void ByteArray::writeFuint8 (uint8_t value){
//     write(&value,1);
// }

// void ByteArray::writeFint16 (int16_t value){
//     XX(value,2);
// }

// void ByteArray::writeFuint16(uint16_t value){
//     XX(value,2);
// }

// void ByteArray::writeFint32 (int32_t value){
//     XX(value,4);
// }

// void ByteArray::writeFuint32(uint32_t value){
//     XX(value,4);
// }

// void ByteArray::writeFint64 (int64_t value){
//     XX(value,8);
// }

// void ByteArray::writeFuint64(uint64_t value){
//     XX(value,8);
// }
// #undef XX

// /*
// read
// */
// int8_t      ByteArray::readFint8  (){
//     int8_t v;
//     read(&v,1);
//     return v;
// }

// uint8_t     ByteArray::readFuint8 (){
//     uint8_t v;
//     read(&v,1);
//     return v;
// }

// #define XX(Type)    \
//     Type v;                         \
//     read(&v,sizeof(Type));           \
//     if(m_endian != FEPOH_ENDIAN){   \
//         return byteswap(v);         \
//     }                               \
//     return v;                       

// int16_t     ByteArray::readFint16 (){
//     XX(int16_t);
// }

// uint16_t    ByteArray::readFuint16(){
//     XX(uint16_t);
// }

// int32_t     ByteArray::readFint32 (){
//     XX(int32_t);
// }

// uint32_t    ByteArray::readFuint32(){
//     XX(uint32_t);
// }

// int64_t     ByteArray::readFint64 (){
//     XX(int64_t);
// }

// uint64_t    ByteArray::readFuint64(){
//     XX(uint64_t);
// }
// #undef XX

// int32_t     ByteArray::readInt32  (){
//     return DecodeZigzag32(readUint32());
// }

// uint32_t    ByteArray::readUint32 (){
//     uint32_t res;
//     uint8_t tmp;
//     for(uint8_t i=0;i<32;i+=7){
//         tmp = readFuint8();
//         if(tmp < 0x80){
//             res |= (((uint32_t)(tmp & 0x7F)) << i);
//             break;
//         }
//         res |= (((uint32_t)(tmp & 0x7F)) << i);
//     }
//     return res;
// }

// int64_t     ByteArray::readInt64  (){
//     return DecodeZigzag32(readUint64());
// }

// uint64_t    ByteArray::readUint64 (){
//     uint64_t res;
//     uint8_t tmp;
//     for(uint8_t i=0;i<64;i+=7){
//         tmp = readFuint8();
//         if(tmp < 0x80){
//             res |= (((uint64_t)(tmp & 0x7F)) << i);
//             break;
//         }
//         res |= (((uint64_t)(tmp & 0x7F)) << i);
//     }
//     return res;
// }

// float       ByteArray::readFloat  (){
//     uint32_t v = readFuint32();
//     float f;
//     memcpy(&f,&v,4);
//     return f;
// }

// double      ByteArray::readDouble (){
//     uint64_t v = readFuint32();
//     double d;
//     memcpy(&d,&v,8);
//     return d;
// }




// void ByteArray::writeInt32  (int32_t value){
//     writeUint32(EncodeZigzag32(value));
    
// }

// //将4个字节用5个字节来存储
// //第5个字节中的第7位来表示后面是否还有数据
// void ByteArray::writeUint32 (uint32_t value){
//     uint8_t tmp[5];
//     uint8_t i = 0;
//     while(value >= 0x80){
//         tmp[i++] = (value & 0x7F) | 0x80;
//         value >> 7;
//     }
//     tmp[i++] = value;
//     write(tmp,i);
// }

// void ByteArray::writeInt64  (int64_t value){
//     writeUint64(EncodeZigzag64(value));
// }

// void ByteArray::writeUint64 (uint64_t value){
//     uint8_t tmp[10];
//     uint8_t i = 0;
//     while(value >= 0x80){
//         tmp[i++] = (value& 0x7F) | 0x80;
//         value >> 7;
//     }
//     tmp[i++] = value;
//     write(tmp,i);
// }

// void ByteArray::writeFloat  (float value){
//     uint32_t v;
//     memcpy(&v,&value,4);
//     writeFuint32(v);
// }

// void ByteArray::writeDouble (double value){
//     uint64_t v;
//     memcpy(&v,&value,8);
//     writeFuint64(v);
// }

// void ByteArray::writeStringF16 (const std::string& value){
//     writeFuint16(value.size());
//     write(value.c_str(),value.size());
// }

// void ByteArray::writeStringF32 (const std::string& value){
//     writeFuint32(value.size());
//     write(value.c_str(),value.size());
// }

// void ByteArray::writeStringF64 (const std::string& value){
//     writeFuint64(value.size());
//     write(value.c_str(),value.size());
// }

// // void ByteArray::writeStringWithoutLength(const std::string& value){
// //     write(value.c_str(),value.size());
// // }

// std::string ByteArray::readStringF16 (){
//     uint16_t size = readFuint16();
//     std::string str;
//     str.resize(size);
//     read(&str[0],size);
//     return str;
// }

// std::string ByteArray::readStringF32 (){
//     uint32_t size = readFuint32();
//     std::string str;
//     str.resize(size);
//     read(&str[0],size);
//     return str;
// }

// std::string ByteArray::readStringF64 (){
//     uint64_t size = readFuint64();
//     std::string str;
//     str.resize(size);
//     read(&str[0],size);
//     return str;
// }




// }//namespace


// class ByteArray{
// public:
//     typedef std::shared_ptr<ByteArray> ptr;
//     struct Node{
//         Node();
//         Node(size_t size);
//         ~Node();
//         size_t size;    //数据大小
//         char* data;     //数据
//         Node* next;     //链表指针
//     };

//     ByteArray(size_t basesize = 4096,uint8_t endian = (uint8_t)BIG_ENDIAN);
//     ~ByteArray();
//     //write
//     void writeFint8  (int8_t value);
//     void writeFuint8 (uint8_t vlaue);
//     void writeFint16 (int16_t value);
//     void writeFuint16(uint16_t value);
//     void writeFint32 (int32_t value);
//     void writeFuint32(uint32_t value);
//     void writeFint64 (int64_t value);
//     void writeFuint64(uint64_t value);
//     void writeInt32  (int32_t value);
//     void writeUint32 (uint32_t value);
//     void writeInt64  (int64_t value);
//     void writeUint64 (uint64_t value);
//     void writeFloat  (float f);
//     void writeDouble (double value);
//     void writeStringF16 (const std::string& value);
//     void writeStringF32 (const std::string& value);
//     void writeStringF64 (const std::string& value);
//     void writeString(const std::string& value);
//     //read
//     int8_t    readFint8  ();
//     uint8_t   readFuint8 ();
//     int16_t   readFint16 ();
//     uint16_t  readFuint16();
//     int32_t   readFint32 ();
//     uint32_t  readFuint32();
//     int64_t   readFint64 ();
//     uint64_t  readFuint64();
//     int32_t   readInt32  ();
//     uint32_t  readUint32 ();
//     int64_t   readInt64  ();
//     uint64_t  readUint64 ();
//     float     readFloat  ();
//     double    readDouble ();
//     std::string readStringF16 ();
//     std::string readStringF32 ();
//     std::string readStringF64 ();
//     //清空(实际改变的是指针指向,以及实际数据大小,不清空内存,重置很大几率是重用)
//     void clear();
//     //写,统一接口
//     void write(const void* buf, size_t size);
//     //读,统一接口
//     size_t read(void* buf, size_t size);
//     //从拿个位置开始读
//     size_t read(void* buf, size_t size, size_t position);
    
//     //获取数据大小
//     size_t getSize() const{return m_size;}
//     //设置数据大小
//     void setSize(size_t size){m_size = size;}
//     //设置读起始点
//     void setReadSize(size_t size){m_readBegin = size;}
//     //获取大小端
//     uint8_t getEndian() const {return m_endian;}
//     //获取可读数据大小
//     size_t getReadSize() const { return m_size - m_readBegin;}
//     //获取基础数据大小
//     size_t getBaseSize() const { return m_baseSize;}

//     //输入文件:消耗全部可读数据
//     bool writeToFile(const std::string& name);
//     //从文件中读取数据
//     bool readFromFile(const std::string& name);
//     //转为字符串:消耗数据
//     std::string tostring();
//     //读取所有数据,不消耗可读数据
//     std::string tostringAll();
//     //转为16进制字符串:消耗数据
//     std::string toHexString();
//     std::string insert();
//     // uint64_t getReadBuffers(std::vector<iovec>& buffers, uint64_t len = ~0ull) const;
//     // uint64_t getReadBuffers(std::vector<iovec>& buffers, uint64_t len, uint64_t position) const;
//     // uint64_t getWriteBuffers(std::vector<iovec>& buffers, uint64_t len);
// private:
//     //获取剩余容量大小
//     size_t getCapcity() const{return m_capcity - m_size;}
//     void addCapcity(size_t size);
// private:
//     Node* m_head = nullptr;     //头部指针
//     Node* m_write = nullptr;    //写指针
//     Node* m_read = nullptr;     //读指针

//     size_t m_readBegin = 0;     //目前已经读完的大小
//     size_t m_capcity = 0;       //总容量大小
//     size_t m_size = 0;          //数据大小

//     size_t m_baseSize;          //基础node大小                        
//     int8_t m_endian;            //存储的数据是大端还是小端
// };









***********************************************************************


